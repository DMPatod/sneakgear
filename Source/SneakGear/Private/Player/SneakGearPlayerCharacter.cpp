#include "Player/SneakGearPlayerCharacter.h"

#include "Components/Cover/CoverComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/StealthPlayerController.h"
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

	EPlayerItemSlot StartupWeaponSlot = ItemComponent->GetActiveWeaponSlot();
	if (!ItemComponent->GetWeaponInSlot(StartupWeaponSlot))
	{
		if (ItemComponent->GetWeaponInSlot(EPlayerItemSlot::PrimaryWeapon))
		{
			StartupWeaponSlot = EPlayerItemSlot::PrimaryWeapon;
		}
		else if (ItemComponent->GetWeaponInSlot(EPlayerItemSlot::SecondaryWeapon))
		{
			StartupWeaponSlot = EPlayerItemSlot::SecondaryWeapon;
		}
		else
		{
			return;
		}
	}

	ItemComponent->SetActiveWeaponSlot(StartupWeaponSlot, true);
}

void ASneakGearPlayerCharacter::HandleActiveWeaponFired(EPlayerItemSlot FiredSlot)
{
	(void)FiredSlot;

	OnPlayerUIWeaponStateChanged.Broadcast();

	if (AStealthPlayerController* Controller = Cast<AStealthPlayerController>(GetController()))
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

	if (AStealthPlayerController* Controller = Cast<AStealthPlayerController>(GetController()))
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

	if (AStealthPlayerController* Controller = Cast<AStealthPlayerController>(GetController()))
	{
		Controller->OpenWeaponSelectionWidget(PendingWeaponSelectionSlot);
	}
}
