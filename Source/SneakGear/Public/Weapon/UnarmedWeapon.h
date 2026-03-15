#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "UnarmedWeapon.generated.h"

UCLASS()
class SNEAKGEAR_API AUnarmedWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AUnarmedWeapon();

	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
};
