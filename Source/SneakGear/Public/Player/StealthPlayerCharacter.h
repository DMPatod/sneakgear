#pragma once

#include "CoreMinimal.h"
#include "Player/ThirdPersonPlayerCharacter.h"
#include "Player/Components/PlayerItemComponent.h"
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
	UPlayerItemComponent* GetItemComponent() const
	{
		return ItemComponent;
	}

	UFUNCTION(BlueprintPure, Category="Cover")
	bool IsVaulting() const
	{
		return bIsVaulting;
	}

	virtual AWeaponBase* GetCurrentWeapon() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnCharacterDeath() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverStateComponent> CoverStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UPlayerItemComponent> ItemComponent;

private:
	void InitializeActiveWeaponFromInventory();
	void HandlePrimaryWeaponPressed();
	void HandlePrimaryWeaponReleased();
	void HandleSecondaryWeaponPressed();
	void HandleSecondaryWeaponReleased();
	void HandleWeaponSlotPressed(EPlayerItemSlot Slot);
	void HandleWeaponSlotReleased(EPlayerItemSlot Slot);
	void OnWeaponSelectHoldTriggered();

	void HandleWeaponSlotSelect(EPlayerItemSlot Slot);
	void HandleActiveWeaponFired(EPlayerItemSlot FiredSlot);

	UPROPERTY(EditDefaultsOnly, Category="Input|Weapons")
	float WeaponSelectionHoldTime = 0.35f;

	FTimerHandle WeaponSelectionHoldTimer;
	EPlayerItemSlot PendingWeaponSelectionSlot = EPlayerItemSlot::PrimaryWeapon;
	bool bWeaponSelectionButtonDown = false;
	bool bWeaponSelectionHoldTriggered = false;
	bool bIsVaulting = false;
};
