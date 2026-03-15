#include "Player/SneakGearPlayerCharacter.h"

#include "Components/Cover/CoverComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Misc/DataValidation.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/SneakGearPlayerController.h"
#include "TimerManager.h"
#include "Player/Components/PlayerWeaponComponent.h"
#include "Weapon/WeaponBase.h"

ASneakGearPlayerCharacter::ASneakGearPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	CoverStateComponent = CreateDefaultSubobject<UCoverStateComponent>(TEXT("CoverStateComponent"));
	ItemComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("ItemComponent"));
}

// Lifecycle
void ASneakGearPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeActiveWeaponFromInventory();

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->StopFire();
		WeaponComponent->GetCurrentWeapon()->SetActorHiddenInGame(true);
		WeaponComponent->GetCurrentWeapon()->SetActorEnableCollision(false);
	}

	if (ItemComponent)
	{
		ItemComponent->OnActiveWeaponFiredEvent().AddUObject(this, &ASneakGearPlayerCharacter::HandleActiveWeaponFired);
		ItemComponent->OnInventoryStateChangedEvent().AddUObject(this, &ASneakGearPlayerCharacter::HandleInventoryStateChanged);
	}
}

void ASneakGearPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WeaponSelectionHoldTimer);

	if (ItemComponent)
	{
		ItemComponent->OnActiveWeaponFiredEvent().RemoveAll(this);
		ItemComponent->OnInventoryStateChangedEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASneakGearPlayerCharacter::OnCharacterDeath()
{
	Super::OnCharacterDeath();

	GetWorldTimerManager().ClearTimer(WeaponSelectionHoldTimer);
	bWeaponSelectionButtonDown = false;
	bWeaponSelectionHoldTriggered = false;
	bIsVaulting = false;

	if (ItemComponent)
	{
		ItemComponent->StopActiveWeaponFire();
	}
}

EDataValidationResult ASneakGearPlayerCharacter::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ItemComponent)
	{
		Context.AddError(FText::FromString(TEXT("ItemComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!CoverComponent)
	{
		Context.AddError(FText::FromString(TEXT("CoverComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!CoverStateComponent)
	{
		Context.AddError(FText::FromString(TEXT("CoverStateComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!SelectPrimaryWeaponAction)
	{
		Context.AddWarning(FText::FromString(TEXT("SelectPrimaryWeaponAction is not assigned.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!SelectSecondaryWeaponAction)
	{
		Context.AddWarning(FText::FromString(TEXT("SelectSecondaryWeaponAction is not assigned.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!PickUpNearbyItemAction)
	{
		Context.AddWarning(FText::FromString(TEXT("PickUpNearbyItemAction is not assigned; nearby pickups cannot be collected via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!UseSupportItemAction)
	{
		Context.AddWarning(FText::FromString(TEXT("UseSupportItemAction is not assigned; support items cannot be used via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!UseUtilityItemAction)
	{
		Context.AddWarning(FText::FromString(TEXT("UseUtilityItemAction is not assigned; utility items cannot be used via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (NearbyPickupSearchRadius <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("NearbyPickupSearchRadius must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void ASneakGearPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	if (SelectPrimaryWeaponAction)
	{
		EnhancedInput->BindAction(SelectPrimaryWeaponAction, ETriggerEvent::Started, this,
		                          &ASneakGearPlayerCharacter::HandlePrimaryWeaponPressed);
		EnhancedInput->BindAction(SelectPrimaryWeaponAction, ETriggerEvent::Completed, this,
		                          &ASneakGearPlayerCharacter::HandlePrimaryWeaponReleased);
	}

	if (SelectSecondaryWeaponAction)
	{
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Started, this,
		                          &ASneakGearPlayerCharacter::HandleSecondaryWeaponPressed);
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Completed, this,
		                          &ASneakGearPlayerCharacter::HandleSecondaryWeaponReleased);
	}

	if (PickUpNearbyItemAction)
	{
		EnhancedInput->BindAction(PickUpNearbyItemAction, ETriggerEvent::Started, this,
		                          &ASneakGearPlayerCharacter::HandlePickUpNearbyItem);
	}

	if (UseSupportItemAction)
	{
		EnhancedInput->BindAction(UseSupportItemAction, ETriggerEvent::Started, this,
		                          &ASneakGearPlayerCharacter::HandleUseSupportItem);
	}

	if (UseUtilityItemAction)
	{
		EnhancedInput->BindAction(UseUtilityItemAction, ETriggerEvent::Started, this,
		                          &ASneakGearPlayerCharacter::HandleUseUtilityItem);
	}
}

void ASneakGearPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateNearbyPickup();

	if (!bDrawPickupRadiusDebug || !GetWorld())
	{
		return;
	}

	const FVector DrawOrigin = GetActorLocation();
	const FColor DrawColor = FColor::Cyan;
	DrawDebugSphere(
		GetWorld(),
		DrawOrigin,
		NearbyPickupSearchRadius,
		24,
		DrawColor,
		false,
		0.f,
		0,
		1.5f
	);
}

// Cover
void ASneakGearPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CoverStateComponent && CoverStateComponent->HandleMoveInput(this, Value))
	{
		return;
	}

	Super::Move(Value);
}

AWeaponBase* ASneakGearPlayerCharacter::GetCurrentWeapon() const
{
	return ItemComponent ? ItemComponent->GetActiveWeapon() : Super::GetCurrentWeapon();
}

bool ASneakGearPlayerCharacter::GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const
{
	const AWeaponBase* Weapon = GetCurrentWeapon();
	if (!Weapon || !ItemComponent)
	{
		return Super::GetWeaponStatusViewData(OutData);
	}

	OutData.bHasWeapon = true;
	OutData.WeaponName = FText::FromString(Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName());
	OutData.FireRate = Weapon->GetFireRate();
	OutData.InClip = ItemComponent->GetActiveWeaponInClip();
	OutData.ClipSize = ItemComponent->GetActiveWeaponClipSize();
	OutData.ReserveAmmo = ItemComponent->GetReserveAmmoCount();
	return true;
}

bool ASneakGearPlayerCharacter::GetWeaponQuickSlotViewData(EPlayerItemSlot Slot, FWeaponQuickSlotViewData& OutData) const
{
	OutData.Slot = Slot;
	if (!ItemComponent)
	{
		return false;
	}

	const AWeaponBase* Weapon = ItemComponent->GetWeaponInSlot(Slot);
	OutData.bHasWeapon = Weapon != nullptr;
	OutData.WeaponName = Weapon
		? FText::FromString(Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName())
		: NSLOCTEXT("SneakGear", "WeaponQuickIndicatorEmpty", "Empty");
	OutData.InClip = FMath::Max(ItemComponent->GetInClip(Slot), 0);
	OutData.ClipSize = FMath::Max(ItemComponent->GetClipSize(Slot), 0);
	OutData.ReserveAmmo = ItemComponent->GetReserveAmmoCount();
	return true;
}

bool ASneakGearPlayerCharacter::GetStealthDebugViewData(FStealthDebugViewData& OutData) const
{
	Super::GetStealthDebugViewData(OutData);

	OutData.bHasPlayer = true;
	OutData.bInCover = IsInCover();
	OutData.bVaulting = IsVaulting();
	OutData.CoverMoveAxis = GetCoverMoveAxis();
	OutData.Stance = Stance;

	if (ItemComponent)
	{
		OutData.bFiredRecently = ItemComponent->WasActiveWeaponFiredRecently();
		OutData.ActiveWeaponSlot = ItemComponent->GetActiveWeaponSlot();
		OutData.InClip = ItemComponent->GetActiveWeaponInClip();
		OutData.ClipSize = ItemComponent->GetActiveWeaponClipSize();
		OutData.ReserveAmmo = ItemComponent->GetReserveAmmoCount();
	}

	if (const AWeaponBase* Weapon = GetCurrentWeapon())
	{
		OutData.WeaponName = FText::FromString(Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName());
	}

	return true;
}

FText ASneakGearPlayerCharacter::GetInventoryItemDisplayName(EPlayerItemSlot Slot) const
{
	if (!ItemComponent)
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem Item = ItemComponent->GetItem(Slot);
	if (!Item.IsValid())
	{
		return NSLOCTEXT("SneakGear", "InventoryItemNone", "None");
	}

	return !Item.DisplayName.IsEmpty() ? Item.DisplayName : FText::FromName(Item.ItemId);
}

int32 ASneakGearPlayerCharacter::GetInventoryItemCount(EPlayerItemSlot Slot) const
{
	return ItemComponent ? ItemComponent->GetItemCount(Slot) : 0;
}

FText ASneakGearPlayerCharacter::GetInventoryItemDisplayNameAt(EPlayerItemSlot Slot, int32 Index) const
{
	if (!ItemComponent)
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem Item = ItemComponent->GetItemAt(Slot, Index);
	if (!Item.IsValid())
	{
		return FText::GetEmpty();
	}

	return !Item.DisplayName.IsEmpty() ? Item.DisplayName : FText::FromName(Item.ItemId);
}

int32 ASneakGearPlayerCharacter::GetActiveInventoryItemIndex(EPlayerItemSlot Slot) const
{
	return ItemComponent ? ItemComponent->GetActiveItemIndex(Slot) : INDEX_NONE;
}

bool ASneakGearPlayerCharacter::HasNearbyPickup() const
{
	return NearbyPickupComponent.IsValid();
}

FText ASneakGearPlayerCharacter::GetNearbyPickupDisplayName() const
{
	if (!NearbyPickupComponent.IsValid())
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem PickupItem = NearbyPickupComponent->GetPickupItem();
	if (!PickupItem.IsValid())
	{
		return FText::GetEmpty();
	}

	return !PickupItem.DisplayName.IsEmpty() ? PickupItem.DisplayName : FText::FromName(PickupItem.ItemId);
}

FText ASneakGearPlayerCharacter::GetNearbyPickupSlotLabel() const
{
	if (!NearbyPickupComponent.IsValid())
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem PickupItem = NearbyPickupComponent->GetPickupItem();
	if (!PickupItem.IsValid())
	{
		return FText::GetEmpty();
	}

	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	return SlotEnum
		? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(PickupItem.SlotType))
		: FText::GetEmpty();
}

#if WITH_DEV_AUTOMATION_TESTS
void ASneakGearPlayerCharacter::TestTriggerNearbyPickupInput()
{
	HandlePickUpNearbyItem();
}

void ASneakGearPlayerCharacter::TestTriggerUseSupportItemInput()
{
	HandleUseSupportItem();
}

void ASneakGearPlayerCharacter::TestTriggerUseUtilityItemInput()
{
	HandleUseUtilityItem();
}

void ASneakGearPlayerCharacter::TestTriggerPrimaryWeaponInput()
{
	HandlePrimaryWeaponPressed();
	HandlePrimaryWeaponReleased();
}
#endif

void ASneakGearPlayerCharacter::StartFire()
{
	if (!IsAiming())
	{
		return;
	}

	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->StartActiveWeaponFire();
		return;
	}

	Super::StartFire();
}

void ASneakGearPlayerCharacter::StopFire()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->StopActiveWeaponFire();
		return;
	}

	Super::StopFire();
}

void ASneakGearPlayerCharacter::ReloadWeapon()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->ReloadActiveWeapon();
		return;
	}

	Super::ReloadWeapon();
}

void ASneakGearPlayerCharacter::OnJumpPressed()
{
	if (CoverStateComponent && CoverStateComponent->TryVault(this))
	{
		// Match regular jump behavior: leave crouch stance when taking off.
		SetStance(EStance::Standing);
		bIsVaulting = true;
		return;
	}

	Super::OnJumpPressed();
}

void ASneakGearPlayerCharacter::OnJumpReleased()
{
	Super::OnJumpReleased();
}

void ASneakGearPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bIsVaulting && Stance == EStance::Crouching)
	{
		SetStance(EStance::Standing);
	}

	bIsVaulting = false;
}

void ASneakGearPlayerCharacter::InitializeActiveWeaponFromInventory()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon))
	{
		ItemComponent->SetActiveWeaponSlot(EPlayerItemSlot::PrimaryWeapon, true);
		return;
	}

	if (ItemComponent->GetWeaponInSlot(EPlayerItemSlot::SecondaryWeapon))
	{
		ItemComponent->SetActiveWeaponSlot(EPlayerItemSlot::SecondaryWeapon, true);
		return;
	}

	ItemComponent->SetWeaponEquipped(true);
}

void ASneakGearPlayerCharacter::UpdateNearbyPickup()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		NearbyPickupComponent = nullptr;
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerNearbyPickup), false, this);
	const FVector Center = GetActorLocation();
	const FCollisionShape SearchShape = FCollisionShape::MakeSphere(FMath::Max(NearbyPickupSearchRadius, 1.f));

	const bool bHasOverlaps = World->OverlapMultiByObjectType(
		Overlaps,
		Center,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		SearchShape,
		QueryParams
	);

	if (!bHasOverlaps)
	{
		NearbyPickupComponent = nullptr;
		return;
	}

	UPlayerItemPickupComponent* BestPickupComponent = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* PickupActor = Result.GetActor();
		if (!PickupActor)
		{
			continue;
		}

		UPlayerItemPickupComponent* PickupComponent = PickupActor->FindComponentByClass<UPlayerItemPickupComponent>();
		if (!PickupComponent)
		{
			continue;
		}

		const FPlayerInventoryItem PickupItem = PickupComponent->GetPickupItem();
		if (!PickupItem.IsValid())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Center, PickupActor->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestPickupComponent = PickupComponent;
		}
	}

	NearbyPickupComponent = BestPickupComponent;
}

void ASneakGearPlayerCharacter::HandleActiveWeaponFired(EPlayerItemSlot FiredSlot)
{
	(void)FiredSlot;

	OnPlayerUIWeaponStateChanged.Broadcast();

	if (ASneakGearPlayerController* Controller = Cast<ASneakGearPlayerController>(GetController()))
	{
		Controller->OnWeaponFired();
	}
}

void ASneakGearPlayerCharacter::HandleInventoryStateChanged()
{
	OnPlayerUIWeaponStateChanged.Broadcast();
}

// Weapon Selection
void ASneakGearPlayerCharacter::HandleWeaponSlotSelect(EPlayerItemSlot Slot)
{
	if (!ItemComponent)
	{
		return;
	}

	const bool bSameSlot = ItemComponent->GetActiveWeaponSlot() == Slot;
	const bool bCurrentlyEquipped = ItemComponent->IsWeaponEquipped();

	// Pressing the already-active weapon slot toggles to holstered state.
	if (bSameSlot && bCurrentlyEquipped)
	{
		ItemComponent->SetWeaponEquipped(false);
		return;
	}

	ItemComponent->SetActiveWeaponSlot(Slot, true);
}

void ASneakGearPlayerCharacter::HandlePrimaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::PrimaryWeapon);
}

void ASneakGearPlayerCharacter::HandlePrimaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::PrimaryWeapon);
}

void ASneakGearPlayerCharacter::HandleSecondaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::SecondaryWeapon);
}

void ASneakGearPlayerCharacter::HandleSecondaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::SecondaryWeapon);
}

void ASneakGearPlayerCharacter::HandlePickUpNearbyItem()
{
	if (!ItemComponent)
	{
		return;
	}

	ItemComponent->TryPickUpNearbyFloorItem(NearbyPickupSearchRadius);
}

void ASneakGearPlayerCharacter::HandleUseSupportItem()
{
	if (ItemComponent)
	{
		ItemComponent->UseActiveSupportItem();
	}
}

void ASneakGearPlayerCharacter::HandleUseUtilityItem()
{
	if (ItemComponent)
	{
		ItemComponent->UseActiveUtilityItem();
	}
}

void ASneakGearPlayerCharacter::HandleWeaponSlotPressed(EPlayerItemSlot Slot)
{
	bWeaponSelectionButtonDown = true;
	bWeaponSelectionHoldTriggered = false;
	PendingWeaponSelectionSlot = Slot;

	GetWorldTimerManager().SetTimer(
		WeaponSelectionHoldTimer,
		this,
		&ASneakGearPlayerCharacter::OnWeaponSelectHoldTriggered,
		FMath::Max(WeaponSelectionHoldTime, 0.05f),
		false
	);
}

void ASneakGearPlayerCharacter::HandleWeaponSlotReleased(EPlayerItemSlot Slot)
{
	if (Slot != PendingWeaponSelectionSlot)
	{
		return;
	}

	bWeaponSelectionButtonDown = false;
	GetWorldTimerManager().ClearTimer(WeaponSelectionHoldTimer);

	if (bWeaponSelectionHoldTriggered)
	{
		// Keep the selection menu open after hold-trigger release.
		// It now closes only when the user confirms a selection or cancels from the menu.
		return;
	}

	HandleWeaponSlotSelect(Slot);

	if (ASneakGearPlayerController* Controller = Cast<ASneakGearPlayerController>(GetController()))
	{
		Controller->ShowWeaponQuickSelectIndicator(Slot);
	}
}

void ASneakGearPlayerCharacter::OnWeaponSelectHoldTriggered()
{
	if (!bWeaponSelectionButtonDown)
	{
		return;
	}

	bWeaponSelectionHoldTriggered = true;

	if (ASneakGearPlayerController* Controller = Cast<ASneakGearPlayerController>(GetController()))
	{
		Controller->OpenWeaponSelectionWidget(PendingWeaponSelectionSlot);
	}
}
