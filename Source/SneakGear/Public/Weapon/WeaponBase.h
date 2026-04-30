#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/PlayerInventoryTypes.h"
#include "WeaponBase.generated.h"

class UWeaponFireModeComponent;
class UAnimInstance;
struct FWeaponFireContext;
DECLARE_MULTICAST_DELEGATE(FOnWeaponFireRequested);
DECLARE_MULTICAST_DELEGATE(FOnWeaponFired);
DECLARE_MULTICAST_DELEGATE(FOnWeaponReloaded);

UCLASS(Abstract)
class SNEAKGEAR_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	virtual void StartFire();
	virtual void StopFire();
	virtual float Reload();

	FOnWeaponFireRequested& OnWeaponFireRequestedEvent()
	{
		return OnWeaponFireRequested;
	}

	FOnWeaponFired& OnWeaponFiredEvent()
	{
		return OnWeaponFired;
	}

	FOnWeaponReloaded& OnWeaponReloadedEvent()
	{
		return OnWeaponReloaded;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon")
	FName GetMuzzleSocketName() const
	{
		return MuzzleSocketName;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon")
	EAmmoType GetAmmoType() const
	{
		return AmmoType;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon")
	int32 GetClipSize() const
	{
		return ClipSize;
	}

	FTransform GetAttachmentOffset(bool bUseHolsterOffset) const
	{
		return bUseHolsterOffset ? HolsterOffset : GripOffset;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon|Animation")
	bool NotifyFireAnimation();

	UFUNCTION(BlueprintCallable, Category="Weapon|Animation")
	bool NotifyReloadAnimationFinished();

	UFUNCTION(BlueprintPure, Category="Weapon|Animation")
	bool IsFireNotifyPending() const
	{
		return bFireNotifyPending;
	}

	UFUNCTION(BlueprintPure, Category="Weapon|Animation")
	bool IsReloadNotifyPending() const
	{
		return bReloadPending;
	}

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSubclassOf<UAnimInstance> AnimationSetBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	EAmmoType AmmoType = EAmmoType::None;

protected:
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<UWeaponFireModeComponent> PrimaryFireModeClass;

	UPROPERTY(Transient)
	TObjectPtr<UWeaponFireModeComponent> PrimaryFireMode;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName MuzzleSocketName = "muzzle_socket_name";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attachment")
	FTransform GripOffset = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attachment")
	FTransform HolsterOffset = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, Category="Details")
	int32 ClipSize = 8;

	virtual void BeginPlay() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	bool BuildFireContext(FWeaponFireContext& OutContext) const;
	virtual void FireOnce();
	void HandleReloadFinished();
	void RequestFireAnimationNotify();

private:
	FOnWeaponFireRequested OnWeaponFireRequested;
	FOnWeaponFired OnWeaponFired;
	FOnWeaponReloaded OnWeaponReloaded;
	bool bWantsToFire = false;
	bool bFireNotifyPending = false;
	bool bReloadPending = false;

	UPROPERTY(EditDefaultsOnly, Category="Details")
	FName Name;

};
