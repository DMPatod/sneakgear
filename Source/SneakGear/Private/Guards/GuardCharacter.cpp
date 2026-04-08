#include "Guards/GuardCharacter.h"

#include "AbilitySystemComponent.h"
#include "Guards/GuardAIController.h"
#include "Guards/GuardManagerSubsystem.h"
#include "Guards/Patrol/PatrolPath.h"
#include "Guards/Components/GuardAwarenessComponent.h"
#include "Guards/Components/GuardPatrolComponent.h"
#include "Components/CharacterWeaponComponent.h"
#include "Guards/Data/GuardArchetypeData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Radar/RadarRegistrySubsystem.h"
#include "Misc/DataValidation.h"
#include "UI/EventLogSubsystem.h"

AGuardCharacter::AGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AGuardAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AwarenessComponent = CreateDefaultSubobject<UGuardAwarenessComponent>(TEXT("AwarenessComponent"));
	PatrolComponent = CreateDefaultSubobject<UGuardPatrolComponent>(TEXT("PatrolComponent"));
	WeaponComponent = CreateDefaultSubobject<UCharacterWeaponComponent>(TEXT("WeaponComponent"));
}

void AGuardCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!bSpawnAtLevelStart)
	{
		if (HasAuthority())
		{
			Destroy();
		}
		else
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
		return;
	}

	if (AwarenessComponent)
	{
		AwarenessComponent->InitializeFromArchetype(ArchetypeData);
	}

	if (ArchetypeData)
	{
		ReactionTimeSeconds = FMath::Max(ArchetypeData->ReactionTimeSeconds, 0.f);
		AimErrorDegrees = FMath::Clamp(ArchetypeData->AimErrorDegrees, 0.f, 45.f);
	}

	if (!ensureAlwaysMsgf(BehaviorTreeAsset, TEXT("Guard '%s' must have a BehaviorTreeAsset assigned."), *GetName()))
	{
		return;
	}

	if (HasAuthority() && Controller == nullptr)
	{
		SpawnDefaultController();
	}

	if (auto W = GetWorld())
	{
		if (auto Radar = W->GetSubsystem<URadarRegistrySubsystem>())
		{
			Radar->RegisterActor(this);
		}
		if (auto GuardManager = W->GetSubsystem<UGuardManagerSubsystem>())
		{
			GuardManager->RegisterGuard(this);
		}
	}

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}
}

void AGuardCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCombatState(DeltaSeconds);
}

void AGuardCharacter::UpdateCombatState(float DeltaSeconds)
{
	if (AwarenessComponent)
	{
		AwarenessComponent->UpdatePerception(DeltaSeconds);
	}

	const bool bHasLineOfSight = HasLineOfSight();
	if (bHasLineOfSight && !bHadLineOfSightLastTick)
	{
		LastLineOfSightAcquiredTimestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	}
	else if (!bHasLineOfSight)
	{
		LastLineOfSightAcquiredTimestamp = -1.f;
	}

	bHadLineOfSightLastTick = bHasLineOfSight;
}

#if WITH_EDITOR
void AGuardCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!PatrolComponent)
	{
		return;
	}

	PatrolComponent->DrawEditorPreview();
}
#endif

void AGuardCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (WeaponComponent)
	{
		WeaponComponent->StopFire();
	}

	if (auto W = GetWorld())
	{
		if (auto Radar = W->GetSubsystem<URadarRegistrySubsystem>())
		{
			Radar->UnregisterActor(this);
		}
		if (auto GuardManager = W->GetSubsystem<UGuardManagerSubsystem>())
		{
			GuardManager->UnregisterGuard(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

EDataValidationResult AGuardCharacter::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!BehaviorTreeAsset)
	{
		Context.AddError(FText::FromString(TEXT("Guard must have a BehaviorTreeAsset assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!BlackboardAsset && (!BehaviorTreeAsset || !BehaviorTreeAsset->BlackboardAsset))
	{
		Context.AddError(FText::FromString(TEXT("Guard must define a blackboard either explicitly or through its BehaviorTreeAsset.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!ArchetypeData)
	{
		Context.AddWarning(FText::FromString(TEXT("Guard has no ArchetypeData assigned; awareness values will use component defaults.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

void AGuardCharacter::SetCombatFiringEnabled(bool bEnabled)
{
	if (!WeaponComponent)
	{
		return;
	}

	if (bEnabled)
	{
		if (!CanStartCombatFiring())
		{
			return;
		}

		if (!bIsFiringAtTarget)
		{
			WeaponComponent->StartFire();
			if (UEventLogSubsystem* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
			{
				EventLog->ReportGuardStartedFiring(this, GetTargetActor());
			}
			bIsFiringAtTarget = true;
		}
	}
	else if (bIsFiringAtTarget)
	{
		WeaponComponent->StopFire();
		bIsFiringAtTarget = false;
	}
}

bool AGuardCharacter::CanStartCombatFiring() const
{
	if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon() || !HasLineOfSight())
	{
		return false;
	}

	if (ReactionTimeSeconds <= 0.f)
	{
		return true;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	if (LastLineOfSightAcquiredTimestamp < 0.f)
	{
		return HasLineOfSight();
	}

	return (World->GetTimeSeconds() - LastLineOfSightAcquiredTimestamp) >= ReactionTimeSeconds;
}

bool AGuardCharacter::IsWeaponClipEmpty() const
{
	return WeaponComponent && WeaponComponent->GetCurrentWeapon() && WeaponComponent->GetInClip() <= 0;
}

bool AGuardCharacter::ReloadWeaponIfNeeded()
{
	if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon())
	{
		return false;
	}

	if (!IsWeaponClipEmpty())
	{
		return false;
	}

	WeaponComponent->StopFire();
	bIsFiringAtTarget = false;
	WeaponComponent->Reload();
	return true;
}

void AGuardCharacter::SetTargetActor(AActor* NewTarget)
{
	if (AwarenessComponent)
	{
		AwarenessComponent->SetTargetActor(NewTarget);
	}

	bHadLineOfSightLastTick = false;
	LastLineOfSightAcquiredTimestamp = -1.f;
}

void AGuardCharacter::SetPatrolPath(APatrolPath* NewPatrolPath)
{
	if (PatrolComponent)
	{
		PatrolComponent->SetPatrolPath(NewPatrolPath);
	}
}

void AGuardCharacter::AddAwareness(float DeltaAwareness)
{
	if (AwarenessComponent)
	{
		AwarenessComponent->AddAwareness(DeltaAwareness);
	}
}

float AGuardCharacter::GetAwareness() const
{
	return AwarenessComponent ? AwarenessComponent->GetAwareness() : 0.f;
}

float AGuardCharacter::GetVisionRange() const
{
	return AwarenessComponent ? AwarenessComponent->GetVisionRange() : 0.f;
}

float AGuardCharacter::GetHearingRange() const
{
	return AwarenessComponent ? AwarenessComponent->GetHearingRange() : 0.f;
}

bool AGuardCharacter::HasLineOfSight() const
{
	return AwarenessComponent && AwarenessComponent->HasLineOfSight();
}

EGuardAwarenessState AGuardCharacter::GetAwarenessState() const
{
	return AwarenessComponent ? AwarenessComponent->GetAwarenessState() : EGuardAwarenessState::Calm;
}

AActor* AGuardCharacter::GetTargetActor() const
{
	return AwarenessComponent ? AwarenessComponent->GetTargetActor() : nullptr;
}

AWeaponBase* AGuardCharacter::GetCurrentWeapon() const
{
	return WeaponComponent ? WeaponComponent->GetCurrentWeapon() : nullptr;
}

bool AGuardCharacter::GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const
{
	FRotator EyeRotation;
	GetActorEyesViewPoint(OutAimOrigin, EyeRotation);

	if (AwarenessComponent)
	{
		if (const AActor* TargetActor = AwarenessComponent->GetTargetActor())
		{
			const FVector TargetPoint = TargetActor->GetActorLocation() + FVector(0.f, 0.f, 60.f);
			OutAimDirection = (TargetPoint - OutAimOrigin).GetSafeNormal();
			if (!OutAimDirection.IsNearlyZero())
			{
				if (AimErrorDegrees > KINDA_SMALL_NUMBER)
				{
					OutAimDirection = FMath::VRandCone(OutAimDirection,
						FMath::DegreesToRadians(AimErrorDegrees));
				}
				return true;
			}
		}
	}

	OutAimDirection = EyeRotation.Vector().GetSafeNormal();
	if (!OutAimDirection.IsNearlyZero())
	{
		return true;
	}

	OutAimDirection = GetActorForwardVector().GetSafeNormal();
	return !OutAimDirection.IsNearlyZero();
}

void AGuardCharacter::OnCharacterDeath()
{
	Super::OnCharacterDeath();

	bIsFiringAtTarget = false;
	if (WeaponComponent)
	{
		WeaponComponent->StopFire();
	}
}
