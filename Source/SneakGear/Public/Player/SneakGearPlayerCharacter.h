#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerCharacterBase.h"
#include "SneakGearPlayerCharacter.generated.h"

class UCoverComponent;
class UCoverStateComponent;
class UPlayerInventoryInteractionComponent;
class UPlayerInventoryComponent;
class UPlayerWeaponComponent;
class AWeaponBase;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API ASneakGearPlayerCharacter : public APlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASneakGearPlayerCharacter();

	bool IsInCover() const;
	float GetCoverMoveAxis() const;
	bool IsVaultAvailable() const;
	float GetCoverObstacleHeight() const;
	float GetCurrentVaultMaxObstacleHeight() const;
	UCoverStateComponent* GetCoverStateComponent() const { return CoverStateComponent; }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	UPlayerInventoryComponent* GetItemComponent() const
	{
		return ItemComponent;
	}

	UFUNCTION(BlueprintCallable, Category="Combat")
	UPlayerWeaponComponent* GetWeaponComponent() const
	{
		return WeaponComponent;
	}

	UFUNCTION(BlueprintPure, Category="Cover")
	bool IsVaulting() const;

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
	void TestTriggerJumpInput();
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
	virtual void ToggleEquip() override;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UPlayerInventoryInteractionComponent> InventoryInteractionComponent;

private:
	void HandleActiveWeaponFired(EPlayerItemSlot FiredSlot);
	void HandleInventoryStateChanged();

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDrawPickupRadiusDebug = false;
};
