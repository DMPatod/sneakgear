#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterWeaponComponent.generated.h"

class AWeaponBase;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UCharacterWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterWeaponComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	void StartFire();
	void StopFire();
	void ToggleEquip();

	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	virtual void Reload();

	AWeaponBase* GetCurrentWeapon() const
	{
		return CurrentWeapon;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	int32 GetInClip() const
	{
		return InClip;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon|Ammo")
	int32 GetClipSize() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeaponBase> StartedWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HandSocketName = "hand_r_socket";

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName HolsterSocketName = "spine_socket";

	int32 InClip = -1;

private:
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon;

	void AttachWeaponToSocket(FName SocketName) const;
	void HandleWeaponFired();
};
