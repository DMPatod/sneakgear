#pragma once

#include "CoreMinimal.h"
#include "PlayerInventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerItemSlot : uint8
{
	Equipped UMETA(DisplayName="Equipped"),
	Support UMETA(DisplayName="Support"),
	PrimaryWeapon UMETA(DisplayName="Primary Weapon"),
	SecondaryWeapon UMETA(DisplayName="Secondary Weapon"),
	Utility UMETA(DisplayName="Utility")
};

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FPlayerInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	EPlayerItemSlot SlotType = EPlayerItemSlot::Equipped;

	bool IsValid() const
	{
		return ItemId != NAME_None;
	}
};
