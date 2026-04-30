#include "Player/Components/PlayerInventoryComponent.h"

#include "Items/PlayerItemDefinition.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Items/WorldItemPickup.h"
#include "Misc/DataValidation.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Components/PlayerInventoryPickupQuery.h"
#include "Player/Components/PlayerInventoryWeaponRuntime.h"
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

	AmmoReserves = InitialAmmoReserves;
	for (TPair<EAmmoType, FAmmoReserve>& Entry : AmmoReserves)
	{
		Entry.Value.Max = FMath::Max(Entry.Value.Max, 0);
		Entry.Value.Current = FMath::Clamp(Entry.Value.Current, 0, Entry.Value.Max);
	}

	FPlayerInventoryWeaponRuntime::Initialize(*this);

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

}

void UPlayerInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateCurrentEquippedItem();
	FPlayerInventoryWeaponRuntime::Shutdown(*this);

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
		FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(*this, Slot, nullptr);
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

	const bool bHadActiveWeaponSelection = HasValidWeaponSelection(ActiveWeaponSlot);

	if (!AddItemInternal(PickupItem, PickupComponent->GetItemDefinition(), bAllowReplace))
	{
		return false;
	}

	if (PickupItem.SlotType == EPlayerItemSlot::PrimaryWeapon || PickupItem.SlotType == EPlayerItemSlot::SecondaryWeapon)
	{
		SetItemDefinitionForSlot(PickupItem.SlotType, PickupComponent->GetItemDefinition());
		if (!FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(*this, PickupItem.SlotType, PickupComponent->GetPickupWeaponClass()))
		{
			return false;
		}
		if (!bHadActiveWeaponSelection || PickupItem.SlotType == EPlayerItemSlot::PrimaryWeapon)
		{
			SetActiveWeaponSlot(PickupItem.SlotType, true);
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
	AActor* BestPickup = FPlayerInventoryPickupQuery::FindBestNearbyFloorPickup(*this, SearchRadius);
	return BestPickup ? PickUpFromFloor(BestPickup, bAllowReplace) : false;
}

bool UPlayerInventoryComponent::RequiresHoldToSwapNearbyFloorItem(float SearchRadius) const
{
	AActor* BestPickup = FPlayerInventoryPickupQuery::FindBestNearbyFloorPickup(*this, SearchRadius);
	if (!BestPickup)
	{
		return false;
	}

	const UPlayerItemPickupComponent* PickupComponent = BestPickup->FindComponentByClass<UPlayerItemPickupComponent>();
	return FPlayerInventoryPickupQuery::PickupRequiresWeaponSwap(*this, PickupComponent);
}

bool UPlayerInventoryComponent::RequiresHoldToSwapItem(const FPlayerInventoryItem& Item) const
{
	if (Item.SlotType != EPlayerItemSlot::PrimaryWeapon && Item.SlotType != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	const FPlayerInventoryItem* ExistingItem = ResolveSlot(Item.SlotType);
	return ExistingItem && ExistingItem->IsValid();
}

bool UPlayerInventoryComponent::SwapNearbyFloorWeaponItem(float SearchRadius)
{
	AActor* BestPickup = FPlayerInventoryPickupQuery::FindBestNearbyFloorPickup(*this, SearchRadius);
	if (!BestPickup)
	{
		return false;
	}

	UPlayerItemPickupComponent* PickupComponent = BestPickup->FindComponentByClass<UPlayerItemPickupComponent>();
	if (!FPlayerInventoryPickupQuery::PickupRequiresWeaponSwap(*this, PickupComponent))
	{
		return false;
	}

	const FPlayerInventoryItem PickupItem = PickupComponent->GetPickupItem();
	UPlayerItemDefinition* PickupDefinition = PickupComponent->GetItemDefinition();
	const EPlayerItemSlot Slot = PickupItem.SlotType;
	FPlayerInventoryItem* ExistingItem = ResolveMutableSlot(Slot);
	UPlayerItemDefinition* ExistingDefinition = GetItemDefinition(Slot);
	if (!ExistingItem || !ExistingItem->IsValid() || !ExistingDefinition || !PickupDefinition)
	{
		return false;
	}

	const FPlayerInventoryItem PreviousItem = *ExistingItem;
	UPlayerItemDefinition* PreviousDefinition = ExistingDefinition;
	const TSubclassOf<AWeaponBase> PreviousWeaponClass = PreviousDefinition->WeaponClass;

	*ExistingItem = PickupItem;
	SetItemDefinitionForSlot(Slot, PickupDefinition);
	if (!FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(*this, Slot, PickupComponent->GetPickupWeaponClass()))
	{
		*ExistingItem = PreviousItem;
		SetItemDefinitionForSlot(Slot, PreviousDefinition);
		FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(*this, Slot, PreviousWeaponClass);
		return false;
	}

	PickupComponent->SetItemDefinition(PreviousDefinition);
	OnItemSlotUpdated.Broadcast(Slot);
	OnInventoryStateChanged.Broadcast();

	if (UWorld* World = GetWorld())
	{
		if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
		{
			EventLog->ReportItemPickedUp(GetOwner(), GetInventoryItemLabel(PickupItem));
		}
	}

	return true;
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
	return FPlayerInventoryWeaponRuntime::SetActiveWeaponSlot(*this, WeaponSlot, bEquipInHand);
}

bool UPlayerInventoryComponent::SetWeaponEquipped(bool bNewEquipped)
{
	return FPlayerInventoryWeaponRuntime::SetWeaponEquipped(*this, bNewEquipped);
}

bool UPlayerInventoryComponent::IsWeaponEquipped() const
{
	return bWeaponEquipped;
}

EPlayerItemSlot UPlayerInventoryComponent::GetActiveWeaponSlot() const
{
	return ActiveWeaponSlot;
}

FOnActiveWeaponFireRequested& UPlayerInventoryComponent::OnActiveWeaponFireRequestedEvent()
{
	return OnActiveWeaponFireRequested;
}

FOnActiveWeaponFired& UPlayerInventoryComponent::OnActiveWeaponFiredEvent()
{
	return OnActiveWeaponFired;
}

FOnInventoryStateChanged& UPlayerInventoryComponent::OnInventoryStateChangedEvent()
{
	return OnInventoryStateChanged;
}

AWeaponBase* UPlayerInventoryComponent::GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const
{
	return FPlayerInventoryWeaponRuntime::GetWeaponInSlot(*this, WeaponSlot);
}

AWeaponBase* UPlayerInventoryComponent::GetActiveWeapon() const
{
	return FPlayerInventoryWeaponRuntime::GetActiveWeapon(*this);
}

EPlayerInventoryWeaponState UPlayerInventoryComponent::GetWeaponState(EPlayerItemSlot WeaponSlot) const
{
	return FPlayerInventoryWeaponRuntime::GetWeaponState(*this, WeaponSlot);
}

EPlayerInventoryWeaponState UPlayerInventoryComponent::GetActiveWeaponState() const
{
	return FPlayerInventoryWeaponRuntime::GetActiveWeaponState(*this);
}

void UPlayerInventoryComponent::StartActiveWeaponFire()
{
	FPlayerInventoryWeaponRuntime::StartFire(*this);
}

void UPlayerInventoryComponent::StopActiveWeaponFire()
{
	FPlayerInventoryWeaponRuntime::StopFire(*this);
}

bool UPlayerInventoryComponent::ReloadActiveWeapon()
{
	return FPlayerInventoryWeaponRuntime::Reload(*this);
}

bool UPlayerInventoryComponent::NotifyActiveWeaponFireAnimation()
{
	return FPlayerInventoryWeaponRuntime::NotifyFireAnimation(*this);
}

bool UPlayerInventoryComponent::NotifyActiveWeaponReloadAnimationFinished()
{
	return FPlayerInventoryWeaponRuntime::NotifyReloadAnimationFinished(*this);
}

int32 UPlayerInventoryComponent::GetInClip(EPlayerItemSlot WeaponSlot) const
{
	return FPlayerInventoryWeaponRuntime::GetInClip(*this, WeaponSlot);
}

int32 UPlayerInventoryComponent::GetClipSize(EPlayerItemSlot WeaponSlot) const
{
	return FPlayerInventoryWeaponRuntime::GetClipSize(*this, WeaponSlot);
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

EAmmoType UPlayerInventoryComponent::GetAmmoTypeForSlot(EPlayerItemSlot Slot) const
{
	const AWeaponBase* Weapon = GetWeaponInSlot(Slot);
	return Weapon ? Weapon->GetAmmoType() : EAmmoType::None;
}

int32 UPlayerInventoryComponent::GetActiveWeaponInClip() const
{
	return FPlayerInventoryWeaponRuntime::GetActiveWeaponInClip(*this);
}

bool UPlayerInventoryComponent::IsActiveWeaponReloading() const
{
	return FPlayerInventoryWeaponRuntime::IsActiveWeaponReloading(*this);
}

int32 UPlayerInventoryComponent::GetActiveWeaponClipSize() const
{
	return FPlayerInventoryWeaponRuntime::GetActiveWeaponClipSize(*this);
}

bool UPlayerInventoryComponent::SetAmmoReserve(EAmmoType AmmoType, int32 CurrentAmount, int32 MaxAmount)
{
	if (AmmoType == EAmmoType::None)
	{
		return false;
	}

	FAmmoReserve& Reserve = AmmoReserves.FindOrAdd(AmmoType);
	Reserve.Max = FMath::Max(MaxAmount, 0);
	Reserve.Current = FMath::Clamp(CurrentAmount, 0, Reserve.Max);
	OnInventoryStateChanged.Broadcast();
	return true;
}

int32 UPlayerInventoryComponent::GetReserveAmmoCountForType(EAmmoType AmmoType) const
{
	const FAmmoReserve* Reserve = FindAmmoReserve(AmmoType);
	return Reserve ? FMath::Max(Reserve->Current, 0) : 0;
}

int32 UPlayerInventoryComponent::GetReserveAmmoCount() const
{
	return GetReserveAmmoCountForType(GetAmmoTypeForSlot(ActiveWeaponSlot));
}

bool UPlayerInventoryComponent::WasActiveWeaponFireRequestedRecently(float WindowSeconds) const
{
	return FPlayerInventoryWeaponRuntime::WasFireRequestedRecently(*this, WindowSeconds);
}

bool UPlayerInventoryComponent::IsActiveWeaponFireNotifyPending() const
{
	return FPlayerInventoryWeaponRuntime::IsFireNotifyPending(*this);
}

bool UPlayerInventoryComponent::WasActiveWeaponFiredRecently(float WindowSeconds) const
{
	return FPlayerInventoryWeaponRuntime::WasFiredRecently(*this, WindowSeconds);
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

FPlayerInventoryWeaponSlotRuntime* UPlayerInventoryComponent::ResolveWeaponRuntimeMutable(EPlayerItemSlot Slot)
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

const FPlayerInventoryWeaponSlotRuntime* UPlayerInventoryComponent::ResolveWeaponRuntime(EPlayerItemSlot Slot) const
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

void UPlayerInventoryComponent::HandleWeaponFireRequested(EPlayerItemSlot Slot)
{
	FPlayerInventoryWeaponRuntime::HandleFireRequested(*this, Slot);
}

void UPlayerInventoryComponent::HandleWeaponFired(EPlayerItemSlot Slot)
{
	FPlayerInventoryWeaponRuntime::HandleFired(*this, Slot);
}

void UPlayerInventoryComponent::OnPrimaryWeaponFireRequested()
{
	HandleWeaponFireRequested(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryComponent::OnSecondaryWeaponFireRequested()
{
	HandleWeaponFireRequested(EPlayerItemSlot::SecondaryWeapon);
}

void UPlayerInventoryComponent::OnPrimaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryComponent::OnSecondaryWeaponFired()
{
	HandleWeaponFired(EPlayerItemSlot::SecondaryWeapon);
}

void UPlayerInventoryComponent::HandleWeaponReloaded(EPlayerItemSlot Slot)
{
	FPlayerInventoryWeaponRuntime::HandleReloaded(*this, Slot);
}

void UPlayerInventoryComponent::OnPrimaryWeaponReloaded()
{
	HandleWeaponReloaded(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryComponent::OnSecondaryWeaponReloaded()
{
	HandleWeaponReloaded(EPlayerItemSlot::SecondaryWeapon);
}

FAmmoReserve* UPlayerInventoryComponent::FindAmmoReserveMutable(EAmmoType AmmoType)
{
	return AmmoType == EAmmoType::None ? nullptr : AmmoReserves.Find(AmmoType);
}

const FAmmoReserve* UPlayerInventoryComponent::FindAmmoReserve(EAmmoType AmmoType) const
{
	return AmmoType == EAmmoType::None ? nullptr : AmmoReserves.Find(AmmoType);
}

int32 UPlayerInventoryComponent::ConsumeReserveAmmo(EAmmoType AmmoType, int32 Amount)
{
	FAmmoReserve* Reserve = FindAmmoReserveMutable(AmmoType);
	if (!Reserve || Amount <= 0)
	{
		return 0;
	}

	const int32 Consumed = FMath::Clamp(Amount, 0, FMath::Max(Reserve->Current, 0));
	Reserve->Current = FMath::Max(Reserve->Current - Consumed, 0);
	return Consumed;
}
