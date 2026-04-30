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

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	None UMETA(DisplayName="None"),
	Light UMETA(DisplayName="Light"),
	Shell UMETA(DisplayName="Shell"),
	Heavy UMETA(DisplayName="Heavy"),
	Energy UMETA(DisplayName="Energy")
};

UENUM(BlueprintType)
enum class EPlayerInventoryWeaponState : uint8
{
	Idle UMETA(DisplayName="Idle"),
	FireRequested UMETA(DisplayName="Fire Requested"),
	Firing UMETA(DisplayName="Firing"),
	Reloading UMETA(DisplayName="Reloading")
};

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FAmmoReserve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo")
	int32 Current = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ammo")
	int32 Max = 0;
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
