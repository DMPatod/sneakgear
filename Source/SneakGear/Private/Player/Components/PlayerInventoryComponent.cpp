#include "Player/Components/PlayerInventoryComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "Player/PlayerCharacterBase.h"
#include "GameFramework/Character.h"
#include "Items/WorldItemPickup.h"
#include "Misc/DataValidation.h"
#include "Weapon/WeaponBase.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	EquippedItem.SlotType = EPlayerItemSlot::Equipped;
	SupportItem.SlotType = EPlayerItemSlot::Support;
	PrimaryWeaponItem.SlotType = EPlayerItemSlot::PrimaryWeapon;
	SecondaryWeaponItem.SlotType = EPlayerItemSlot::SecondaryWeapon;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryWeaponRuntime.WeaponActor = SpawnWeapon(PrimaryWeaponClass);
	SecondaryWeaponRuntime.WeaponActor = SpawnWeapon(SecondaryWeaponClass);
	PrimaryWeaponRuntime.InClip = PrimaryWeaponRuntime.WeaponActor ? FMath::Max(PrimaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;
	SecondaryWeaponRuntime.InClip = SecondaryWeaponRuntime.WeaponActor ? FMath::Max(SecondaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;

	if (PrimaryWeaponRuntime.WeaponActor)
	{
		PrimaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerInventoryComponent::OnPrimaryWeaponFired);
	}

	if (SecondaryWeaponRuntime.WeaponActor)
	{
		SecondaryWeaponRuntime.WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerInventoryComponent::OnSecondaryWeaponFired);
	}

	if (PrimaryWeaponItem.IsValid() && !PrimaryWeaponRuntime.WeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' has a primary weapon item but failed to spawn PrimaryWeaponClass '%s'."),
			*GetName(), *GetNameSafe(PrimaryWeaponClass));
	}

	if (SecondaryWeaponItem.IsValid() && !SecondaryWeaponRuntime.WeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' has a secondary weapon item but failed to spawn SecondaryWeaponClass '%s'."),
			*GetName(), *GetNameSafe(SecondaryWeaponClass));
	}

	SyncWeaponAttachments();
}

void UPlayerInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

EDataValidationResult UPlayerInventoryComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (PrimaryWeaponItem.IsValid() && !PrimaryWeaponClass)
	{
		Context.AddError(FText::FromString(TEXT("PrimaryWeaponItem is set but PrimaryWeaponClass is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (SecondaryWeaponItem.IsValid() && !SecondaryWeaponClass)
	{
		Context.AddError(FText::FromString(TEXT("SecondaryWeaponItem is set but SecondaryWeaponClass is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if ((PrimaryWeaponClass || SecondaryWeaponClass) &&
		(WeaponHandSocketName.IsNone() || PrimaryWeaponHolsterSocketName.IsNone() || SecondaryWeaponHolsterSocketName.IsNone()))
	{
		Context.AddError(FText::FromString(TEXT("Weapon socket names must be configured when weapon classes are assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (HasValidWeaponItem(ActiveWeaponSlot) == false && (PrimaryWeaponItem.IsValid() || SecondaryWeaponItem.IsValid()))
	{
		Context.AddWarning(FText::FromString(TEXT("ActiveWeaponSlot does not point to a valid configured weapon item.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

bool UPlayerInventoryComponent::AddItem(const FPlayerInventoryItem& Item, bool bAllowReplace)
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
	OnInventoryStateChanged.Broadcast();
	return true;
}

bool UPlayerInventoryComponent::RemoveItem(EPlayerItemSlot Slot, FPlayerInventoryItem& OutItem)
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
	OnInventoryStateChanged.Broadcast();
	return true;
}

bool UPlayerInventoryComponent::PickUpFromFloor(AWorldItemPickup* PickupActor, bool bAllowReplace)
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

bool UPlayerInventoryComponent::TryPickUpNearbyFloorItem(float SearchRadius, bool bAllowReplace)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FloorItemPickup), false, OwnerActor);
	const FVector Center = OwnerActor->GetActorLocation();
	const FCollisionShape Shape = FCollisionShape::MakeSphere(FMath::Max(SearchRadius, 1.f));

	const bool bHasOverlaps = World->OverlapMultiByObjectType(
		Overlaps,
		Center,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		Shape,
		QueryParams
	);

	if (!bHasOverlaps)
	{
		return false;
	}

	AWorldItemPickup* BestPickup = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : Overlaps)
	{
		AWorldItemPickup* Pickup = Cast<AWorldItemPickup>(Result.GetActor());
		if (!Pickup)
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Center, Pickup->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestPickup = Pickup;
		}
	}

	return BestPickup ? PickUpFromFloor(BestPickup, bAllowReplace) : false;
}

bool UPlayerInventoryComponent::HasItem(EPlayerItemSlot Slot) const
{
	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? Item->IsValid() : false;
}

FPlayerInventoryItem UPlayerInventoryComponent::GetItem(EPlayerItemSlot Slot) const
{
	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? *Item : FPlayerInventoryItem();
}

bool UPlayerInventoryComponent::SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand)
{
	if (WeaponSlot != EPlayerItemSlot::PrimaryWeapon && WeaponSlot != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	if (!HasValidWeaponItem(WeaponSlot) || !GetWeaponInSlot(WeaponSlot))
	{
		return false;
	}

	ActiveWeaponSlot = WeaponSlot;
	bWeaponEquipped = bEquipInHand;
	SyncWeaponAttachments();
	OnItemSlotUpdated.Broadcast(WeaponSlot);
	OnInventoryStateChanged.Broadcast();
	return true;
}

bool UPlayerInventoryComponent::SetWeaponEquipped(bool bNewEquipped)
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
	OnInventoryStateChanged.Broadcast();
	return true;
}

AWeaponBase* UPlayerInventoryComponent::GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const
{
	if (!HasValidWeaponItem(WeaponSlot))
	{
		return nullptr;
	}

	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->WeaponActor.Get() : nullptr;
}

AWeaponBase* UPlayerInventoryComponent::GetActiveWeapon() const
{
	return GetWeaponInSlot(ActiveWeaponSlot);
}

void UPlayerInventoryComponent::StartActiveWeaponFire()
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

void UPlayerInventoryComponent::StopActiveWeaponFire()
{
	AWeaponBase* Weapon = GetActiveWeapon();
	if (Weapon)
	{
		Weapon->StopFire();
	}
}

bool UPlayerInventoryComponent::ReloadActiveWeapon()
{
	FWeaponSlotRuntime* ActiveRuntime = ResolveWeaponRuntimeMutable(ActiveWeaponSlot);
	APlayerCharacterBase* OwnerPlayer = Cast<APlayerCharacterBase>(GetOwner());
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
	if (LoadedAmmo > 0)
	{
		OnInventoryStateChanged.Broadcast();
	}
	return LoadedAmmo > 0;
}

int32 UPlayerInventoryComponent::GetInClip(EPlayerItemSlot WeaponSlot) const
{
	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return Runtime ? Runtime->InClip : -1;
}

int32 UPlayerInventoryComponent::GetClipSize(EPlayerItemSlot WeaponSlot) const
{
	const FWeaponSlotRuntime* Runtime = ResolveWeaponRuntime(WeaponSlot);
	return (Runtime && Runtime->WeaponActor) ? FMath::Max(Runtime->WeaponActor->ClipSize, 0) : -1;
}

bool UPlayerInventoryComponent::HasValidWeaponItem(EPlayerItemSlot Slot) const
{
	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item && Item->IsValid() &&
		(Slot == EPlayerItemSlot::PrimaryWeapon || Slot == EPlayerItemSlot::SecondaryWeapon);
}

int32 UPlayerInventoryComponent::GetActiveWeaponInClip() const
{
	return FMath::Max(GetInClip(ActiveWeaponSlot), 0);
}

int32 UPlayerInventoryComponent::GetActiveWeaponClipSize() const
{
	return FMath::Max(GetClipSize(ActiveWeaponSlot), 0);
}

int32 UPlayerInventoryComponent::GetReserveAmmoCount() const
{
	const APlayerCharacterBase* OwnerPlayer = Cast<APlayerCharacterBase>(GetOwner());
	return OwnerPlayer ? FMath::Max(FMath::FloorToInt(OwnerPlayer->GetAmmo()), 0) : 0;
}

bool UPlayerInventoryComponent::WasActiveWeaponFiredRecently(float WindowSeconds) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float SafeWindow = FMath::Max(WindowSeconds, 0.01f);
	return (World->GetTimeSeconds() - LastActiveWeaponFireTimestamp) <= SafeWindow;
}

FPlayerInventoryItem* UPlayerInventoryComponent::ResolveMutableSlot(EPlayerItemSlot Slot)
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

const FPlayerInventoryItem* UPlayerInventoryComponent::ResolveSlot(EPlayerItemSlot Slot) const
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

UPlayerInventoryComponent::FWeaponSlotRuntime* UPlayerInventoryComponent::ResolveWeaponRuntimeMutable(EPlayerItemSlot Slot)
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

const UPlayerInventoryComponent::FWeaponSlotRuntime* UPlayerInventoryComponent::ResolveWeaponRuntime(EPlayerItemSlot Slot) const
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

AWeaponBase* UPlayerInventoryComponent::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass) const
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
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' failed to spawn weapon class '%s'."),
			*GetName(), *GetNameSafe(WeaponClass));
		return nullptr;
	}

	SpawnedWeapon->SetOwner(OwnerCharacter);
	return SpawnedWeapon;
}

void UPlayerInventoryComponent::AttachWeapon(AWeaponBase* Weapon, FName SocketName, bool bUseHolsterOffset) const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!Weapon || !OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	Weapon->AttachToCharacter(OwnerCharacter->GetMesh(), SocketName, bUseHolsterOffset);
}

FName UPlayerInventoryComponent::GetHolsterSocketForSlot(EPlayerItemSlot WeaponSlot) const
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

void UPlayerInventoryComponent::SyncWeaponAttachments() const
{
	const FName PrimaryHolsterSocket = GetHolsterSocketForSlot(EPlayerItemSlot::PrimaryWeapon);
	const FName SecondaryHolsterSocket = GetHolsterSocketForSlot(EPlayerItemSlot::SecondaryWeapon);
	const bool bHasPrimaryItem = HasValidWeaponItem(EPlayerItemSlot::PrimaryWeapon);
	const bool bHasSecondaryItem = HasValidWeaponItem(EPlayerItemSlot::SecondaryWeapon);

	auto SyncWeaponPresentation = [](AWeaponBase* Weapon, bool bShouldShow)
	{
		if (!Weapon)
		{
			return;
		}

		Weapon->SetActorHiddenInGame(!bShouldShow);
		Weapon->SetActorEnableCollision(bShouldShow);
		if (!bShouldShow)
		{
			Weapon->StopFire();
		}
	};

	if (!bWeaponEquipped)
	{
		SyncWeaponPresentation(PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
		SyncWeaponPresentation(SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
		if (bHasPrimaryItem)
		{
			AttachWeapon(PrimaryWeaponRuntime.WeaponActor, PrimaryHolsterSocket, true);
		}
		if (bHasSecondaryItem)
		{
			AttachWeapon(SecondaryWeaponRuntime.WeaponActor, SecondaryHolsterSocket, true);
		}
		return;
	}

	const bool bPrimaryActive = ActiveWeaponSlot == EPlayerItemSlot::PrimaryWeapon;
	SyncWeaponPresentation(PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
	SyncWeaponPresentation(SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
	if (bHasPrimaryItem)
	{
		AttachWeapon(PrimaryWeaponRuntime.WeaponActor, bPrimaryActive ? WeaponHandSocketName : PrimaryHolsterSocket, !bPrimaryActive);
	}
	if (bHasSecondaryItem)
	{
		AttachWeapon(SecondaryWeaponRuntime.WeaponActor, bPrimaryActive ? SecondaryWeaponHolsterSocketName : WeaponHandSocketName, bPrimaryActive);
	}
}

void UPlayerInventoryComponent::HandleWeaponFired(EPlayerItemSlot Slot)
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
		LastActiveWeaponFireTimestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : LastActiveWeaponFireTimestamp;
		// HUD/crosshair feedback should only react to the currently equipped slot.
		OnActiveWeaponFired.Broadcast(Slot);
	}

	OnInventoryStateChanged.Broadcast();
}

void UPlayerInventoryComponent::OnPrimaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryComponent::OnSecondaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::SecondaryWeapon);
}
