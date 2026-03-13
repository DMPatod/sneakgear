#pragma once

#include "CoreMinimal.h"
#include "WeaponFireModeComponent.h"
#include "HitscanFireMode.generated.h"


UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UHitscanFireMode : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Fire|Hitscan")
	float Damage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Hitscan")
	TSubclassOf<UDamageType> DamageType;

	virtual void FireOnce(const FWeaponFireContext& Context) override;
};
