#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Components/PlayerItemComponent.h"
#include "Components/Cover/CoverStateComponent.h"
#include "StealthPlayerCharacter.generated.h"

class UCoverComponent;
class UCoverStateComponent;
class UPlayerItemComponent;
class AWorldItemPickup;
class AWeaponBase;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API AStealthPlayerCharacter : public AThirdPersonPlayerCharacter
{
	GENERATED_BODY()

public:
	AStealthPlayerCharacter();

	bool IsInCover() const
	{
		return CoverStateComponent ? CoverStateComponent->IsInCover() : false;
	}

	float GetCoverMoveAxis() const
	{
		return CoverStateComponent ? CoverStateComponent->GetCoverMoveAxis() : 0.f;
	}

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryPickUpFloorItem(AWorldItemPickup* PickupActor, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryPickUpNearbyFloorItem(float SearchRadius = 150.f, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	bool SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand = true);

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	EPlayerItemSlot GetActiveWeaponSlot() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	bool WasWeaponFiredRecently(float WindowSeconds = 0.12f) const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetActiveWeaponInClip() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetActiveWeaponClipSize() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetReserveAmmoCount() const;

	virtual AWeaponBase* GetCurrentWeapon() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Move(const FInputActionValue& Value) override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void ReloadWeapon() override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SelectPrimaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SelectSecondaryWeaponAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverStateComponent> CoverStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UPlayerItemComponent> ItemComponent;

private:
	void HandlePrimaryWeaponSelected();
	void HandleSecondaryWeaponSelected();
	void HandleWeaponSlotSelect(EPlayerItemSlot Slot);
	void HandleActiveWeaponFired(EPlayerItemSlot FiredSlot);

	UPROPERTY(Transient)
	float LastWeaponFireTimestamp = -1000.f;
};
