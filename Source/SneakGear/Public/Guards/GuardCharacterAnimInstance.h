#pragma once

#include "CoreMinimal.h"
#include "Types/GuardAwarenessTypes.h"
#include "Characters/BaseCharacterAnimInstance.h"
#include "GuardCharacterAnimInstance.generated.h"

class AGuardCharacter;
class UGuardAwarenessComponent;

UCLASS()
class SNEAKGEAR_API UGuardCharacterAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Guard")
	float Awareness = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Guard")
	bool bHasLineOfSight = false;

	UPROPERTY(BlueprintReadOnly, Category="Guard")
	bool bIsSuspicious = false;

	UPROPERTY(BlueprintReadOnly, Category="Guard")
	bool bIsAlerted = false;

	UPROPERTY(BlueprintReadOnly, Category="Guard")
	EGuardAwarenessState AwarenessState = EGuardAwarenessState::Calm;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	bool bWeaponFireRequestedRecently = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	bool bWeaponFirePending = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	bool bIsReloading = false;

	UFUNCTION(BlueprintCallable, Category="Weapon|Animation")
	bool NotifyWeaponFireAnimation();

	UFUNCTION(BlueprintCallable, Category="Weapon|Animation")
	bool NotifyWeaponReloadAnimationFinished();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AGuardCharacter> GuardCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UGuardAwarenessComponent> AwarenessComponent;
};
