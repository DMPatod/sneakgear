#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "HitscanWeaponBase.generated.h"

UCLASS()
class SNEAKGEAR_API AHitscanWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

public:
	AHitscanWeaponBase();

	virtual void Tick(float DeltaSeconds) override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual float Reload() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

protected:
	void FireOnce();
	void ApplyRecoil() const;
	FVector BuildShotDirection(const FVector& MuzzleLocation, const FVector& AimPoint) const;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Damage")
	float BaseDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Damage", meta=(ClampMin="0.0"))
	float Range = 8000.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Penetration", meta=(ClampMin="0", ClampMax="8"))
	int32 MaxPenetrations = 2;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Penetration", meta=(ClampMin="0.05", ClampMax="1.0"))
	float PenetrationDamageMultiplier = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Penetration", meta=(ClampMin="1.0"))
	float PenetrationAdvanceDistance = 12.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Penetration", meta=(ClampMin="0.0"))
	float MinimumDamageAfterPenetration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
	float MinSpreadDegrees = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
	float MaxSpreadDegrees = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
	float SpreadIncreasePerShot = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
	float SpreadRecoveryPerSecond = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Recoil", meta=(ClampMin="0.0"))
	float RecoilPitchMin = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Recoil", meta=(ClampMin="0.0"))
	float RecoilPitchMax = 1.4f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Recoil")
	float RecoilYawMin = -0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Fire|Recoil")
	float RecoilYawMax = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDrawDebug = true;

	UPROPERTY(Transient)
	float CurrentSpreadDegrees = 0.f;
};
