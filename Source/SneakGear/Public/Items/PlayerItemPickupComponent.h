#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/PlayerInventoryTypes.h"
#include "PlayerItemPickupComponent.generated.h"

class AWeaponBase;
class UPlayerItemDefinition;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerItemPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintPure, Category="Pickup")
	FPlayerInventoryItem GetPickupItem() const;

	UFUNCTION(BlueprintPure, Category="Pickup")
	TSubclassOf<AWeaponBase> GetPickupWeaponClass() const;

	UFUNCTION(BlueprintPure, Category="Pickup")
	UPlayerItemDefinition* GetItemDefinition() const;

	UFUNCTION(BlueprintCallable, Category="Pickup")
	void ConsumePickup();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UPlayerItemDefinition> ItemDefinition;
};
