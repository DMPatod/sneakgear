#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Components/PlayerInventoryComponentTypes.h"
#include "Types/PlayerInventoryTypes.h"
#include "PlayerWeaponComponent.generated.h"

class AWeaponBase;
class UPlayerInventoryComponent;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerWeaponComponent();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	bool SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand = true);

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	bool SetWeaponEquipped(bool bNewEquipped);

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	bool IsWeaponEquipped() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	EPlayerItemSlot GetActiveWeaponSlot() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	AWeaponBase* GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	AWeaponBase* GetActiveWeapon() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	EPlayerInventoryWeaponState GetWeaponState(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	EPlayerInventoryWeaponState GetActiveWeaponState() const;

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	void StartActiveWeaponFire();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	void StopActiveWeaponFire();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	bool ReloadActiveWeapon();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	bool NotifyActiveWeaponFireAnimation();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapons")
	bool NotifyActiveWeaponReloadAnimationFinished();

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	int32 GetInClip(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	int32 GetClipSize(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	int32 GetActiveWeaponInClip() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	int32 GetActiveWeaponClipSize() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	bool WasActiveWeaponFireRequestedRecently(float WindowSeconds = 0.12f) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	bool IsActiveWeaponFireNotifyPending() const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	bool WasActiveWeaponFiredRecently(float WindowSeconds = 0.12f) const;

	UFUNCTION(BlueprintPure, Category="Combat|Weapons")
	bool IsActiveWeaponReloading() const;

	FOnActiveWeaponFireRequested& OnActiveWeaponFireRequestedEvent();
	FOnActiveWeaponFired& OnActiveWeaponFiredEvent();

private:
	UPlayerInventoryComponent* GetInventoryComponent() const;
};
