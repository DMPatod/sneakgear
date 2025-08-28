#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "MainPlayerCharacterBase.generated.h"

UCLASS(Abstract)
class SNEAKGEAR_API AMainPlayerCharacterBase : public ACharacter
{
public:
	AMainPlayerCharacterBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Player|Aim")
	void ToggleAim();

	UFUNCTION(BlueprintCallable, Category = "Player|Aim")
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable, Category = "Player|Aim")
	void SwitchToFirstPersonCamera();

	UFUNCTION(BlueprintCallable, Category = "Player|Aim")
	void SwitchToNormalCamera();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> CameraBoomNormal;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> CameraBoomAiming;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> ThirdPersonCameraNormal;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> ThirdPersonCameraAiming;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

private:
	bool IsAiming = false;

	void SetActiveNormalThirdPersonCamera();
	void SetActiveAimingThirdPersonCamera();
	void SetActiveFirstPersonCamera();

	GENERATED_BODY()
};
