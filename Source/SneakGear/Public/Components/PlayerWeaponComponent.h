#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWeaponComponent.generated.h"

class AWeaponBase;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerWeaponComponent();

	virtual void BeginPlay() override;

	void StartFire();
	void StopFire();
	void ToggleEquip();

	AWeaponBase* GetCurrentWeapon() const
	{
		return CurrentWeapon;
	}

protected:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> StartedWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HandSocketName = "hand_r_socket";

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HolsterSocketName = "spine_socket";

private:
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon;

	void AttachWeaponToSocket(FName SocketName) const;
};
