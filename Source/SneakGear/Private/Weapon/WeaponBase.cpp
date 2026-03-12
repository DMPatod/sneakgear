#include "Weapon/WeaponBase.h"

#include "GameFramework/Character.h"
#include "Misc/DataValidation.h"
#include "Weapon/WeaponAimProvider.h"
#include "Weapon/WeaponFireModeComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void AWeaponBase::AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName, bool bUseHolsterOffset)
{
	if (!CharacterMesh)
	{
		return;
	}

	auto Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(CharacterMesh, Rules, SocketName);
	SetActorRelativeTransform(bUseHolsterOffset ? HolsterOffset : GripOffset);
}

void AWeaponBase::StartFire()
{
	auto Interval = FireRate > 0.f ? 1.f / FireRate : 0.1f;

	GetWorldTimerManager().SetTimer(FireTimer, this, &AWeaponBase::FireOnce, Interval, true);
	FireOnce();
}

void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
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
		Context.AddError(FText::FromString(TEXT("FireRate must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (MuzzleSocketName.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("MuzzleSocketName must be set.")));
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
