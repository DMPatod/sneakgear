#pragma once

#include "CoreMinimal.h"
#include "Guards/GuardCharacter.h"
#include "DroneGuardCharacter.generated.h"

class UCharacterMovementComponent;

UCLASS()
class SNEAKGEAR_API ADroneGuardCharacter : public AGuardCharacter
{
	GENERATED_BODY()

public:
	ADroneGuardCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Movement", meta=(ClampMin="0.0"))
	float FlyingMaxSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Movement", meta=(ClampMin="0.0"))
	float FlyingAcceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Movement", meta=(ClampMin="0.0"))
	float FlyingBrakingDeceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Hover")
	bool bEnableHoverOffset = true;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Hover", meta=(ClampMin="0.0"))
	float HoverAmplitude = 12.f;

	UPROPERTY(EditDefaultsOnly, Category="Drone|Hover", meta=(ClampMin="0.0"))
	float HoverFrequency = 1.5f;

private:
	void ConfigureFlyingMovement();

	float HoverTime = 0.f;
	FVector BaseHoverLocation = FVector::ZeroVector;
};
