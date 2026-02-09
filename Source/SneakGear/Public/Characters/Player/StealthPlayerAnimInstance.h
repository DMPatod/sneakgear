#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/ThirdPersonPlayerAnimInstance.h"
#include "StealthPlayerAnimInstance.generated.h"

class AStealthPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UStealthPlayerAnimInstance : public UThirdPersonPlayerAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Cover")
	bool bInCover = false;

	UPROPERTY(BlueprintReadOnly, Category="Cover")
	float CoverMove = 0.f;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AStealthPlayerCharacter> StealthCharacter;
};
