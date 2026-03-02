#include "Characters/AI/GuardCharacter.h"

#include "AbilitySystemComponent.h"
#include "AI/GuardAIController.h"
#include "AI/GuardManagerSubsystem.h"
#include "AI/PatrolPath.h"
#include "Components/AI/GuardAwarenessComponent.h"
#include "Components/AI/GuardPatrolComponent.h"
#include "Components/CharacterWeaponComponent.h"
#include "Data/GuardArchetypeData.h"
#include "DrawDebugHelpers.h"
#include "GAS/HealthAttributeSet.h"
#include "Radar/RadarRegistrySubsystem.h"

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

	if (PatrolComponent)
	{
		PatrolComponent->ApplyToController(GetController());
	}

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}

	InitGAS();
	BindHealthDeath();
}

void AGuardCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCombatFromAwareness();
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

void AGuardCharacter::UpdateCombatFromAwareness()
{
	if (!AwarenessComponent || !WeaponComponent)
	{
		return;
	}

	const bool bCanShootTarget = AwarenessComponent->HasLineOfSight();

	if (bCanShootTarget)
	{
		if (!bIsFiringAtTarget)
		{
			WeaponComponent->StartFire();
			bIsFiringAtTarget = true;
		}
	}
	else if (bIsFiringAtTarget)
	{
		WeaponComponent->StopFire();
		bIsFiringAtTarget = false;
	}
}

void AGuardCharacter::SetTargetActor(AActor* NewTarget)
{
	if (AwarenessComponent)
	{
		AwarenessComponent->SetTargetActor(NewTarget);
	}
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

void AGuardCharacter::InitGAS()
{
	if (!AbilitySystem)
	{
		AbilitySystem = FindComponentByClass<UAbilitySystemComponent>();
		if (!AbilitySystem)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is not set on %s"), *GetName());
			return;
		}
	}

	AbilitySystem->InitAbilityActorInfo(this, this);

	auto ApplyGE = [&](TSubclassOf<UGameplayEffect> EffectClass)
	{
		if (!EffectClass)
		{
			return;
		}

		auto Context = AbilitySystem->MakeEffectContext();
		Context.AddSourceObject(this);

		auto Spec = AbilitySystem->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (Spec.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	};

	ApplyGE(GE_DefaultHealth);
}

void AGuardCharacter::BindHealthDeath()
{
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue <= 0.f)
			{
				auto TextLoc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
				auto Txt = FString::Printf(TEXT("Player Died!"));
				DrawDebugString(GetWorld(), TextLoc, Txt, nullptr, FColor::Red, 0.f, false);
			}
		});
}
