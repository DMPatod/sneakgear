#pragma once

#include "CoreMinimal.h"

class AWeaponBase;
class UPlayerInventoryComponent;
struct FPlayerInventoryWeaponSlotRuntime;
enum class EPlayerItemSlot : uint8;

class FPlayerInventoryWeaponRuntime
{
public:
	static void Initialize(UPlayerInventoryComponent& Inventory);
	static void Shutdown(UPlayerInventoryComponent& Inventory);
	static bool SetWeaponClassForSlot(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot, TSubclassOf<AWeaponBase> WeaponClass);
	static void SyncAttachments(const UPlayerInventoryComponent& Inventory);

private:
	static AWeaponBase* SpawnWeapon(const UPlayerInventoryComponent& Inventory, TSubclassOf<AWeaponBase> WeaponClass);
	static void ClearRuntimeWeapon(UPlayerInventoryComponent& Inventory, FPlayerInventoryWeaponSlotRuntime& Runtime);
	static void BindRuntimeWeaponDelegates(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot, AWeaponBase* WeaponActor);
	static void AttachWeapon(const UPlayerInventoryComponent& Inventory, AWeaponBase* Weapon, FName SocketName, bool bUseHolsterOffset);
	static FName GetHolsterSocketForSlot(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
};
