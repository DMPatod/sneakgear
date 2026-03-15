#pragma once

#include "CoreMinimal.h"
#include "Items/ConsumableItemDefinition.h"
#include "MedkitItemDefinition.generated.h"

UCLASS(BlueprintType)
class SNEAKGEAR_API UMedkitItemDefinition : public UConsumableItemDefinition
{
	GENERATED_BODY()

public:
	UMedkitItemDefinition();
};
