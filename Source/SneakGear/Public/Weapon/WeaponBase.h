#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/PlayerInventoryTypes.h"
#include "WeaponBase.generated.h"

class UWeaponFireModeComponent;
class UAnimInstance;
class UAnimMontage;
struct FWeaponFireContext;
DECLARE_MULTICAST_DELEGATE(FOnWeaponFired);
DECLARE_MULTICAST_DELEGATE(FOnWeaponReloaded);

UCLASS(Abstract)
class SNEAKGEAR_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	virtual void Tick(float DeltaSeconds) override;
	virtual void StartFire();
	virtual void StopFire();
	virtual float Reload();

	FOnWeaponFired& OnWeaponFiredEvent()
	{
		return OnWeaponFired;
	}

	FOnWeaponReloaded& OnWeaponReloadedEvent()
	{
		return OnWeaponReloaded;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon")
	float GetFireRate() const
	{
		const float FireInterval = GetFireInterval();
		return FireInterval > 0.f ? 1.f / FireInterval : 0.f;
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

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSubclassOf<UAnimInstance> AnimationSetBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Ammo")
	EAmmoType AmmoType = EAmmoType::None;

protected:
	FTimerHandle FireTimer;

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float FireRate = 10.f;

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
	void FireOnce();
	void HandleReloadFinished();
	virtual float GetFireInterval() const;
	virtual float PlayFireAnimation() const;
	virtual float PlayReloadAnimation() const;
	void FireAndScheduleNextShot();
	void ScheduleReloadCompletion(float Duration);

private:
	FOnWeaponFired OnWeaponFired;
	FOnWeaponReloaded OnWeaponReloaded;
	bool bWantsToFire = false;
	bool bReloadPending = false;
	float NextFireTimeSeconds = -1.f;
	float ReloadCompleteTimeSeconds = -1.f;

	UPROPERTY(EditDefaultsOnly, Category="Details")
	FName Name;

};
