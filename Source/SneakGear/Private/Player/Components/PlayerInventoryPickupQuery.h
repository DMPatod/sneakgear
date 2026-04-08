#pragma once

#include "CoreMinimal.h"

class AActor;
class UPlayerInventoryComponent;
class UPlayerItemPickupComponent;

class FPlayerInventoryPickupQuery
{
public:
	static AActor* FindBestNearbyFloorPickup(const UPlayerInventoryComponent& Inventory, float SearchRadius);
	static bool PickupRequiresWeaponSwap(const UPlayerInventoryComponent& Inventory, const UPlayerItemPickupComponent* PickupComponent);
};
