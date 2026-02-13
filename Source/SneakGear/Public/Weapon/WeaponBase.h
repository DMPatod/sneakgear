#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UWeaponFireModeComponent;

UCLASS(Abstract)
class SNEAKGEAR_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	void AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName);

	virtual void StartFire();
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	float GetFireRate() const
	{
		return FireRate;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon")
	FName GetMuzzleSocketName() const
	{
		return MuzzleSocketName;
	}

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSubclassOf<UAnimInstance> AnimationSetBP;

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

	virtual void BeginPlay() override;

	void FireOnce();

private:
	UPROPERTY(EditDefaultsOnly, Category="Details")
	FName Name;

	UPROPERTY(EditDefaultsOnly, Category="Details")
	int8 ClipSize = 8;
};
