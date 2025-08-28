#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDefinition.h"
#include "EquipableToolDefinition.generated.h"

class AEquipableToolBase;
class UInputMappingContext;

UCLASS(BlueprintType, Blueprintable)
class SNEAKGEAR_API UEquipableToolDefinition : public UItemDefinition
{
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEquipableToolBase> ToolAsset;

	UFUNCTION()
	virtual UEquipableToolDefinition* CreateItemCopy() const override;

private:
	GENERATED_BODY()

};
