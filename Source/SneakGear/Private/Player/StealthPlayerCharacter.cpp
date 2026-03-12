#include "Player/StealthPlayerCharacter.h"

#include "Components/Cover/CoverComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "Player/Components/PlayerItemComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/StealthPlayerController.h"
#include "TimerManager.h"
#include "Player/Components/PlayerWeaponComponent.h"
#include "Weapon/WeaponBase.h"

AStealthPlayerCharacter::AStealthPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	CoverStateComponent = CreateDefaultSubobject<UCoverStateComponent>(TEXT("CoverStateComponent"));
	ItemComponent = CreateDefaultSubobject<UPlayerItemComponent>(TEXT("ItemComponent"));
}

// Lifecycle
void AStealthPlayerCharacter::BeginPlay()
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
		ItemComponent->OnActiveWeaponFiredEvent().AddUObject(this, &AStealthPlayerCharacter::HandleActiveWeaponFired);
	}
}

void AStealthPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WeaponSelectionHoldTimer);

	if (ItemComponent)
	{
		ItemComponent->OnActiveWeaponFiredEvent().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AStealthPlayerCharacter::OnCharacterDeath()
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

void AStealthPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		                          &AStealthPlayerCharacter::HandlePrimaryWeaponPressed);
		EnhancedInput->BindAction(SelectPrimaryWeaponAction, ETriggerEvent::Completed, this,
		                          &AStealthPlayerCharacter::HandlePrimaryWeaponReleased);
	}

	if (SelectSecondaryWeaponAction)
	{
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Started, this,
		                          &AStealthPlayerCharacter::HandleSecondaryWeaponPressed);
		EnhancedInput->BindAction(SelectSecondaryWeaponAction, ETriggerEvent::Completed, this,
		                          &AStealthPlayerCharacter::HandleSecondaryWeaponReleased);
	}
}

// Cover
void AStealthPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CoverStateComponent && CoverStateComponent->HandleMoveInput(this, Value))
	{
		return;
	}

	Super::Move(Value);
}

AWeaponBase* AStealthPlayerCharacter::GetCurrentWeapon() const
{
	return ItemComponent ? ItemComponent->GetActiveWeapon() : Super::GetCurrentWeapon();
}

void AStealthPlayerCharacter::StartFire()
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

void AStealthPlayerCharacter::StopFire()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->StopActiveWeaponFire();
		return;
	}

	Super::StopFire();
}

void AStealthPlayerCharacter::ReloadWeapon()
{
	if (ItemComponent && ItemComponent->GetActiveWeapon())
	{
		ItemComponent->ReloadActiveWeapon();
		return;
	}

	Super::ReloadWeapon();
}

void AStealthPlayerCharacter::OnJumpPressed()
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

void AStealthPlayerCharacter::OnJumpReleased()
{
	Super::OnJumpReleased();
}

void AStealthPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bIsVaulting && Stance == EStance::Crouching)
	{
		SetStance(EStance::Standing);
	}

	bIsVaulting = false;
}

void AStealthPlayerCharacter::InitializeActiveWeaponFromInventory()
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

void AStealthPlayerCharacter::HandleActiveWeaponFired(EPlayerItemSlot FiredSlot)
{
	(void)FiredSlot;

	if (AStealthPlayerController* Controller = Cast<AStealthPlayerController>(GetController()))
	{
		Controller->OnWeaponFired();
	}
}

// Weapon Selection
void AStealthPlayerCharacter::HandleWeaponSlotSelect(EPlayerItemSlot Slot)
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

void AStealthPlayerCharacter::HandlePrimaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::PrimaryWeapon);
}

void AStealthPlayerCharacter::HandlePrimaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::PrimaryWeapon);
}

void AStealthPlayerCharacter::HandleSecondaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::SecondaryWeapon);
}

void AStealthPlayerCharacter::HandleSecondaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::SecondaryWeapon);
}

void AStealthPlayerCharacter::HandleWeaponSlotPressed(EPlayerItemSlot Slot)
{
	bWeaponSelectionButtonDown = true;
	bWeaponSelectionHoldTriggered = false;
	PendingWeaponSelectionSlot = Slot;

	GetWorldTimerManager().SetTimer(
		WeaponSelectionHoldTimer,
		this,
		&AStealthPlayerCharacter::OnWeaponSelectHoldTriggered,
		FMath::Max(WeaponSelectionHoldTime, 0.05f),
		false
	);
}

void AStealthPlayerCharacter::HandleWeaponSlotReleased(EPlayerItemSlot Slot)
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

void AStealthPlayerCharacter::OnWeaponSelectHoldTriggered()
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
