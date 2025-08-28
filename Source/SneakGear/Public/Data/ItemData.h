#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

class UItemDefinition;

UENUM()
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Tool UMETA(DisplayName = "Tool"),
	Consumable UMETA(DisplayName = "Consumable"),
};

USTRUCT(BlueprintType)
struct FItemText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemText ItemText;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	TObjectPtr<UItemDefinition> ItemBase;
};