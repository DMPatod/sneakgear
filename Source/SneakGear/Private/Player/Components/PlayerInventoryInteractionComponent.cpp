#include "Player/Components/PlayerInventoryInteractionComponent.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Controller.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Player/SneakGearPlayerController.h"

UPlayerInventoryInteractionComponent::UPlayerInventoryInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInventoryInteractionComponent::InitializeActiveWeaponFromInventory()
{
	UPlayerInventoryComponent* ItemComponent = GetInventoryComponent();
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

void UPlayerInventoryInteractionComponent::ResetInteractionState()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WeaponSelectionHoldTimer);
		World->GetTimerManager().ClearTimer(PickupSwapHoldTimer);
	}

	NearbyPickupComponent = nullptr;
	bWeaponSelectionButtonDown = false;
	bWeaponSelectionHoldTriggered = false;
	bPickupButtonDown = false;
	bPickupHoldTriggered = false;
	PendingWeaponSelectionSlot = EPlayerItemSlot::PrimaryWeapon;
}

void UPlayerInventoryInteractionComponent::UpdateNearbyPickup()
{
	UWorld* World = GetWorld();
	ASneakGearPlayerCharacter* Character = GetOwnerCharacter();
	if (!World || !Character)
	{
		NearbyPickupComponent = nullptr;
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerNearbyPickup), false, Character);
	const FVector Center = Character->GetActorLocation();
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

bool UPlayerInventoryInteractionComponent::HasNearbyPickup() const
{
	return NearbyPickupComponent.IsValid();
}

FText UPlayerInventoryInteractionComponent::GetNearbyPickupDisplayName() const
{
	if (!NearbyPickupComponent.IsValid())
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem PickupItem = NearbyPickupComponent->GetPickupItem();
	return !PickupItem.DisplayName.IsEmpty() ? PickupItem.DisplayName : FText::FromName(PickupItem.ItemId);
}

FText UPlayerInventoryInteractionComponent::GetNearbyPickupSlotLabel() const
{
	if (!NearbyPickupComponent.IsValid())
	{
		return FText::GetEmpty();
	}

	const FPlayerInventoryItem PickupItem = NearbyPickupComponent->GetPickupItem();
	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	return SlotEnum
		       ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(PickupItem.SlotType))
		       : FText::GetEmpty();
}

float UPlayerInventoryInteractionComponent::GetNearbyPickupSearchRadius() const
{
	return NearbyPickupSearchRadius;
}

bool UPlayerInventoryInteractionComponent::IsPickupSwapHoldActive() const
{
	const UWorld* World = GetWorld();
	return World && bPickupButtonDown && !bPickupHoldTriggered && World->GetTimerManager().IsTimerActive(PickupSwapHoldTimer);
}

float UPlayerInventoryInteractionComponent::GetPickupSwapHoldProgress() const
{
	const UWorld* World = GetWorld();
	if (!World || !IsPickupSwapHoldActive())
	{
		return bPickupHoldTriggered ? 1.f : 0.f;
	}

	const float HoldDuration = FMath::Max(PickupSwapHoldTime, 0.05f);
	const float Elapsed = World->GetTimerManager().GetTimerElapsed(PickupSwapHoldTimer);
	return FMath::Clamp(Elapsed / HoldDuration, 0.f, 1.f);
}

void UPlayerInventoryInteractionComponent::HandlePrimaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryInteractionComponent::HandlePrimaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::PrimaryWeapon);
}

void UPlayerInventoryInteractionComponent::HandleSecondaryWeaponPressed()
{
	HandleWeaponSlotPressed(EPlayerItemSlot::SecondaryWeapon);
}

void UPlayerInventoryInteractionComponent::HandleSecondaryWeaponReleased()
{
	HandleWeaponSlotReleased(EPlayerItemSlot::SecondaryWeapon);
}

void UPlayerInventoryInteractionComponent::HandlePickUpNearbyItemPressed()
{
	if (!GetInventoryComponent())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bPickupButtonDown = true;
	bPickupHoldTriggered = false;

	World->GetTimerManager().SetTimer(
		PickupSwapHoldTimer,
		this,
		&UPlayerInventoryInteractionComponent::OnPickupSwapHoldTriggered,
		FMath::Max(PickupSwapHoldTime, 0.05f),
		false
	);
}

void UPlayerInventoryInteractionComponent::HandlePickUpNearbyItemReleased()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(PickupSwapHoldTimer);
	}

	bPickupButtonDown = false;

	UPlayerInventoryComponent* ItemComponent = GetInventoryComponent();
	if (!ItemComponent || bPickupHoldTriggered)
	{
		return;
	}

	if (ItemComponent->RequiresHoldToSwapNearbyFloorItem(NearbyPickupSearchRadius))
	{
		return;
	}

	ItemComponent->TryPickUpNearbyFloorItem(NearbyPickupSearchRadius, false);
}

void UPlayerInventoryInteractionComponent::HandleUseSupportItem()
{
	if (UPlayerInventoryComponent* ItemComponent = GetInventoryComponent())
	{
		ItemComponent->UseActiveSupportItem();
	}
}

void UPlayerInventoryInteractionComponent::HandleUseUtilityItem()
{
	if (UPlayerInventoryComponent* ItemComponent = GetInventoryComponent())
	{
		ItemComponent->UseActiveUtilityItem();
	}
}

void UPlayerInventoryInteractionComponent::TriggerPickupSwapHoldForTest()
{
	OnPickupSwapHoldTriggered();
}

ASneakGearPlayerCharacter* UPlayerInventoryInteractionComponent::GetOwnerCharacter() const
{
	return Cast<ASneakGearPlayerCharacter>(GetOwner());
}

UPlayerInventoryComponent* UPlayerInventoryInteractionComponent::GetInventoryComponent() const
{
	ASneakGearPlayerCharacter* Character = GetOwnerCharacter();
	return Character ? Character->GetItemComponent() : nullptr;
}

void UPlayerInventoryInteractionComponent::HandleWeaponSlotPressed(EPlayerItemSlot Slot)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bWeaponSelectionButtonDown = true;
	bWeaponSelectionHoldTriggered = false;
	PendingWeaponSelectionSlot = Slot;
	World->GetTimerManager().SetTimer(
		WeaponSelectionHoldTimer,
		this,
		&UPlayerInventoryInteractionComponent::OnWeaponSelectHoldTriggered,
		FMath::Max(WeaponSelectionHoldTime, 0.05f),
		false
	);
}

void UPlayerInventoryInteractionComponent::HandleWeaponSlotReleased(EPlayerItemSlot Slot)
{
	if (Slot != PendingWeaponSelectionSlot)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(WeaponSelectionHoldTimer);
	}

	bWeaponSelectionButtonDown = false;

	if (bWeaponSelectionHoldTriggered)
	{
		return;
	}

	HandleWeaponSlotSelect(Slot);

	if (ASneakGearPlayerController* Controller = Cast<ASneakGearPlayerController>(GetOwnerCharacter() ? GetOwnerCharacter()->GetController() : nullptr))
	{
		Controller->ShowWeaponQuickSelectIndicator(Slot);
	}
}

void UPlayerInventoryInteractionComponent::HandleWeaponSlotSelect(EPlayerItemSlot Slot)
{
	UPlayerInventoryComponent* ItemComponent = GetInventoryComponent();
	ASneakGearPlayerCharacter* Character = GetOwnerCharacter();
	if (!ItemComponent || !Character)
	{
		return;
	}

	const bool bSameSlot = ItemComponent->GetActiveWeaponSlot() == Slot;
	const bool bCurrentlyEquipped = ItemComponent->IsWeaponEquipped();
	if (bSameSlot && bCurrentlyEquipped)
	{
		if (ItemComponent->SetWeaponEquipped(false))
		{
			Character->OnPlayerUIWeaponStateChangedEvent().Broadcast();
		}
		return;
	}

	if (ItemComponent->SetActiveWeaponSlot(Slot, true))
	{
		Character->OnPlayerUIWeaponStateChangedEvent().Broadcast();
	}
}

void UPlayerInventoryInteractionComponent::OnWeaponSelectHoldTriggered()
{
	if (!bWeaponSelectionButtonDown)
	{
		return;
	}

	bWeaponSelectionHoldTriggered = true;

	if (ASneakGearPlayerController* Controller = Cast<ASneakGearPlayerController>(GetOwnerCharacter() ? GetOwnerCharacter()->GetController() : nullptr))
	{
		Controller->OpenWeaponSelectionWidget(PendingWeaponSelectionSlot);
	}
}

void UPlayerInventoryInteractionComponent::OnPickupSwapHoldTriggered()
{
	if (!bPickupButtonDown)
	{
		return;
	}

	if (UPlayerInventoryComponent* ItemComponent = GetInventoryComponent())
	{
		bPickupHoldTriggered = true;
		ItemComponent->SwapNearbyFloorWeaponItem(NearbyPickupSearchRadius);
	}
}
