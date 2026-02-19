#include "Components/CharacterWeaponComponent.h"

#include "GameFramework/Character.h"
#include "Weapon/WeaponBase.h"

UCharacterWeaponComponent::UCharacterWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InClip = FMath::Max(ClipSize, 0);

	if (!StartedWeaponClass)
	{
		return;
	}

	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	auto* World = GetWorld();
	if (!World)
	{
		return;
	}

	CurrentWeapon = World->SpawnActor<AWeaponBase>(StartedWeaponClass);
	if (!CurrentWeapon)
	{
		return;
	}

	CurrentWeapon->SetOwner(OwnerCharacter);
	AttachWeaponToSocket(HolsterSocketName);
	CurrentWeapon->OnWeaponFiredEvent().AddUObject(this, &UCharacterWeaponComponent::HandleWeaponFired);
}

void UCharacterWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->OnWeaponFiredEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UCharacterWeaponComponent::StartFire()
{
	if (CurrentWeapon && InClip > 0)
	{
		CurrentWeapon->StartFire();
	}
}

void UCharacterWeaponComponent::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void UCharacterWeaponComponent::ToggleEquip()
{
	if (!CurrentWeapon)
	{
		return;
	}

	const auto TargetSocket = CurrentWeapon->GetAttachParentSocketName() == HolsterSocketName
		                          ? HandSocketName
		                          : HolsterSocketName;

	AttachWeaponToSocket(TargetSocket);
}

void UCharacterWeaponComponent::AttachWeaponToSocket(FName SocketName) const
{
	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh() || !CurrentWeapon)
	{
		return;
	}

	CurrentWeapon->AttachToCharacter(OwnerCharacter->GetMesh(), SocketName);
}

void UCharacterWeaponComponent::Reload()
{
	InClip = FMath::Max(ClipSize, 0);
}

void UCharacterWeaponComponent::HandleWeaponFired()
{
	if (InClip <= 0)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
		return;
	}

	--InClip;
	if (InClip <= 0)
	{
		InClip = 0;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}
