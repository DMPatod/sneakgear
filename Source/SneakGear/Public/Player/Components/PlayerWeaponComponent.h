#pragma once

#include "CoreMinimal.h"
#include "Components/CharacterWeaponComponent.h"
#include "PlayerWeaponComponent.generated.h"

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerWeaponComponent : public UCharacterWeaponComponent
{
	GENERATED_BODY()

public:
	UPlayerWeaponComponent();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	float GetPlayerArmor() const;

protected:
	virtual bool CanReload() const override;
	virtual void FinishReload() override;
};
