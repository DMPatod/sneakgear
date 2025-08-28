#pragma once

#include "CoreMinimal.h"
#include "Data/ItemData.h"
#include "ItemDefinition.generated.h"


UCLASS(BlueprintType, Blueprintable)
class SNEAKGEAR_API UItemDefinition : public UDataAsset
{
public:
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemText ItemText;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	virtual UItemDefinition* CreateItemCopy() const;

private:
	GENERATED_BODY()
};
