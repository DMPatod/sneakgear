#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/PlayerInventoryTypes.h"
#include "WeaponMenuActions.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponMenuActions : public UInterface
{
	GENERATED_BODY()
};

class SNEAKGEARCORE_API IWeaponMenuActions
{
	GENERATED_BODY()

public:
	virtual void SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot) = 0;
	virtual void CancelWeaponSelectionMenu() = 0;
};
