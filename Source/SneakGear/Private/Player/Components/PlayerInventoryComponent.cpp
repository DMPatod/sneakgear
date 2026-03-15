#include "Player/Components/PlayerInventoryComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Items/PlayerItemDefinition.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Items/WorldItemPickup.h"
#include "Misc/DataValidation.h"
#include "Player/PlayerCharacterBase.h"
#include "UI/EventLogSubsystem.h"
#include "Weapon/UnarmedWeapon.h"
#include "Weapon/WeaponBase.h"

namespace
{
FText GetInventoryItemLabel(const FPlayerInventoryItem& Item)
{
	return !Item.DisplayName.IsEmpty() ? Item.DisplayName : FText::FromName(Item.ItemId);
}

FText GetInventorySlotLabel(EPlayerItemSlot Slot)
{
	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	return SlotEnum ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(Slot)) : FText::GetEmpty();
}
}

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UnarmedWeaponClass = AUnarmedWeapon::StaticClass();
	PrimaryWeaponItem.SlotType = EPlayerItemSlot::PrimaryWeapon;
	SecondaryWeaponItem.SlotType = EPlayerItemSlot::SecondaryWeapon;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryWeaponRuntime.WeaponActor = SpawnWeapon(PrimaryWeaponClass);
	SecondaryWeaponRuntime.WeaponActor = SpawnWeapon(SecondaryWeaponClass);
	UnarmedWeapon = SpawnWeapon(UnarmedWeaponClass);
	PrimaryWeaponRuntime.InClip = PrimaryWeaponRuntime.WeaponActor ? FMath::Max(PrimaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;
	SecondaryWeaponRuntime.InClip = SecondaryWeaponRuntime.WeaponActor ? FMath::Max(SecondaryWeaponRuntime.WeaponActor->ClipSize, 0) : -1;

	if (PrimaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(EPlayerItemSlot::PrimaryWeapon, PrimaryWeaponRuntime.WeaponActor);
	}

	if (SecondaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(EPlayerItemSlot::SecondaryWeapon, SecondaryWeaponRuntime.WeaponActor);
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

	if (!UnarmedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' failed to spawn UnarmedWeaponClass '%s'."),
			*GetName(), *GetNameSafe(UnarmedWeaponClass));
	}

	SyncWeaponAttachments();
}

void UPlayerInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateCurrentEquippedItem();

	if (PrimaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(PrimaryWeaponRuntime);
	}

	if (SecondaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(SecondaryWeaponRuntime);
	}

	if (UnarmedWeapon)
	{
		UnarmedWeapon->Destroy();
		UnarmedWeapon = nullptr;
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

	if ((PrimaryWeaponClass || SecondaryWeaponClass || UnarmedWeaponClass) &&
		(WeaponHandSocketName.IsNone() || PrimaryWeaponHolsterSocketName.IsNone() || SecondaryWeaponHolsterSocketName.IsNone()))
	{
		Context.AddError(FText::FromString(TEXT("Weapon socket names must be configured when weapon classes are assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!UnarmedWeaponClass)
	{
		Context.AddWarning(FText::FromString(TEXT("UnarmedWeaponClass is not configured. Fallback-to-unarmed behavior will be unavailable.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!HasValidWeaponSelection(ActiveWeaponSlot) && (PrimaryWeaponItem.IsValid() || SecondaryWeaponItem.IsValid()))
	{
		Context.AddWarning(FText::FromString(TEXT("ActiveWeaponSlot does not point to a valid configured weapon item.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (ResolveActiveItemIndex(EPlayerItemSlot::Equipped) >= EquippedItems.Num())
	{
		Context.AddError(FText::FromString(TEXT("Active equipped item index is out of range.")));
		Result = EDataValidationResult::Invalid;
	}

	if (ResolveActiveItemIndex(EPlayerItemSlot::Support) >= SupportItems.Num())
	{
		Context.AddError(FText::FromString(TEXT("Active support item index is out of range.")));
		Result = EDataValidationResult::Invalid;
	}

	if (ResolveActiveItemIndex(EPlayerItemSlot::Utility) >= UtilityItems.Num())
	{
		Context.AddError(FText::FromString(TEXT("Active utility item index is out of range.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool UPlayerInventoryComponent::AddItem(const FPlayerInventoryItem& Item, bool bAllowReplace)
{
	return AddItemInternal(Item, nullptr, bAllowReplace);
}

bool UPlayerInventoryComponent::AddItemInternal(const FPlayerInventoryItem& Item, UPlayerItemDefinition* ItemDefinition,
                                                bool bAllowReplace)
{
	if (!Item.IsValid())
	{
		return false;
	}

	if (TArray<FPlayerInventoryItem>* Collection = ResolveMutableCollection(Item.SlotType))
	{
		TArray<TObjectPtr<UPlayerItemDefinition>>* DefinitionCollection = ResolveMutableDefinitionCollection(Item.SlotType);
		if (!DefinitionCollection)
		{
			return false;
		}

		int32& ActiveIndex = ResolveActiveItemIndex(Item.SlotType);
		const int32 PreviousActiveIndex = ActiveIndex;
		UPlayerItemDefinition* PreviousActiveDefinition = Item.SlotType == EPlayerItemSlot::Equipped ? GetItemDefinition(Item.SlotType) : nullptr;
		bool bActiveItemChanged = false;
		if (bAllowReplace && Collection->IsValidIndex(ActiveIndex))
		{
			(*Collection)[ActiveIndex] = Item;
			if (DefinitionCollection->IsValidIndex(ActiveIndex))
			{
				(*DefinitionCollection)[ActiveIndex] = ItemDefinition;
			}
			bActiveItemChanged = true;
		}
		else
		{
			Collection->Add(Item);
			DefinitionCollection->Add(ItemDefinition);
			if (ActiveIndex == INDEX_NONE)
			{
				ActiveIndex = 0;
				bActiveItemChanged = true;
			}
		}

		if (Item.SlotType == EPlayerItemSlot::Equipped)
		{
			if (PreviousActiveDefinition && (bActiveItemChanged || PreviousActiveIndex != ActiveIndex) &&
				PreviousActiveDefinition != GetItemDefinition(Item.SlotType))
			{
				PreviousActiveDefinition->DeactivateItem(GetOwnerPlayerCharacter(), this);
			}
			if (bActiveItemChanged || PreviousActiveIndex != ActiveIndex)
			{
				ActivateCurrentEquippedItem();
			}
		}

		OnItemSlotUpdated.Broadcast(Item.SlotType);
		OnInventoryStateChanged.Broadcast();
		return true;
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
	if (const TArray<FPlayerInventoryItem>* Collection = ResolveCollection(Slot))
	{
		const int32 ActiveIndex = ResolveActiveItemIndex(Slot);
		return Collection->IsValidIndex(ActiveIndex) ? RemoveItemAt(Slot, ActiveIndex, OutItem) : false;
	}

	FPlayerInventoryItem* TargetSlot = ResolveMutableSlot(Slot);
	if (!TargetSlot || !TargetSlot->IsValid())
	{
		return false;
	}

	OutItem = *TargetSlot;
	*TargetSlot = FPlayerInventoryItem();
	TargetSlot->SlotType = Slot;
	if (Slot == EPlayerItemSlot::PrimaryWeapon || Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		SetItemDefinitionForSlot(Slot, nullptr);
		SetWeaponClassForSlot(Slot, nullptr);
	}
	OnItemSlotUpdated.Broadcast(Slot);
	OnInventoryStateChanged.Broadcast();
	return true;
}

bool UPlayerInventoryComponent::RemoveItemAt(EPlayerItemSlot Slot, int32 Index, FPlayerInventoryItem& OutItem)
{
	TArray<FPlayerInventoryItem>* Collection = ResolveMutableCollection(Slot);
	TArray<TObjectPtr<UPlayerItemDefinition>>* DefinitionCollection = ResolveMutableDefinitionCollection(Slot);
	if (!Collection || !DefinitionCollection || !Collection->IsValidIndex(Index) || !DefinitionCollection->IsValidIndex(Index))
	{
		return false;
	}

	UPlayerItemDefinition* RemovedDefinition = (*DefinitionCollection)[Index];
	const int32 PreviousActiveIndex = ResolveActiveItemIndex(Slot);
	const bool bRemovedActiveEquippedItem = Slot == EPlayerItemSlot::Equipped && Index == PreviousActiveIndex;
	if (bRemovedActiveEquippedItem && RemovedDefinition)
	{
		RemovedDefinition->DeactivateItem(GetOwnerPlayerCharacter(), this);
	}

	OutItem = (*Collection)[Index];
	Collection->RemoveAt(Index);
	DefinitionCollection->RemoveAt(Index);
	NormalizeActiveItemIndex(Slot);
	if (bRemovedActiveEquippedItem)
	{
		ActivateCurrentEquippedItem();
	}
	OnItemSlotUpdated.Broadcast(Slot);
	OnInventoryStateChanged.Broadcast();
	return true;
}

bool UPlayerInventoryComponent::PickUpFromFloor(AActor* PickupActor, bool bAllowReplace)
{
	if (!PickupActor)
	{
		return false;
	}

	UPlayerItemPickupComponent* PickupComponent = PickupActor->FindComponentByClass<UPlayerItemPickupComponent>();
	if (!PickupComponent)
	{
		return false;
	}

	const UPlayerItemDefinition* ItemDefinition = PickupComponent->GetItemDefinition();
	const FPlayerInventoryItem PickupItem = PickupComponent->GetPickupItem();
	if (!ItemDefinition || !PickupItem.IsValid())
	{
		return false;
	}

	if (!AddItemInternal(PickupItem, PickupComponent->GetItemDefinition(), bAllowReplace))
	{
		return false;
	}

	if (PickupItem.SlotType == EPlayerItemSlot::PrimaryWeapon || PickupItem.SlotType == EPlayerItemSlot::SecondaryWeapon)
	{
		SetItemDefinitionForSlot(PickupItem.SlotType, PickupComponent->GetItemDefinition());
		if (!SetWeaponClassForSlot(PickupItem.SlotType, PickupComponent->GetPickupWeaponClass()))
		{
			return false;
		}
	}

	if (UWorld* World = GetWorld())
	{
		if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
		{
			EventLog->ReportItemPickedUp(GetOwner(), GetInventoryItemLabel(PickupItem));
		}
	}

	PickupComponent->ConsumePickup();
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

	AActor* BestPickup = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* PickupActor = Result.GetActor();
		if (!PickupActor)
		{
			continue;
		}

		UPlayerItemPickupComponent* PickupComponent = PickupActor->FindComponentByClass<UPlayerItemPickupComponent>();
		if (!PickupComponent || !PickupComponent->GetItemDefinition())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Center, PickupActor->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestPickup = PickupActor;
		}
	}

	return BestPickup ? PickUpFromFloor(BestPickup, bAllowReplace) : false;
}

bool UPlayerInventoryComponent::HasItem(EPlayerItemSlot Slot) const
{
	if (const TArray<FPlayerInventoryItem>* Collection = ResolveCollection(Slot))
	{
		return !Collection->IsEmpty();
	}

	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? Item->IsValid() : false;
}

FPlayerInventoryItem UPlayerInventoryComponent::GetItem(EPlayerItemSlot Slot) const
{
	if (const TArray<FPlayerInventoryItem>* Collection = ResolveCollection(Slot))
	{
		const int32 ActiveIndex = ResolveActiveItemIndex(Slot);
		return Collection->IsValidIndex(ActiveIndex) ? (*Collection)[ActiveIndex] : FPlayerInventoryItem();
	}

	const FPlayerInventoryItem* Item = ResolveSlot(Slot);
	return Item ? *Item : FPlayerInventoryItem();
}

int32 UPlayerInventoryComponent::GetItemCount(EPlayerItemSlot Slot) const
{
	if (const TArray<FPlayerInventoryItem>* Collection = ResolveCollection(Slot))
	{
		return Collection->Num();
	}

	return HasItem(Slot) ? 1 : 0;
}

FPlayerInventoryItem UPlayerInventoryComponent::GetItemAt(EPlayerItemSlot Slot, int32 Index) const
{
	if (const TArray<FPlayerInventoryItem>* Collection = ResolveCollection(Slot))
	{
		return Collection->IsValidIndex(Index) ? (*Collection)[Index] : FPlayerInventoryItem();
	}

	return Index == 0 ? GetItem(Slot) : FPlayerInventoryItem();
}

bool UPlayerInventoryComponent::SetActiveItemIndex(EPlayerItemSlot Slot, int32 Index)
{
	TArray<FPlayerInventoryItem>* Collection = ResolveMutableCollection(Slot);
	if (!Collection || !Collection->IsValidIndex(Index))
	{
		return false;
	}

	UPlayerItemDefinition* PreviousActiveDefinition = Slot == EPlayerItemSlot::Equipped ? GetItemDefinition(Slot) : nullptr;
	int32& ActiveIndex = ResolveActiveItemIndex(Slot);
	if (ActiveIndex == Index)
	{
		return true;
	}

	ActiveIndex = Index;
	if (Slot == EPlayerItemSlot::Equipped)
	{
		if (PreviousActiveDefinition)
		{
			PreviousActiveDefinition->DeactivateItem(GetOwnerPlayerCharacter(), this);
		}
		ActivateCurrentEquippedItem();
	}
	OnItemSlotUpdated.Broadcast(Slot);
	OnInventoryStateChanged.Broadcast();
	return true;
}

int32 UPlayerInventoryComponent::GetActiveItemIndex(EPlayerItemSlot Slot) const
{
	return ResolveActiveItemIndex(Slot);
}

bool UPlayerInventoryComponent::UseActiveSupportItem()
{
	const int32 ActiveIndex = GetActiveItemIndex(EPlayerItemSlot::Support);
	UPlayerItemDefinition* ActiveDefinition = GetItemDefinitionAt(EPlayerItemSlot::Support, ActiveIndex);
	APlayerCharacterBase* PlayerCharacter = GetOwnerPlayerCharacter();
	const FPlayerInventoryItem ActiveItem = GetItemAt(EPlayerItemSlot::Support, ActiveIndex);
	if (!ActiveDefinition || !PlayerCharacter)
	{
		return false;
	}

	if (!ActiveDefinition->UseItem(PlayerCharacter, this))
	{
		return false;
	}

	if (ActiveDefinition->ShouldConsumeOnUse())
	{
		FPlayerInventoryItem ConsumedItem;
		RemoveItemAt(EPlayerItemSlot::Support, ActiveIndex, ConsumedItem);
	}
	else
	{
		OnItemSlotUpdated.Broadcast(EPlayerItemSlot::Support);
		OnInventoryStateChanged.Broadcast();
	}

	if (UWorld* World = GetWorld())
	{
		if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
		{
			EventLog->ReportItemUsed(GetOwner(), GetInventoryItemLabel(ActiveItem), GetInventorySlotLabel(EPlayerItemSlot::Support));
		}
	}

	return true;
}

bool UPlayerInventoryComponent::UseActiveUtilityItem()
{
	const int32 ActiveIndex = GetActiveItemIndex(EPlayerItemSlot::Utility);
	UPlayerItemDefinition* ActiveDefinition = GetItemDefinitionAt(EPlayerItemSlot::Utility, ActiveIndex);
	APlayerCharacterBase* PlayerCharacter = GetOwnerPlayerCharacter();
	const FPlayerInventoryItem ActiveItem = GetItemAt(EPlayerItemSlot::Utility, ActiveIndex);
	if (!ActiveDefinition || !PlayerCharacter)
	{
		return false;
	}

	if (!ActiveDefinition->UseItem(PlayerCharacter, this))
	{
		return false;
	}

	if (ActiveDefinition->ShouldConsumeOnUse())
	{
		FPlayerInventoryItem ConsumedItem;
		RemoveItemAt(EPlayerItemSlot::Utility, ActiveIndex, ConsumedItem);
	}
	else
	{
		OnItemSlotUpdated.Broadcast(EPlayerItemSlot::Utility);
		OnInventoryStateChanged.Broadcast();
	}

	if (UWorld* World = GetWorld())
	{
		if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
		{
			EventLog->ReportItemUsed(GetOwner(), GetInventoryItemLabel(ActiveItem), GetInventorySlotLabel(EPlayerItemSlot::Utility));
		}
	}

	return true;
}

bool UPlayerInventoryComponent::SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand)
{
	if (WeaponSlot != EPlayerItemSlot::PrimaryWeapon && WeaponSlot != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	if (!HasValidWeaponSelection(WeaponSlot))
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
	if (AWeaponBase* ActiveWeapon = GetWeaponInSlot(ActiveWeaponSlot))
	{
		return ActiveWeapon;
	}

	return UnarmedWeapon;
}

void UPlayerInventoryComponent::StartActiveWeaponFire()
{
	if (!bWeaponEquipped)
	{
		return;
	}

	if (!HasValidWeaponSelection(ActiveWeaponSlot))
	{
		if (UnarmedWeapon)
		{
			UnarmedWeapon->StartFire();
		}
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
	if (!HasValidWeaponSelection(ActiveWeaponSlot))
	{
		return false;
	}

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

bool UPlayerInventoryComponent::HasValidWeaponSelection(EPlayerItemSlot Slot) const
{
	return HasValidWeaponItem(Slot) && GetWeaponInSlot(Slot) != nullptr;
}

bool UPlayerInventoryComponent::SetWeaponClassForSlot(EPlayerItemSlot Slot, TSubclassOf<AWeaponBase> WeaponClass)
{
	FWeaponSlotRuntime* Runtime = ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime)
	{
		return false;
	}

	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		PrimaryWeaponClass = WeaponClass;
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		SecondaryWeaponClass = WeaponClass;
	}

	ClearRuntimeWeapon(*Runtime);
	Runtime->WeaponActor = SpawnWeapon(WeaponClass);
	Runtime->InClip = Runtime->WeaponActor ? FMath::Max(Runtime->WeaponActor->ClipSize, 0) : -1;

	if (Runtime->WeaponActor)
	{
		BindRuntimeWeaponDelegates(Slot, Runtime->WeaponActor);
	}

	SyncWeaponAttachments();
	return Runtime->WeaponActor != nullptr || !WeaponClass;
}

int32 UPlayerInventoryComponent::GetActiveWeaponInClip() const
{
	if (!HasValidWeaponSelection(ActiveWeaponSlot))
	{
		return 0;
	}

	return FMath::Max(GetInClip(ActiveWeaponSlot), 0);
}

int32 UPlayerInventoryComponent::GetActiveWeaponClipSize() const
{
	if (!HasValidWeaponSelection(ActiveWeaponSlot))
	{
		return 0;
	}

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

TArray<FPlayerInventoryItem>* UPlayerInventoryComponent::ResolveMutableCollection(EPlayerItemSlot Slot)
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItems;
	case EPlayerItemSlot::Support:
		return &SupportItems;
	case EPlayerItemSlot::Utility:
		return &UtilityItems;
	default:
		return nullptr;
	}
}

const TArray<FPlayerInventoryItem>* UPlayerInventoryComponent::ResolveCollection(EPlayerItemSlot Slot) const
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItems;
	case EPlayerItemSlot::Support:
		return &SupportItems;
	case EPlayerItemSlot::Utility:
		return &UtilityItems;
	default:
		return nullptr;
	}
}

TArray<TObjectPtr<UPlayerItemDefinition>>* UPlayerInventoryComponent::ResolveMutableDefinitionCollection(EPlayerItemSlot Slot)
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItemDefinitions;
	case EPlayerItemSlot::Support:
		return &SupportItemDefinitions;
	case EPlayerItemSlot::Utility:
		return &UtilityItemDefinitions;
	default:
		return nullptr;
	}
}

const TArray<TObjectPtr<UPlayerItemDefinition>>* UPlayerInventoryComponent::ResolveDefinitionCollection(EPlayerItemSlot Slot) const
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return &EquippedItemDefinitions;
	case EPlayerItemSlot::Support:
		return &SupportItemDefinitions;
	case EPlayerItemSlot::Utility:
		return &UtilityItemDefinitions;
	default:
		return nullptr;
	}
}

UPlayerItemDefinition* UPlayerInventoryComponent::GetItemDefinition(EPlayerItemSlot Slot) const
{
	if (const TArray<TObjectPtr<UPlayerItemDefinition>>* DefinitionCollection = ResolveDefinitionCollection(Slot))
	{
		const int32 ActiveIndex = ResolveActiveItemIndex(Slot);
		return DefinitionCollection->IsValidIndex(ActiveIndex) ? (*DefinitionCollection)[ActiveIndex] : nullptr;
	}

	switch (Slot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return PrimaryWeaponItemDefinition;
	case EPlayerItemSlot::SecondaryWeapon:
		return SecondaryWeaponItemDefinition;
	default:
		return nullptr;
	}
}

UPlayerItemDefinition* UPlayerInventoryComponent::GetItemDefinitionAt(EPlayerItemSlot Slot, int32 Index) const
{
	if (const TArray<TObjectPtr<UPlayerItemDefinition>>* DefinitionCollection = ResolveDefinitionCollection(Slot))
	{
		return DefinitionCollection->IsValidIndex(Index) ? (*DefinitionCollection)[Index] : nullptr;
	}

	return Index == 0 ? GetItemDefinition(Slot) : nullptr;
}

void UPlayerInventoryComponent::SetItemDefinitionForSlot(EPlayerItemSlot Slot, UPlayerItemDefinition* ItemDefinition)
{
	switch (Slot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		PrimaryWeaponItemDefinition = ItemDefinition;
		break;
	case EPlayerItemSlot::SecondaryWeapon:
		SecondaryWeaponItemDefinition = ItemDefinition;
		break;
	default:
		break;
	}
}

void UPlayerInventoryComponent::SetActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition,
                                                             FActiveGameplayEffectHandle EffectHandle)
{
	if (!ItemDefinition)
	{
		return;
	}

	if (EffectHandle.IsValid())
	{
		ActiveItemEffectHandles.Add(ItemDefinition, EffectHandle);
	}
	else
	{
		ActiveItemEffectHandles.Remove(ItemDefinition);
	}
}

FActiveGameplayEffectHandle UPlayerInventoryComponent::GetActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition) const
{
	const FActiveGameplayEffectHandle* FoundHandle = ActiveItemEffectHandles.Find(ItemDefinition);
	return FoundHandle ? *FoundHandle : FActiveGameplayEffectHandle();
}

void UPlayerInventoryComponent::ClearActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition)
{
	if (!ItemDefinition)
	{
		return;
	}

	ActiveItemEffectHandles.Remove(ItemDefinition);
}

void UPlayerInventoryComponent::ActivateCurrentEquippedItem()
{
	if (UPlayerItemDefinition* ActiveDefinition = GetItemDefinition(EPlayerItemSlot::Equipped))
	{
		const bool bActivated = ActiveDefinition->ActivateItem(GetOwnerPlayerCharacter(), this);
		if (bActivated)
		{
			const FPlayerInventoryItem ActiveItem = GetItem(EPlayerItemSlot::Equipped);
			if (UWorld* World = GetWorld())
			{
				if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
				{
					EventLog->ReportItemEquipped(GetOwner(), GetInventoryItemLabel(ActiveItem));
				}
			}
		}
	}
}

void UPlayerInventoryComponent::DeactivateCurrentEquippedItem()
{
	if (UPlayerItemDefinition* ActiveDefinition = GetItemDefinition(EPlayerItemSlot::Equipped))
	{
		ActiveDefinition->DeactivateItem(GetOwnerPlayerCharacter(), this);
	}
}

APlayerCharacterBase* UPlayerInventoryComponent::GetOwnerPlayerCharacter() const
{
	return Cast<APlayerCharacterBase>(GetOwner());
}

void UPlayerInventoryComponent::NormalizeActiveItemIndex(EPlayerItemSlot Slot)
{
	TArray<FPlayerInventoryItem>* Collection = ResolveMutableCollection(Slot);
	if (!Collection)
	{
		return;
	}

	int32& ActiveIndex = ResolveActiveItemIndex(Slot);
	if (Collection->IsEmpty())
	{
		ActiveIndex = INDEX_NONE;
		return;
	}

	ActiveIndex = FMath::Clamp(ActiveIndex, 0, Collection->Num() - 1);
}

int32& UPlayerInventoryComponent::ResolveActiveItemIndex(EPlayerItemSlot Slot)
{
	static int32 DummyIndex = INDEX_NONE;

	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return ActiveEquippedItemIndex;
	case EPlayerItemSlot::Support:
		return ActiveSupportItemIndex;
	case EPlayerItemSlot::Utility:
		return ActiveUtilityItemIndex;
	default:
		return DummyIndex;
	}
}

int32 UPlayerInventoryComponent::ResolveActiveItemIndex(EPlayerItemSlot Slot) const
{
	switch (Slot)
	{
	case EPlayerItemSlot::Equipped:
		return ActiveEquippedItemIndex;
	case EPlayerItemSlot::Support:
		return ActiveSupportItemIndex;
	case EPlayerItemSlot::Utility:
		return ActiveUtilityItemIndex;
	default:
		return INDEX_NONE;
	}
}

FPlayerInventoryItem* UPlayerInventoryComponent::ResolveMutableSlot(EPlayerItemSlot Slot)
{
	switch (Slot)
	{
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

void UPlayerInventoryComponent::BindRuntimeWeaponDelegates(EPlayerItemSlot Slot, AWeaponBase* WeaponActor)
{
	if (!WeaponActor)
	{
		return;
	}

	WeaponActor->OnWeaponFiredEvent().RemoveAll(this);
	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerInventoryComponent::OnPrimaryWeaponFired);
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		WeaponActor->OnWeaponFiredEvent().AddUObject(this, &UPlayerInventoryComponent::OnSecondaryWeaponFired);
	}
}

void UPlayerInventoryComponent::ClearRuntimeWeapon(FWeaponSlotRuntime& Runtime) const
{
	if (!Runtime.WeaponActor)
	{
		return;
	}

	Runtime.WeaponActor->OnWeaponFiredEvent().RemoveAll(this);
	Runtime.WeaponActor->Destroy();
	Runtime.WeaponActor = nullptr;
	Runtime.InClip = -1;
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
	const bool bHasActiveSlottedWeapon = HasValidWeaponSelection(ActiveWeaponSlot);

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
		SyncWeaponPresentation(UnarmedWeapon, false);
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
	SyncWeaponPresentation(UnarmedWeapon, !bHasActiveSlottedWeapon);
	if (bHasPrimaryItem)
	{
		AttachWeapon(PrimaryWeaponRuntime.WeaponActor, bPrimaryActive ? WeaponHandSocketName : PrimaryHolsterSocket, !bPrimaryActive);
	}
	if (bHasSecondaryItem)
	{
		AttachWeapon(SecondaryWeaponRuntime.WeaponActor, bPrimaryActive ? SecondaryWeaponHolsterSocketName : WeaponHandSocketName, bPrimaryActive);
	}
	if (!bHasActiveSlottedWeapon && UnarmedWeapon)
	{
		AttachWeapon(UnarmedWeapon, WeaponHandSocketName, false);
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
