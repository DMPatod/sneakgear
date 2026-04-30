#pragma once

#include "CoreMinimal.h"
#include "Types/PlayerInventoryTypes.h"

class AWeaponBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponFireRequested, EPlayerItemSlot);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponFired, EPlayerItemSlot);
DECLARE_MULTICAST_DELEGATE(FOnInventoryStateChanged);

struct FPlayerInventoryWeaponSlotRuntime
{
	TObjectPtr<AWeaponBase> WeaponActor = nullptr;
	int32 InClip = -1;
	EPlayerInventoryWeaponState State = EPlayerInventoryWeaponState::Idle;
	bool bTriggerHeld = false;
};
