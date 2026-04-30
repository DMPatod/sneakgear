#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "GameplayEffectTypes.h"
#include "UI/PlayerUIDataSource.h"
#include "Types/PlayerMovementTypes.h"
#include "Weapon/WeaponAimProvider.h"
#include "PlayerCharacterBase.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UEnhancedInputComponent;
class UGameplayEffect;
class UAmmoAttributeSet;
class UPlayerAimComponent;
class UPlayerLocomotionComponent;
class UStaminaAttributeSet;
class AWeaponBase;
class UUserWidget;
class UMaterialInterface;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API APlayerCharacterBase : public ABaseCharacter, public IWeaponAimProvider, public IPlayerUIDataSource
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

	bool IsAiming() const;
	virtual AWeaponBase* GetCurrentWeapon() const;
	virtual bool GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const override;
	float GetAmmo() const;
	const UStaminaAttributeSet* GetStaminaSet() const
	{
		return StaminaSet;
	}

	const UAmmoAttributeSet* GetAmmoSet() const
	{
		return AmmoSet;
	}

	float ConsumeAmmo(float Amount);
	bool ApplyHealthDelta(float DeltaHealth);
	FActiveGameplayEffectHandle ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass);
	bool RemoveGameplayEffectFromSelf(FActiveGameplayEffectHandle EffectHandle);
	void SetCameraPostProcessMaterialEnabled(UMaterialInterface* Material, bool bEnabled, float Weight = 1.f);
	void SetHUDOverlayWidgetVisible(TSubclassOf<UUserWidget> WidgetClass, bool bVisible);
	float GetMaxSpeed() const;
	virtual bool GetPlayerVitalsViewData(FPlayerVitalsViewData& OutData) const override;
	virtual bool GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const override;
	virtual bool GetWeaponQuickSlotViewData(EPlayerItemSlot Slot, FWeaponQuickSlotViewData& OutData) const override;
	virtual bool GetStealthDebugViewData(FStealthDebugViewData& OutData) const override;
	virtual bool GetCurrentStanceForUI(EStance& OutStance) const override;
	virtual FText GetInventoryItemDisplayName(EPlayerItemSlot Slot) const override;
	virtual int32 GetInventoryItemCount(EPlayerItemSlot Slot) const override;
	virtual FText GetInventoryItemDisplayNameAt(EPlayerItemSlot Slot, int32 Index) const override;
	virtual int32 GetActiveInventoryItemIndex(EPlayerItemSlot Slot) const override;
	virtual FOnPlayerUIVitalsChanged& OnPlayerUIVitalsChangedEvent() override;
	virtual FOnPlayerUIWeaponStateChanged& OnPlayerUIWeaponStateChangedEvent() override;
	virtual FOnPlayerUIStanceChanged& OnPlayerUIStanceChangedEvent() override;

	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetStance(EStance NewStance);

	FOnPlayerUIStanceChanged& OnStanceChangedEvent()
	{
		return OnStanceChanged;
	}

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EStance Stance = EStance::Standing;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitGAS() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void StartFire();
	virtual void StopFire();
	virtual void ReloadWeapon();
	virtual void ToggleEquip();
	virtual void OnJumpPressed();
	virtual void OnJumpReleased();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> AimViewToggleAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SprintToggleAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> StanceAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerAimComponent> AimComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerLocomotionComponent> LocomotionComponent;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UStaminaAttributeSet> StaminaSet;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAmmoAttributeSet> AmmoSet;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultStamina;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultAmmo;

	FOnPlayerUIStanceChanged OnStanceChanged;
	FOnPlayerUIVitalsChanged OnPlayerUIVitalsChanged;
	FOnPlayerUIWeaponStateChanged OnPlayerUIWeaponStateChanged;

private:
	void SetupViewComponents();
	void SetupGameplayComponents();
	void InitializeGameplayState();
	void BindInputActions(UEnhancedInputComponent* EnhancedInput);
	void BindUIDataDelegates();
	void UnbindUIDataDelegates();
	void HandleHealthAttributeChanged(const struct FOnAttributeChangeData& Data);
	void HandleStaminaAttributeChanged(const struct FOnAttributeChangeData& Data);
	void HandleAmmoAttributeChanged(const struct FOnAttributeChangeData& Data);

	void StartAim();
	void StopAim();
	void ApplyAimRotationMode(bool bEnableAimRotation);
	void ToggleAimView();
	void ToggleSprint();
	
	void OnStancePressed();
	void OnStanceReleased();
	bool bHasBoundUIDataDelegates = false;
};
