#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterBase.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class UCharacterMovementComponent;
class APlayerCharacterBase;

UCLASS()
class SNEAKGEAR_API UPlayerAnimInstance : public UAnimInstance
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

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsProne = false;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsVaulting = false;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UAnimInstance> WeaponSetAnimationBP;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UAnimInstance> DefaultSetAnimationBP;

	UPROPERTY(Transient)
	TSubclassOf<UAnimInstance> LinkedWeaponSet;

	UPROPERTY(BlueprintReadOnly, Category="Stance")
	EStance Stance = EStance::Standing;

	UPROPERTY(BlueprintReadOnly, Category="Stance")
	float Speed2D = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	float AimPitch;

private:
	UPROPERTY(Transient)
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(Transient)
	TObjectPtr<APlayerCharacterBase> ThirdPersonCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
