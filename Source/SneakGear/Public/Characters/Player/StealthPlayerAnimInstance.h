#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerItemComponent.h"
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

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	bool bWeaponFiredRecently = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	EPlayerItemSlot ActiveWeaponSlot = EPlayerItemSlot::PrimaryWeapon;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AStealthPlayerCharacter> StealthCharacter;
};
