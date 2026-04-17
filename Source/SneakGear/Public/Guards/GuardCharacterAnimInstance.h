#pragma once

#include "CoreMinimal.h"
#include "Types/GuardAwarenessTypes.h"
#include "Characters/BaseCharacterAnimInstance.h"
#include "GuardCharacterAnimInstance.generated.h"

class AGuardCharacter;

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

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AGuardCharacter> GuardCharacter;
};
