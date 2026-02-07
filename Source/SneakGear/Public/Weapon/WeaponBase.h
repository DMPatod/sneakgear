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
};
