#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/PlayerInventoryTypes.h"
#include "Types/PlayerMovementTypes.h"
#include "Types/PlayerUITypes.h"
#include "PlayerUIDataSource.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerUIVitalsChanged);
DECLARE_MULTICAST_DELEGATE(FOnPlayerUIWeaponStateChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerUIStanceChanged, EStance);

UINTERFACE(MinimalAPI)
class UPlayerUIDataSource : public UInterface
{
	GENERATED_BODY()
};

class SNEAKGEARCORE_API IPlayerUIDataSource
{
	GENERATED_BODY()

public:
	virtual bool GetPlayerVitalsViewData(FPlayerVitalsViewData& OutData) const = 0;
	virtual bool GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const = 0;
	virtual bool GetWeaponQuickSlotViewData(EPlayerItemSlot Slot, FWeaponQuickSlotViewData& OutData) const = 0;
	virtual bool GetStealthDebugViewData(FStealthDebugViewData& OutData) const = 0;
	virtual bool GetCurrentStanceForUI(EStance& OutStance) const = 0;
	virtual FText GetInventoryItemDisplayName(EPlayerItemSlot Slot) const = 0;
	virtual int32 GetInventoryItemCount(EPlayerItemSlot Slot) const = 0;
	virtual FText GetInventoryItemDisplayNameAt(EPlayerItemSlot Slot, int32 Index) const = 0;
	virtual int32 GetActiveInventoryItemIndex(EPlayerItemSlot Slot) const = 0;

	virtual FOnPlayerUIVitalsChanged& OnPlayerUIVitalsChangedEvent() = 0;
	virtual FOnPlayerUIWeaponStateChanged& OnPlayerUIWeaponStateChangedEvent() = 0;
	virtual FOnPlayerUIStanceChanged& OnPlayerUIStanceChangedEvent() = 0;
};
