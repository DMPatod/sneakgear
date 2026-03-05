#include "Characters/Player/StealthPlayerCharacter.h"

#include "CollisionQueryParams.h"
#include "Components/Cover/CoverComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "Components/PlayerItemComponent.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Engine/OverlapResult.h"
#include "Game/StealthPlayerController.h"
#include "Items/WorldItemPickup.h"
#include "TimerManager.h"
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

// Inventory
bool AStealthPlayerCharacter::TryPickUpFloorItem(AWorldItemPickup* PickupActor, bool bAllowReplace)
{
	if (!ItemComponent)
	{
		return false;
	}

	return ItemComponent->PickUpFromFloor(PickupActor, bAllowReplace);
}

bool AStealthPlayerCharacter::TryPickUpNearbyFloorItem(float SearchRadius, bool bAllowReplace)
{
	UWorld* World = GetWorld();
	if (!World || !ItemComponent)
	{
		return false;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FloorItemPickup), false, this);
	const FVector Center = GetActorLocation();
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

	return BestPickup ? TryPickUpFloorItem(BestPickup, bAllowReplace) : false;
}

// Weapons
bool AStealthPlayerCharacter::SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand)
{
	return ItemComponent ? ItemComponent->SetActiveWeaponSlot(WeaponSlot, bEquipInHand) : false;
}

EPlayerItemSlot AStealthPlayerCharacter::GetActiveWeaponSlot() const
{
	return ItemComponent ? ItemComponent->GetActiveWeaponSlot() : EPlayerItemSlot::PrimaryWeapon;
}

bool AStealthPlayerCharacter::WasWeaponFiredRecently(float WindowSeconds) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float SafeWindow = FMath::Max(WindowSeconds, 0.01f);
	return (World->GetTimeSeconds() - LastWeaponFireTimestamp) <= SafeWindow;
}

int32 AStealthPlayerCharacter::GetActiveWeaponInClip() const
{
	if (!ItemComponent)
	{
		return 0;
	}

	return FMath::Max(ItemComponent->GetInClip(GetActiveWeaponSlot()), 0);
}

int32 AStealthPlayerCharacter::GetActiveWeaponClipSize() const
{
	if (!ItemComponent)
	{
		return 0;
	}

	return FMath::Max(ItemComponent->GetClipSize(GetActiveWeaponSlot()), 0);
}

int32 AStealthPlayerCharacter::GetReserveAmmoCount() const
{
	return FMath::Max(FMath::FloorToInt(GetAmmo()), 0);
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
		return;
	}

	Super::OnJumpPressed();
}

void AStealthPlayerCharacter::OnJumpReleased()
{
	Super::OnJumpReleased();
}

void AStealthPlayerCharacter::HandleActiveWeaponFired(EPlayerItemSlot FiredSlot)
{
	(void)FiredSlot;

	LastWeaponFireTimestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : LastWeaponFireTimestamp;

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
		if (AStealthPlayerController* Controller = Cast<AStealthPlayerController>(GetController()))
		{
			Controller->CloseWeaponSelectionWidget();
		}
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
