#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/WeaponFireModeComponent.h"
#include "TestWeapons.generated.h"

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEARTESTS_API UTestWeaponFireModeComponent : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	virtual void FireOnce(const FWeaponFireContext& Context) override;

	int32 FireCount = 0;
	FWeaponFireContext LastContext;
};

UCLASS()
class SNEAKGEARTESTS_API ATestWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ATestWeapon();

	UTestWeaponFireModeComponent* GetTestFireMode() const;
};
