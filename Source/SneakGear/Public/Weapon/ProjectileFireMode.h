#pragma once

#include "CoreMinimal.h"
#include "WeaponFireModeComponent.h"
#include "ProjectileFireMode.generated.h"


UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UProjectileFireMode : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Fire|Projectile")
	TSubclassOf<AActor> ProjectileClass;

	virtual void FireOnce(const FWeaponFireContext& Context) override;
};
