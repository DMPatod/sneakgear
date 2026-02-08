#include "Components/PlayerWeaponComponent.h"

#include "GameFramework/Character.h"
#include "Weapon/WeaponBase.h"

UPlayerWeaponComponent::UPlayerWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

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
}

void UPlayerWeaponComponent::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void UPlayerWeaponComponent::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void UPlayerWeaponComponent::ToggleEquip()
{
	if (!CurrentWeapon)
	{
		return;
	}

	auto TargetSocket = CurrentWeapon->GetAttachParentSocketName() == HolsterSocketName
		                    ? HandSocketName
		                    : HolsterSocketName;

	AttachWeaponToSocket(TargetSocket);
}

void UPlayerWeaponComponent::AttachWeaponToSocket(FName SocketName) const
{
	auto* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh() || !CurrentWeapon)
	{
		return;
	}

	CurrentWeapon->AttachToCharacter(OwnerCharacter->GetMesh(), SocketName);
}
