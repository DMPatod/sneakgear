#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/PlayerInventoryTypes.h"
#include "PlayerInventoryInteractionComponent.generated.h"

class ASneakGearPlayerCharacter;
class UPlayerInventoryComponent;
class UPlayerItemPickupComponent;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerInventoryInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInventoryInteractionComponent();

	void InitializeActiveWeaponFromInventory();
	void ResetInteractionState();
	void UpdateNearbyPickup();

	bool HasNearbyPickup() const;
	FText GetNearbyPickupDisplayName() const;
	FText GetNearbyPickupSlotLabel() const;
	float GetNearbyPickupSearchRadius() const;
	bool IsPickupSwapHoldActive() const;
	float GetPickupSwapHoldProgress() const;

	void HandlePrimaryWeaponPressed();
	void HandlePrimaryWeaponReleased();
	void HandleSecondaryWeaponPressed();
	void HandleSecondaryWeaponReleased();
	void HandlePickUpNearbyItemPressed();
	void HandlePickUpNearbyItemReleased();
	void HandleUseSupportItem();
	void HandleUseUtilityItem();
	void TriggerPickupSwapHoldForTest();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input|Weapons")
	float WeaponSelectionHoldTime = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	float NearbyPickupSearchRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	float PickupSwapHoldTime = 0.35f;

private:
	ASneakGearPlayerCharacter* GetOwnerCharacter() const;
	UPlayerInventoryComponent* GetInventoryComponent() const;
	void HandleWeaponSlotPressed(EPlayerItemSlot Slot);
	void HandleWeaponSlotReleased(EPlayerItemSlot Slot);
	void HandleWeaponSlotSelect(EPlayerItemSlot Slot);
	void OnWeaponSelectHoldTriggered();
	void OnPickupSwapHoldTriggered();

	FTimerHandle WeaponSelectionHoldTimer;
	FTimerHandle PickupSwapHoldTimer;
	TWeakObjectPtr<UPlayerItemPickupComponent> NearbyPickupComponent;
	EPlayerItemSlot PendingWeaponSelectionSlot = EPlayerItemSlot::PrimaryWeapon;
	bool bWeaponSelectionButtonDown = false;
	bool bWeaponSelectionHoldTriggered = false;
	bool bPickupButtonDown = false;
	bool bPickupHoldTriggered = false;
};
