#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAimComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAimComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(USpringArmComponent* InCameraBoom, UCameraComponent* InThirdPersonCamera,
	                UCameraComponent* InFirstPersonCamera);

	void StartAim();
	void StopAim();
	void ToggleAimView();

	bool IsAiming() const
	{
		return bIsAiming;
	}

	float GetCurrentTurnScalar() const
	{
		return CurrentTurnScalar;
	}

private:
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY()
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float NormalFOV = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimFOVFirstPerson = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimFOVThirdPerson = 72.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimInterpolationSpeed = 18.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float NormalTurnRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimTurnRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	FVector OverTheShoulderOffsetNormal = FVector(0.f, 50.f, 75.f);

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	FVector OverTheShoulderOffsetAim = FVector(75, 55, 25);

	bool bIsAiming = false;
	bool bAimFirstPerson = false;
	float CurrentTurnScalar = 1.f;

	void UpdateAim(float DeltaTime);
};
