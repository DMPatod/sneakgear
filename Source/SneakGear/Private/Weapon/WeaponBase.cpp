#include "Weapon/WeaponBase.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Misc/DataValidation.h"
#include "Weapon/WeaponAimProvider.h"
#include "Weapon/WeaponFireModeComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickEnabled(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTimeSeconds = World->GetTimeSeconds();
	if (bReloadPending && ReloadCompleteTimeSeconds >= 0.f && CurrentTimeSeconds >= ReloadCompleteTimeSeconds)
	{
		HandleReloadFinished();
	}

	if (bWantsToFire && !bReloadPending && NextFireTimeSeconds >= 0.f && CurrentTimeSeconds >= NextFireTimeSeconds)
	{
		FireAndScheduleNextShot();
	}
}

void AWeaponBase::StartFire()
{
	bWantsToFire = true;
	NextFireTimeSeconds = -1.f;
	FireAndScheduleNextShot();
}

void AWeaponBase::StopFire()
{
	bWantsToFire = false;
	NextFireTimeSeconds = -1.f;
}

float AWeaponBase::Reload()
{
	StopFire();
	const float PlayedDuration = PlayReloadAnimation();
	const float ReloadDuration = PlayedDuration > 0.f
		? PlayedDuration
		: (ReloadMontage ? ReloadMontage->GetPlayLength() : 0.f);
	if (ReloadDuration <= 0.f)
	{
		HandleReloadFinished();
		return 0.f;
	}

	ScheduleReloadCompletion(ReloadDuration);
	return ReloadDuration;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (PrimaryFireModeClass)
	{
		PrimaryFireMode = NewObject<UWeaponFireModeComponent>(this, PrimaryFireModeClass);
		if (PrimaryFireMode)
		{
			PrimaryFireMode->RegisterComponent();
		}
	}
}

EDataValidationResult AWeaponBase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (ClipSize <= 0)
	{
		Context.AddError(FText::FromString(TEXT("ClipSize must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (FireRate <= 0.f)
	{
		Context.AddWarning(FText::FromString(TEXT("FireRate should be greater than 0 when FireMontage is not configured.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (FireRate <= 0.f && !FireMontage)
	{
		Context.AddError(FText::FromString(TEXT("FireMontage or a positive FireRate must be set.")));
		Result = EDataValidationResult::Invalid;
	}

	if (MuzzleSocketName.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("MuzzleSocketName must be set.")));
		Result = EDataValidationResult::Invalid;
	}

	if (AmmoType != EAmmoType::None && !ReloadMontage)
	{
		Context.AddError(FText::FromString(TEXT("ReloadMontage must be set for weapons that use ammo.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool AWeaponBase::BuildFireContext(FWeaponFireContext& OutContext) const
{
	if (!WeaponMesh)
	{
		return false;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return false;
	}

	FVector AimOrigin = FVector::ZeroVector;
	FVector AimDirection = FVector::ForwardVector;
	if (const IWeaponAimProvider* AimProvider = Cast<IWeaponAimProvider>(GetOwner()))
	{
		if (!AimProvider->GetWeaponAimData(AimOrigin, AimDirection))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	OutContext = FWeaponFireContext();
	OutContext.InstigatorPawn = OwnerPawn;
	OutContext.WeaponActor = const_cast<AWeaponBase*>(this);
	OutContext.WeaponMesh = WeaponMesh;
	OutContext.AimOrigin = AimOrigin;
	OutContext.AimDirection = AimDirection.GetSafeNormal();
	OutContext.MuzzleSocket = MuzzleSocketName;
	return !OutContext.AimDirection.IsNearlyZero();
}

void AWeaponBase::FireOnce()
{
	if (!PrimaryFireMode)
	{
		return;
	}

	FWeaponFireContext Context;
	if (!BuildFireContext(Context))
	{
		return;
	}

	PrimaryFireMode->FireOnce(Context);
	OnWeaponFiredEvent().Broadcast();
}

void AWeaponBase::HandleReloadFinished()
{
	bReloadPending = false;
	ReloadCompleteTimeSeconds = -1.f;
	OnWeaponReloadedEvent().Broadcast();
}

float AWeaponBase::GetFireInterval() const
{
	if (FireMontage)
	{
		const float MontageLength = FireMontage->GetPlayLength();
		if (MontageLength > 0.f)
		{
			return MontageLength;
		}
	}

	return FireRate > 0.f ? 1.f / FireRate : 0.f;
}

float AWeaponBase::PlayFireAnimation() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* OwnerMesh = OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr;
	UAnimInstance* AnimInstance = OwnerMesh ? OwnerMesh->GetAnimInstance() : nullptr;
	return (AnimInstance && FireMontage) ? AnimInstance->Montage_Play(FireMontage) : 0.f;
}

float AWeaponBase::PlayReloadAnimation() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* OwnerMesh = OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr;
	UAnimInstance* AnimInstance = OwnerMesh ? OwnerMesh->GetAnimInstance() : nullptr;
	return (AnimInstance && ReloadMontage) ? AnimInstance->Montage_Play(ReloadMontage) : 0.f;
}

void AWeaponBase::FireAndScheduleNextShot()
{
	PlayFireAnimation();
	FireOnce();

	const float FireInterval = GetFireInterval();
	if (FireInterval > 0.f)
	{
		if (const UWorld* World = GetWorld())
		{
			NextFireTimeSeconds = World->GetTimeSeconds() + FireInterval;
		}
	}
	else
	{
		NextFireTimeSeconds = -1.f;
	}
}

void AWeaponBase::ScheduleReloadCompletion(float Duration)
{
	if (const UWorld* World = GetWorld())
	{
		bReloadPending = true;
		ReloadCompleteTimeSeconds = World->GetTimeSeconds() + Duration;
	}
}
