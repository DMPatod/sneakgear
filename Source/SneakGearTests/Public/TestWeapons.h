#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitscanWeaponBase.h"
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

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEARTESTS_API UTestDamageWeaponFireModeComponent : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	virtual void FireOnce(const FWeaponFireContext& Context) override;

	UPROPERTY(EditAnywhere, Category="Fire")
	float Damage = 25.f;
};

UCLASS()
class SNEAKGEARTESTS_API ATestWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ATestWeapon();

	UTestWeaponFireModeComponent* GetTestFireMode() const;
};

UCLASS()
class SNEAKGEARTESTS_API ATestDelayedReloadWeapon : public ATestWeapon
{
	GENERATED_BODY()

public:
	ATestDelayedReloadWeapon();
};

UCLASS()
class SNEAKGEARTESTS_API ATestDamageWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ATestDamageWeapon();
};

UCLASS()
class SNEAKGEARTESTS_API ATestHitscanWeapon : public AHitscanWeaponBase
{
	GENERATED_BODY()

public:
	ATestHitscanWeapon();

	UTestWeaponFireModeComponent* GetTestFireMode() const;
};
