#pragma once

#include "CoreMinimal.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/PlayerAnimInstance.h"
#include "StealthPlayerAnimInstance.generated.h"

class ASneakGearPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UStealthPlayerAnimInstance : public UPlayerAnimInstance
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
	TObjectPtr<ASneakGearPlayerCharacter> StealthCharacter;
};
