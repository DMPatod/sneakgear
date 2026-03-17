#pragma once

#include "CoreMinimal.h"
#include "Types/PlayerInventoryTypes.h"
#include "UObject/Interface.h"
#include "PickupItemInterface.generated.h"

class UPlayerItemDefinition;
class UStaticMesh;

UINTERFACE(BlueprintType)
class SNEAKGEAR_API UPickupItemInterface : public UInterface
{
	GENERATED_BODY()
};

class SNEAKGEAR_API IPickupItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	UPlayerItemDefinition* GetPickupItemDefinition() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	UStaticMesh* GetPickupDisplayMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	EPlayerItemSlot GetPickupSlot() const;
};
