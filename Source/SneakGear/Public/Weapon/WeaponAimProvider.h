#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponAimProvider.generated.h"

UINTERFACE()
class SNEAKGEAR_API UWeaponAimProvider : public UInterface
{
	GENERATED_BODY()
};

class SNEAKGEAR_API IWeaponAimProvider
{
	GENERATED_BODY()

public:
	virtual bool GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const = 0;
};
