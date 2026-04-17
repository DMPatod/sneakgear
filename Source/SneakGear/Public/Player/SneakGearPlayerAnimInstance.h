#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacterAnimInstance.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Types/PlayerMovementTypes.h"
#include "SneakGearPlayerAnimInstance.generated.h"

class ASneakGearPlayerCharacter;

UCLASS()
class SNEAKGEAR_API USneakGearPlayerAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsRunning = false;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bIsProne = false;

	UPROPERTY(BlueprintReadOnly, Category="Cover")
	bool bInCover = false;

	UPROPERTY(BlueprintReadOnly, Category="Cover")
	float CoverMove = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	bool bWeaponFiredRecently = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	EPlayerItemSlot ActiveWeaponSlot = EPlayerItemSlot::PrimaryWeapon;

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

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	float AimPitch = 0.f;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<ASneakGearPlayerCharacter> StealthCharacter;
};
