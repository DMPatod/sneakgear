#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAimComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UPlayerTuningData;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAimComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(USpringArmComponent* InCameraBoom, UCameraComponent* InThirdPersonCamera,
	                UCameraComponent* InFirstPersonCamera, const UPlayerTuningData* InTuningData);

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

	UPROPERTY()
	TObjectPtr<const UPlayerTuningData> TuningData;

	bool bIsAiming = false;
	bool bAimFirstPerson = false;
	float CurrentTurnScalar = 1.f;

	void UpdateAim(float DeltaTime);
};
