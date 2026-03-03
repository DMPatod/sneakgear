#include "Components/PlayerItemComponent.h"

#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Items/WorldItemPickup.h"
#include "Weapon/WeaponBase.h"

UPlayerItemComponent::UPlayerItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	EquippedItem.SlotType = EPlayerItemSlot::Equipped;
	SupportItem.SlotType = EPlayerItemSlot::Support;
	PrimaryWeaponItem.SlotType = EPlayerItemSlot::PrimaryWeapon;
	SecondaryWeaponItem.SlotType = EPlayerItemSlot::SecondaryWeapon;
}

void UPlayerItemComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryWeaponRuntime.WeaponActor = SpawnWeapon(PrimaryWeaponClass);
	SecondaryWeaponRuntime.WeaponActor = SpawnWeapon(SecondaryWeaponClass);
	PrimaryWeaponRuntime.InClip = PrimaryWeaponRuntime.WeaponActor ? FMath::Max(PrimaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;
	SecondaryWeaponRuntime.InClip = SecondaryWeaponRuntime.WeaponActor ? FMath::Max(SecondaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;

	if (PrimaryWeaponRuntime.WeaponActor)
	{
		PrimaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerItemComponent::OnPrimaryWeaponFired);
	}

	if (SecondaryWeaponRuntime.WeaponActor)
	{
		SecondaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerItemComponent::OnSecondaryWeaponFired);
	}

	SyncWeaponAttachments();
}

void UPlayerItemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimaryWeaponRuntime.WeaponActor)
	{
		PrimaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().RemoveAll(this);
	}

	if (SecondaryWeaponRuntime.WeaponActor)
	{
		SecondaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool UPlayerItemComponent::AddItem(const FPlayerInventoryItem& Item, bool bAllowReplace)
{
	if (!Item.IsValid())
	{
		return false;
	}

	FPlayerInventoryItem* TargetSlot = ResolveMutableSlot(Item.SlotType);
	if (!TargetSlot)
	{
		return false;
	}

	if (TargetSlot->IsValid() && !bAllowReplace)
	{
		return false;
	}

	*TargetSlot = Item;
	OnItemSlotUpdated.Broadcast(Item.SlotType);
	return true;
}

bool UPlayerItemComponent::RemoveItem(EPlayerItemSlot Slot, FPlayerInventoryItem& OutItem)
{
	FPlayerInventoryItem* TargetSlot = ResolveMutableSlot(Slot);
	if (!TargetSlot || !TargetSlot->IsValid())
	{
		return false;
	}

	OutItem = *TargetSlot;
	*TargetSlot = FPlayerInventoryItem();
	TargetSlot->SlotType = Slot;
	OnItemSlotUpdated.Broadcast(Slot);
	return true;
}

bool UPlayerItemComponent::PickUpFromFloor(AWorldItemPickup* PickupActor, bool bAllowReplace)
{
	if (!PickupActor)
	{
		return false;
	}

	const FPlayerInventoryItem PickupItem = PickupActor->GetPickupItem();
	if (!AddItem(PickupItem, bAllowReplace))
	{
		return false;
	}

	PickupActor->ConsumePickup();
	return true;
}

bool UPlayerItemComponent::HasItem(EPlayerItemSlot Slot) const
{
	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? Item->IsValid() : false;
}

FPlayerInventoryItem UPlayerItemComponent::GetItem(EPlayerItemSlot Slot) const
{
	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? *Item : FPlayerInventoryItem();
}

bool UPlayerItemComponent::SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand)
{
	if (WeaponSlot != EPlayerItemSlot::PrimaryWeapon && WeaponSlot != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	if (!GetWeaponInSlot(WeaponSlot))
	{
		return false;
	}

	ActiveWeaponSlot = WeaponSlot;
	bWeaponEquipped = bEquipInHand;
	SyncWeaponAttachments();
	OnItemSlotUpdated.Broadcast(WeaponSlot);
	return true;
}

bool UPlayerItemComponent::SetWeaponEquipped(bool bNewEquipped)
{
	if (!GetActiveWeapon())
	{
		return false;
	}

	if (bWeaponEquipped == bNewEquipped)
	{
		return true;
	}

	bWeaponEquipped = bNewEquipped;
	SyncWeaponAttachments();
	OnItemSlotUpdated.Broadcast(ActiveWeaponSlot);
	return true;
}

AWeaponBase* UPlayerItemComponent::GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const
{
	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->WeaponActor.Get() : nullptr;
}

AWeaponBase* UPlayerItemComponent::GetActiveWeapon() const
{
	return GetWeaponInSlot(ActiveWeaponSlot);
}

void UPlayerItemComponent::StartActiveWeaponFire()
{
	if (!bWeaponEquipped)
	{
		return;
	}

	const FWeaponSlotRuntime* ActiveRuntime = ResolveWeaponRuntime(ActiveWeaponSlot);
	if (!ActiveRuntime || ActiveRuntime->InClip <= 0 || !ActiveRuntime->WeaponActor)
	{
		return;
	}

	ActiveRuntime->WeaponActor->StartFire();
}

void UPlayerItemComponent::StopActiveWeaponFire()
{
	AWeaponBase* Weapon = GetActiveWeapon();
	if (Weapon)
	{
		Weapon->StopFire();
	}
}

bool UPlayerItemComponent::ReloadActiveWeapon()
{
	FWeaponSlotRuntime* ActiveRuntime = ResolveWeaponRuntimeMutable(ActiveWeaponSlot);
	AThirdPersonPlayerCharacter* OwnerPlayer = Cast<AThirdPersonPlayerCharacter>(GetOwner());
	if (!ActiveRuntime || !ActiveRuntime->WeaponActor || !OwnerPlayer)
	{
		return false;
	}

	const int32 ClipSize = FMath::Max(ActiveRuntime->WeaponActor->ClipSize, 0);
	const int32 MissingAmmo = FMath::Max(ClipSize - ActiveRuntime->InClip, 0);
	if (MissingAmmo <= 0)
	{
		return false;
	}

	// Reserve ammo is consumed only while reloading.
	const int32 AvailableReserve = FMath::Max(FMath::FloorToInt(OwnerPlayer->GetAmmo()), 0);
	const int32 AmmoToLoad = FMath::Min(MissingAmmo, AvailableReserve);
	if (AmmoToLoad <= 0)
	{
		return false;
	}

	const float ConsumedReserve = OwnerPlayer->ConsumeAmmo(static_cast<float>(AmmoToLoad));
	const int32 LoadedAmmo = FMath::Clamp(FMath::FloorToInt(ConsumedReserve), 0, AmmoToLoad);
	ActiveRuntime->InClip = FMath::Clamp(ActiveRuntime->InClip + LoadedAmmo, 0, ClipSize);
	return LoadedAmmo > 0;
}

int32 UPlayerItemComponent::GetInClip(EPlayerItemSlot WeaponSlot) const
{
	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->InClip : -1;
}

int32 UPlayerItemComponent::GetClipSize(EPlayerItemSlot WeaponSlot) const
{
	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return (Runtime && Runtime->WeaponActor) ? FMath::Max(Runtime->WeaponActor->ClipSize, 0) : -1;
}

FPlayerInventoryItem* UPlayerItemComponent::ResolveMutableSlot(EPlayerItemSlot Slot)
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItem;
	case EPlayerItemSlot::Support:
		return &SupportItem;
	case EPlayerItemSlot::PrimaryWeapon:
		return &PrimaryWeaponItem;
	case EPlayerItemSlot::SecondaryWeapon:
		return &SecondaryWeaponItem;
	default:
		return nullptr;
	}
}

const FPlayerInventoryItem* UPlayerItemComponent::ResolveSlot(EPlayerItemSlot Slot) const
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItem;
	case EPlayerItemSlot::Support:
		return &SupportItem;
	case EPlayerItemSlot::PrimaryWeapon:
		return &PrimaryWeaponItem;
	case EPlayerItemSlot::SecondaryWeapon:
		return &SecondaryWeaponItem;
	default:
		return nullptr;
	}
}

UPlayerItemComponent::FWeaponSlotRuntime* UPlayerItemComponent::ResolveWeaponRuntimeMutable(EPlayerItemSlot Slot)
{
	switch (Slot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return &PrimaryWeaponRuntime;
	case EPlayerItemSlot::SecondaryWeapon:
		return &SecondaryWeaponRuntime;
	default:
		return nullptr;
	}
}

const UPlayerItemComponent::FWeaponSlotRuntime* UPlayerItemComponent::ResolveWeaponRuntime(EPlayerItemSlot Slot) const
{
	switch (Slot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return &PrimaryWeaponRuntime;
	case EPlayerItemSlot::SecondaryWeapon:
		return &SecondaryWeaponRuntime;
	default:
		return nullptr;
	}
}

AWeaponBase* UPlayerItemComponent::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass) const
{
	if (!WeaponClass)
	{
		return nullptr;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	UWorld* World = GetWorld();
	if (!OwnerCharacter || !World)
	{
		return nullptr;
	}

	AWeaponBase* SpawnedWeapon = World->SpawnActor<AWeaponBase>(WeaponClass);
	if (!SpawnedWeapon)
	{
		return nullptr;
	}

	SpawnedWeapon->SetOwner(OwnerCharacter);
	return SpawnedWeapon;
}

void UPlayerItemComponent::AttachWeapon(AWeaponBase* Weapon, FName SocketName) const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!Weapon || !OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	Weapon->AttachToCharacter(OwnerCharacter->GetMesh(), SocketName);
}

FName UPlayerItemComponent::GetHolsterSocketForSlot(EPlayerItemSlot WeaponSlot) const
{
	switch (WeaponSlot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return PrimaryWeaponHolsterSocketName;
	case EPlayerItemSlot::SecondaryWeapon:
		return SecondaryWeaponHolsterSocketName;
	default:
		return NAME_None;
	}
}

void UPlayerItemComponent::SyncWeaponAttachments() const
{
	const FName PrimaryHolsterSocket = GetHolsterSocketForSlot(EPlayerItemSlot::PrimaryWeapon);
	const FName SecondaryHolsterSocket = GetHolsterSocketForSlot(EPlayerItemSlot::SecondaryWeapon);

	if (!bWeaponEquipped)
	{
		AttachWeapon(PrimaryWeaponRuntime.WeaponActor, PrimaryHolsterSocket);
		AttachWeapon(SecondaryWeaponRuntime.WeaponActor, SecondaryHolsterSocket);
		return;
	}

	const bool bPrimaryActive = ActiveWeaponSlot == EPlayerItemSlot::PrimaryWeapon;
	AttachWeapon(PrimaryWeaponRuntime.WeaponActor, bPrimaryActive ? WeaponHandSocketName : PrimaryHolsterSocket);
	AttachWeapon(SecondaryWeaponRuntime.WeaponActor, bPrimaryActive ? SecondaryHolsterSocket : WeaponHandSocketName);
}

void UPlayerItemComponent::HandleWeaponFired(EPlayerItemSlot Slot)
{
	FWeaponSlotRuntime* Runtime = ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime)
	{
		return;
	}

	Runtime->InClip = FMath::Max(Runtime->InClip - 1, 0);
	if (Runtime->InClip <= 0 && Runtime->WeaponActor)
	{
		Runtime->WeaponActor->StopFire();
	}

	if (ActiveWeaponSlot == Slot)
	{
		// HUD/crosshair feedback should only react to the currently equipped slot.
		OnActiveWeaponFired.Broadcast(Slot);
	}
}

void UPlayerItemComponent::OnPrimaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerItemComponent::OnSecondaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::SecondaryWeapon);
}
