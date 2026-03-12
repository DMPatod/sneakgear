#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Components/PlayerItemComponent.h"
#include "WorldItemPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class SNEAKGEAR_API AWorldItemPickup : public AActor
{
	GENERATED_BODY()

public:
	AWorldItemPickup();

	UFUNCTION(BlueprintPure, Category="Pickup")
	FPlayerInventoryItem GetPickupItem() const
	{
		return PickupItem;
	}

	UFUNCTION(BlueprintCallable, Category="Pickup")
	void ConsumePickup();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USphereComponent> PickupTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	FPlayerInventoryItem PickupItem;
};
