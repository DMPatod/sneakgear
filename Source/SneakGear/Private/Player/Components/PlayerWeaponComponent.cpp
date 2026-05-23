#include "Player/Components/PlayerWeaponComponent.h"

#include "Player/Components/PlayerInventoryComponent.h"

UPlayerWeaponComponent::UPlayerWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerWeaponComponent::SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand)
{
	UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->SetActiveWeaponSlot(WeaponSlot, bEquipInHand) : false;
}

bool UPlayerWeaponComponent::SetWeaponEquipped(bool bNewEquipped)
{
	UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->SetWeaponEquipped(bNewEquipped) : false;
}

bool UPlayerWeaponComponent::IsWeaponEquipped() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory && Inventory->IsWeaponEquipped();
}

EPlayerItemSlot UPlayerWeaponComponent::GetActiveWeaponSlot() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetActiveWeaponSlot() : EPlayerItemSlot::PrimaryWeapon;
}

AWeaponBase* UPlayerWeaponComponent::GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetWeaponInSlot(WeaponSlot) : nullptr;
}

AWeaponBase* UPlayerWeaponComponent::GetActiveWeapon() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetActiveWeapon() : nullptr;
}

EPlayerInventoryWeaponState UPlayerWeaponComponent::GetWeaponState(EPlayerItemSlot WeaponSlot) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetWeaponState(WeaponSlot) : EPlayerInventoryWeaponState::Idle;
}

EPlayerInventoryWeaponState UPlayerWeaponComponent::GetActiveWeaponState() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetActiveWeaponState() : EPlayerInventoryWeaponState::Idle;
}

void UPlayerWeaponComponent::StartActiveWeaponFire()
{
	if (UPlayerInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->StartActiveWeaponFire();
	}
}

void UPlayerWeaponComponent::StopActiveWeaponFire()
{
	if (UPlayerInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->StopActiveWeaponFire();
	}
}

bool UPlayerWeaponComponent::ReloadActiveWeapon()
{
	UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->ReloadActiveWeapon() : false;
}

bool UPlayerWeaponComponent::NotifyActiveWeaponFireAnimation()
{
	UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->NotifyActiveWeaponFireAnimation() : false;
}

bool UPlayerWeaponComponent::NotifyActiveWeaponReloadAnimationFinished()
{
	UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->NotifyActiveWeaponReloadAnimationFinished() : false;
}

int32 UPlayerWeaponComponent::GetInClip(EPlayerItemSlot WeaponSlot) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetInClip(WeaponSlot) : 0;
}

int32 UPlayerWeaponComponent::GetClipSize(EPlayerItemSlot WeaponSlot) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetClipSize(WeaponSlot) : 0;
}

int32 UPlayerWeaponComponent::GetActiveWeaponInClip() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetActiveWeaponInClip() : 0;
}

int32 UPlayerWeaponComponent::GetActiveWeaponClipSize() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory ? Inventory->GetActiveWeaponClipSize() : 0;
}

bool UPlayerWeaponComponent::WasActiveWeaponFireRequestedRecently(float WindowSeconds) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory && Inventory->WasActiveWeaponFireRequestedRecently(WindowSeconds);
}

bool UPlayerWeaponComponent::IsActiveWeaponFireNotifyPending() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory && Inventory->IsActiveWeaponFireNotifyPending();
}

bool UPlayerWeaponComponent::WasActiveWeaponFiredRecently(float WindowSeconds) const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory && Inventory->WasActiveWeaponFiredRecently(WindowSeconds);
}

bool UPlayerWeaponComponent::IsActiveWeaponReloading() const
{
	const UPlayerInventoryComponent* Inventory = GetInventoryComponent();
	return Inventory && Inventory->IsActiveWeaponReloading();
}

FOnActiveWeaponFireRequested& UPlayerWeaponComponent::OnActiveWeaponFireRequestedEvent()
{
	check(GetInventoryComponent());
	return GetInventoryComponent()->OnActiveWeaponFireRequestedEvent();
}

FOnActiveWeaponFired& UPlayerWeaponComponent::OnActiveWeaponFiredEvent()
{
	check(GetInventoryComponent());
	return GetInventoryComponent()->OnActiveWeaponFiredEvent();
}

UPlayerInventoryComponent* UPlayerWeaponComponent::GetInventoryComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UPlayerInventoryComponent>() : nullptr;
}
