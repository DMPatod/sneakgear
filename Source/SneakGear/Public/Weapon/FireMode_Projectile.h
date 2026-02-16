#pragma once

#include "CoreMinimal.h"
#include "WeaponFireModeComponent.h"
#include "FireMode_Projectile.generated.h"


UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UFireMode_Projectile : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Fire|Projectile")
	TSubclassOf<AActor> ProjectileClass;

	virtual void FireOnce(const FWeaponFireContext& Context) override;
};
