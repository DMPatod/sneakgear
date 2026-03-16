#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "SneakGearPlayerCharacter.generated.h"

class UCoverComponent;
class UCoverStateComponent;
class UPlayerInventoryComponent;
class AWorldItemPickup;
class AWeaponBase;
class UInputAction;
class UPlayerItemPickupComponent;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API ASneakGearPlayerCharacter : public APlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASneakGearPlayerCharacter();

	bool IsInCover() const
	{
		return CoverStateComponent ? CoverStateComponent->IsInCover() : false;
	}

	float GetCoverMoveAxis() const
	{
		return CoverStateComponent ? CoverStateComponent->GetCoverMoveAxis() : 0.f;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	UPlayerInventoryComponent* GetItemComponent() const
	{
		return ItemComponent;
	}

	UFUNCTION(BlueprintPure, Category="Cover")
	bool IsVaulting() const
	{
		return bIsVaulting;
	}

	virtual AWeaponBase* GetCurrentWeapon() const override;
	virtual bool GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const override;
	virtual bool GetWeaponQuickSlotViewData(EPlayerItemSlot Slot, FWeaponQuickSlotViewData& OutData) const override;
	virtual bool GetStealthDebugViewData(FStealthDebugViewData& OutData) const override;
	virtual FText GetInventoryItemDisplayName(EPlayerItemSlot Slot) const override;
	virtual int32 GetInventoryItemCount(EPlayerItemSlot Slot) const override;
	virtual FText GetInventoryItemDisplayNameAt(EPlayerItemSlot Slot, int32 Index) const override;
	virtual int32 GetActiveInventoryItemIndex(EPlayerItemSlot Slot) const override;
	bool HasNearbyPickup() const;
	FText GetNearbyPickupDisplayName() const;
	FText GetNearbyPickupSlotLabel() const;
	bool IsPickupSwapHoldActive() const;
	float GetPickupSwapHoldProgress() const;

#if WITH_DEV_AUTOMATION_TESTS
	void TestTriggerNearbyPickupInput();
	void TestTriggerNearbyPickupHoldInput();
	void TestTriggerUseSupportItemInput();
	void TestTriggerUseUtilityItemInput();
	void TestTriggerPrimaryWeaponInput();
#endif

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnCharacterDeath() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Move(const FInputActionValue& Value) override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void ReloadWeapon() override;
	virtual void OnJumpPressed() override;
	virtual void OnJumpReleased() override;
	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SelectPrimaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SelectSecondaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> PickUpNearbyItemAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> UseSupportItemAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> UseUtilityItemAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverStateComponent> CoverStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UPlayerInventoryComponent> ItemComponent;

private:
	void InitializeActiveWeaponFromInventory();
	void UpdateNearbyPickup();
	void HandlePrimaryWeaponPressed();
	void HandlePrimaryWeaponReleased();
	void HandleSecondaryWeaponPressed();
	void HandleSecondaryWeaponReleased();
	void HandlePickUpNearbyItemPressed();
	void HandlePickUpNearbyItemReleased();
	void HandleUseSupportItem();
	void HandleUseUtilityItem();
	void HandleWeaponSlotPressed(EPlayerItemSlot Slot);
	void HandleWeaponSlotReleased(EPlayerItemSlot Slot);
	void OnWeaponSelectHoldTriggered();
	void OnPickupSwapHoldTriggered();

	void HandleWeaponSlotSelect(EPlayerItemSlot Slot);
	void HandleActiveWeaponFired(EPlayerItemSlot FiredSlot);
	void HandleInventoryStateChanged();

	UPROPERTY(EditDefaultsOnly, Category="Input|Weapons")
	float WeaponSelectionHoldTime = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	float NearbyPickupSearchRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	float PickupSwapHoldTime = 0.35f;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDrawPickupRadiusDebug = false;

	FTimerHandle WeaponSelectionHoldTimer;
	FTimerHandle PickupSwapHoldTimer;
	TWeakObjectPtr<UPlayerItemPickupComponent> NearbyPickupComponent;
	EPlayerItemSlot PendingWeaponSelectionSlot = EPlayerItemSlot::PrimaryWeapon;
	bool bWeaponSelectionButtonDown = false;
	bool bWeaponSelectionHoldTriggered = false;
	bool bPickupButtonDown = false;
	bool bPickupHoldTriggered = false;
	bool bIsVaulting = false;
};
