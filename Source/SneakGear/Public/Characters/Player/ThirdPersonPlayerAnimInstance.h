#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ThirdPersonPlayerAnimInstance.generated.h"

class UCharacterMovementComponent;
class AThirdPersonPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UThirdPersonPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	float Direction = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsRunning = false;

private:
	UPROPERTY(Transient)
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(Transient)
	TObjectPtr<AThirdPersonPlayerCharacter> ThirdPersonCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
