#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "ThirdPersonPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UEnhancedInputComponent;
class UPlayerAimComponent;
class UPlayerWeaponComponent;
class UPlayerLocomotionComponent;
class AWeaponBase;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing UMETA(DisplayName="Standing"),
	Crouching UMETA(DisplayName="Crouching"),
	Prone UMETA(DisplayName="Prone")
};

UCLASS()
class SNEAKGEAR_API AThirdPersonPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AThirdPersonPlayerCharacter();

	bool IsAiming() const;
	AWeaponBase* GetCurrentWeapon() const;
	float GetAmmo() const;
	float ConsumeAmmo(float Amount);
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetStance(EStance NewStance);

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EStance Stance = EStance::Standing;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo")
	float Ammo = 10000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerAimComponent> AimComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerLocomotionComponent> LocomotionComponent;

private:
	void SetupViewComponents();
	void SetupGameplayComponents();
	void InitializeGameplayState();
	void BindInputActions(UEnhancedInputComponent* EnhancedInput);

	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	void ApplyAimRotationMode(bool bEnableAimRotation);
	void ToggleAimView();
	void ToggleEquip();
	void ToggleSprint();
	void ReloadWeapon();
	
	void OnStancePressed();
	void OnStanceReleased();
};
