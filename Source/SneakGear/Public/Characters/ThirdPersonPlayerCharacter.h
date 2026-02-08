#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Components/PlayerAimComponent.h"
#include "Components/PlayerWeaponComponent.h"
#include "ThirdPersonPlayerCharacter.generated.h"

class UStaminaAttributeSet;
class UHealthAttributeSet;
class UAbilitySystemComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UPlayerAimComponent;
class UPlayerWeaponComponent;
class UPlayerTuningData;
class AWeaponBase;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API AThirdPersonPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AThirdPersonPlayerCharacter();

	bool IsAiming() const
	{
		return AimComponent ? AimComponent->IsAiming() : false;
	}

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	AWeaponBase* GetCurrentWeapon() const
	{
		return WeaponComponent ? WeaponComponent->GetCurrentWeapon() : nullptr;
	}

	const UHealthAttributeSet* GetHealthSet() const
	{
		return HealthSet;
	}

	const UStaminaAttributeSet* GetStaminaSet() const
	{
		return StaminaSet;
	}

protected:
	virtual void BeginPlay() override;
	void InitGAS();
	void BindHealthDeath();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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

	UPROPERTY(EditDefaultsOnly, Category="Tuning")
	TObjectPtr<UPlayerTuningData> TuningData;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerTuningData> DefaultTuningData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerAimComponent> AimComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY()
	TObjectPtr<UStaminaAttributeSet> StaminaSet;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

private:
	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	void ToggleAimView();
	void ToggleEquip();
};
