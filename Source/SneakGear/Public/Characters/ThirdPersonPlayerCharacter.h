#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Weapon/WeaponBase.h"
#include "ThirdPersonPlayerCharacter.generated.h"

class UStaminaAttributeSet;
class UHealthAttributeSet;
class UAbilitySystemComponent;
class AWeaponBase;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;

UCLASS()
class SNEAKGEAR_API AThirdPersonPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AThirdPersonPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	bool IsAiming() const
	{
		return bIsAiming;
	}

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

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

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FName FirstPersonCameraSocket = "first_person_camera_attachment";

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> StartedWeaponClass;

	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HandSocketName = "hand_r_socket";

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HolsterSocketName = "spine_socket";

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	bool bAimFirstPerson = false;

	UPROPERTY(EditAnywhere, Category="Aim")
	float NormalFOV = 90.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	float AimFOV_FirstPerson = 60.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	float AimFOV_ThirdPerson = 72.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	float AimInterpolationSpeed = 18.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	float NormalTurnRate = 1.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	float AimTurnRate = 1.f;

	UPROPERTY(EditAnywhere, Category="Aim")
	FVector OverTheShouldOffset_Normal = FVector(0.f, 50.f, 75.f);

	UPROPERTY(EditAnywhere, Category="Aim")
	FVector OverTheShouldOffset_Aim = FVector(0.f, 70.f, 60.f);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> AimViewToggleAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY()
	TObjectPtr<UStaminaAttributeSet> StaminaSet;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

private:
	void StartFire()
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}

	void StopFire()
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}

	void StartAim();
	void StopAim();
	void ToggleAimView();
	void ToggleEquip();

	float CurrentTurnScalar = 1.f;
};
