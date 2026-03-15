#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ABaseCharacter;

UCLASS()
class SNEAKGEAR_API UBaseCharacterAnimInstance : public UAnimInstance
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

	UPROPERTY(BlueprintReadOnly, Category="Stance")
	float Speed2D = 0.f;

protected:
	ABaseCharacter* GetBaseCharacter() const
	{
		return BaseCharacter;
	}

	UCharacterMovementComponent* GetCharacterMovementComponent() const
	{
		return MovementComponent;
	}

	UPROPERTY(Transient)
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
