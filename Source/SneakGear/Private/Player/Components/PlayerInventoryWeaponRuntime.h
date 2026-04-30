#pragma once

#include "CoreMinimal.h"
#include "Types/PlayerInventoryTypes.h"

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
	static bool SetActiveWeaponSlot(UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot, bool bEquipInHand);
	static bool SetWeaponEquipped(UPlayerInventoryComponent& Inventory, bool bNewEquipped);
	static AWeaponBase* GetWeaponInSlot(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
	static AWeaponBase* GetActiveWeapon(const UPlayerInventoryComponent& Inventory);
	static EPlayerInventoryWeaponState GetWeaponState(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
	static EPlayerInventoryWeaponState GetActiveWeaponState(const UPlayerInventoryComponent& Inventory);
	static void StartFire(UPlayerInventoryComponent& Inventory);
	static void StopFire(UPlayerInventoryComponent& Inventory);
	static bool Reload(UPlayerInventoryComponent& Inventory);
	static bool NotifyFireAnimation(UPlayerInventoryComponent& Inventory);
	static bool NotifyReloadAnimationFinished(UPlayerInventoryComponent& Inventory);
	static int32 GetInClip(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
	static int32 GetClipSize(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
	static int32 GetActiveWeaponInClip(const UPlayerInventoryComponent& Inventory);
	static int32 GetActiveWeaponClipSize(const UPlayerInventoryComponent& Inventory);
	static bool IsActiveWeaponReloading(const UPlayerInventoryComponent& Inventory);
	static bool WasFireRequestedRecently(const UPlayerInventoryComponent& Inventory, float WindowSeconds);
	static bool IsFireNotifyPending(const UPlayerInventoryComponent& Inventory);
	static bool WasFiredRecently(const UPlayerInventoryComponent& Inventory, float WindowSeconds);
	static void HandleFireRequested(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);
	static void HandleFired(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);
	static void HandleReloaded(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);

private:
	static AWeaponBase* SpawnWeapon(const UPlayerInventoryComponent& Inventory, TSubclassOf<AWeaponBase> WeaponClass);
	static void ClearRuntimeWeapon(UPlayerInventoryComponent& Inventory, FPlayerInventoryWeaponSlotRuntime& Runtime);
	static void BindRuntimeWeaponDelegates(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot, AWeaponBase* WeaponActor);
	static void AttachWeapon(const UPlayerInventoryComponent& Inventory, AWeaponBase* Weapon, FName SocketName, bool bUseHolsterOffset);
	static FName GetHolsterSocketForSlot(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot WeaponSlot);
	static bool IsReloading(const FPlayerInventoryWeaponSlotRuntime& Runtime);
	static void SetState(UPlayerInventoryComponent& Inventory, FPlayerInventoryWeaponSlotRuntime& Runtime,
		EPlayerInventoryWeaponState NewState, bool bBroadcastChange = true);
	static bool HasValidWeaponItem(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);
	static bool HasValidWeaponSelection(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);
	static EAmmoType GetAmmoTypeForSlot(const UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot);
};
