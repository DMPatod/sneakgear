#include "Player/SneakGearPlayerCharacter.h"

#include "Components/Cover/CoverComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "DrawDebugHelpers.h"
#include "Misc/DataValidation.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/Components/PlayerInventoryInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/SneakGearPlayerController.h"
#include "UI/EventLogSubsystem.h"
#include "Weapon/WeaponBase.h"

ASneakGearPlayerCharacter::ASneakGearPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	CoverStateComponent = CreateDefaultSubobject<UCoverStateComponent>(TEXT("CoverStateComponent"));
	ItemComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("ItemComponent"));
	InventoryInteractionComponent = CreateDefaultSubobject<UPlayerInventoryInteractionComponent>(TEXT("InventoryInteractionComponent"));
}

bool ASneakGearPlayerCharacter::IsInCover() const
{
	return CoverStateComponent ? CoverStateComponent->IsInCover() : false;
}

float ASneakGearPlayerCharacter::GetCoverMoveAxis() const
{
	return CoverStateComponent ? CoverStateComponent->GetCoverMoveAxis() : 0.f;
}

float ASneakGearPlayerCharacter::GetCoverObstacleHeight() const
{
	return CoverStateComponent ? CoverStateComponent->GetCurrentCoverObstacleHeight() : 0.f;
}

float ASneakGearPlayerCharacter::GetCurrentVaultMaxObstacleHeight() const
{
	return CoverStateComponent ? CoverStateComponent->GetCurrentVaultMaxObstacleHeight(this) : 0.f;
}

// Lifecycle
void ASneakGearPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->InitializeActiveWeaponFromInventory();
	}

	if (ItemComponent)
	{
		ItemComponent->OnActiveWeaponFiredEvent().AddUObject(this, &ASneakGearPlayerCharacter::HandleActiveWeaponFired);
		ItemComponent->OnInventoryStateChangedEvent().AddUObject(
			this, &ASneakGearPlayerCharacter::HandleInventoryStateChanged);
	}
}

void ASneakGearPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->ResetInteractionState();
	}

	if (ItemComponent)
	{
		ItemComponent->OnActiveWeaponFiredEvent().RemoveAll(this);
		ItemComponent->OnInventoryStateChangedEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASneakGearPlayerCharacter::OnCharacterDeath()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->ResetInteractionState();
	}

	if (CoverStateComponent)
	{
		CoverStateComponent->HandleLanded(nullptr);
	}

	if (ItemComponent)
	{
		ItemComponent->StopActiveWeaponFire();
	}

	Super::OnCharacterDeath();
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
		Context.AddWarning(FText::FromString(
			TEXT("PickUpNearbyItemAction is not assigned; nearby pickups cannot be collected via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!UseSupportItemAction)
	{
		Context.AddWarning(
			FText::FromString(TEXT("UseSupportItemAction is not assigned; support items cannot be used via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!UseUtilityItemAction)
	{
		Context.AddWarning(
			FText::FromString(TEXT("UseUtilityItemAction is not assigned; utility items cannot be used via input.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
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
		EnhancedInput->BindAction(SelectPrimaryWeaponAction, ETriggerEvent::Started, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandlePrimaryWeaponPressed);
		EnhancedInput->BindAction(SelectPrimaryWeaponAction, ETriggerEvent::Completed, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandlePrimaryWeaponReleased);
	}

	if (SelectSecondaryWeaponAction)
	{
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Started, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandleSecondaryWeaponPressed);
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Completed, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandleSecondaryWeaponReleased);
	}

	if (PickUpNearbyItemAction)
	{
		EnhancedInput->BindAction(PickUpNearbyItemAction, ETriggerEvent::Started, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandlePickUpNearbyItemPressed);
		EnhancedInput->BindAction(PickUpNearbyItemAction, ETriggerEvent::Completed, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandlePickUpNearbyItemReleased);
	}

	if (UseSupportItemAction)
	{
		EnhancedInput->BindAction(UseSupportItemAction, ETriggerEvent::Started, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandleUseSupportItem);
	}

	if (UseUtilityItemAction)
	{
		EnhancedInput->BindAction(UseUtilityItemAction, ETriggerEvent::Started, InventoryInteractionComponent.Get(),
		                          &UPlayerInventoryInteractionComponent::HandleUseUtilityItem);
	}
}

void ASneakGearPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->UpdateNearbyPickup();
	}

	if (!bDrawPickupRadiusDebug || !GetWorld())
	{
		return;
	}

	const FVector DrawOrigin = GetActorLocation();
	const FColor DrawColor = FColor::Cyan;
	DrawDebugSphere(
		GetWorld(),
		DrawOrigin,
		InventoryInteractionComponent ? InventoryInteractionComponent->GetNearbyPickupSearchRadius() : 0.f,
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
	return ItemComponent ? ItemComponent->GetActiveWeapon() : nullptr;
}

bool ASneakGearPlayerCharacter::GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const
{
	if (!ItemComponent)
	{
		return Super::GetWeaponStatusViewData(OutData);
	}

	const EPlayerItemSlot ActiveSlot = ItemComponent->GetActiveWeaponSlot();
	const AWeaponBase* Weapon = ItemComponent->GetWeaponInSlot(ActiveSlot);
	if (!Weapon)
	{
		OutData = FWeaponStatusViewData();
		return false;
	}

	OutData.bHasWeapon = true;
	OutData.WeaponName = GetInventoryItemDisplayName(ActiveSlot);
	OutData.FireRate = 0.f;
	OutData.InClip = ItemComponent->GetActiveWeaponInClip();
	OutData.ClipSize = ItemComponent->GetActiveWeaponClipSize();
	OutData.ReserveAmmo = ItemComponent->GetReserveAmmoCount();
	return true;
}

bool ASneakGearPlayerCharacter::GetWeaponQuickSlotViewData(EPlayerItemSlot Slot,
                                                           FWeaponQuickSlotViewData& OutData) const
{
	OutData.Slot = Slot;
	if (!ItemComponent)
	{
		return false;
	}

	const AWeaponBase* Weapon = ItemComponent->GetWeaponInSlot(Slot);
	OutData.bHasWeapon = Weapon != nullptr;
	OutData.WeaponName = Weapon
		                     ? GetInventoryItemDisplayName(Slot)
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

		if (ItemComponent->GetWeaponInSlot(OutData.ActiveWeaponSlot))
		{
			OutData.WeaponName = GetInventoryItemDisplayName(OutData.ActiveWeaponSlot);
		}
		else
		{
			OutData.WeaponName = FText::GetEmpty();
		}
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

bool ASneakGearPlayerCharacter::IsVaulting() const
{
	return CoverStateComponent && CoverStateComponent->IsVaulting();
}

bool ASneakGearPlayerCharacter::IsVaultAvailable() const
{
	return CoverStateComponent && CoverStateComponent->CanVault(this);
}

bool ASneakGearPlayerCharacter::HasNearbyPickup() const
{
	return InventoryInteractionComponent && InventoryInteractionComponent->HasNearbyPickup();
}

FText ASneakGearPlayerCharacter::GetNearbyPickupDisplayName() const
{
	return InventoryInteractionComponent ? InventoryInteractionComponent->GetNearbyPickupDisplayName() : FText::GetEmpty();
}

FText ASneakGearPlayerCharacter::GetNearbyPickupSlotLabel() const
{
	return InventoryInteractionComponent ? InventoryInteractionComponent->GetNearbyPickupSlotLabel() : FText::GetEmpty();
}

bool ASneakGearPlayerCharacter::IsPickupSwapHoldActive() const
{
	return InventoryInteractionComponent && InventoryInteractionComponent->IsPickupSwapHoldActive();
}

float ASneakGearPlayerCharacter::GetPickupSwapHoldProgress() const
{
	return InventoryInteractionComponent ? InventoryInteractionComponent->GetPickupSwapHoldProgress() : 0.f;
}

#if WITH_DEV_AUTOMATION_TESTS
void ASneakGearPlayerCharacter::TestTriggerNearbyPickupInput()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->HandlePickUpNearbyItemPressed();
		InventoryInteractionComponent->HandlePickUpNearbyItemReleased();
	}
}

void ASneakGearPlayerCharacter::TestTriggerNearbyPickupHoldInput()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->HandlePickUpNearbyItemPressed();
		InventoryInteractionComponent->TriggerPickupSwapHoldForTest();
		InventoryInteractionComponent->HandlePickUpNearbyItemReleased();
	}
}

void ASneakGearPlayerCharacter::TestTriggerUseSupportItemInput()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->HandleUseSupportItem();
	}
}

void ASneakGearPlayerCharacter::TestTriggerUseUtilityItemInput()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->HandleUseUtilityItem();
	}
}

void ASneakGearPlayerCharacter::TestTriggerPrimaryWeaponInput()
{
	if (InventoryInteractionComponent)
	{
		InventoryInteractionComponent->HandlePrimaryWeaponPressed();
		InventoryInteractionComponent->HandlePrimaryWeaponReleased();
	}
}

void ASneakGearPlayerCharacter::TestTriggerJumpInput()
{
	OnJumpPressed();
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
	}
}

void ASneakGearPlayerCharacter::StopFire()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->StopActiveWeaponFire();
	}
}

void ASneakGearPlayerCharacter::ReloadWeapon()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->ReloadActiveWeapon();
	}
}

void ASneakGearPlayerCharacter::ToggleEquip()
{
	if (ItemComponent)
	{
		ItemComponent->SetWeaponEquipped(!ItemComponent->IsWeaponEquipped());
	}
}

void ASneakGearPlayerCharacter::OnJumpPressed()
{
	if (CoverStateComponent && CoverStateComponent->TryVault(this))
	{
		// Match regular jump behavior: leave crouch stance when taking off.
		SetStance(EStance::Standing);
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

	if (CoverStateComponent)
	{
		CoverStateComponent->HandleLanded(this);
	}
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
