#pragma once

#include "CoreMinimal.h"
#include "Types/PlayerInventoryTypes.h"
#include "Types/PlayerMovementTypes.h"
#include "PlayerUITypes.generated.h"

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FPlayerVitalsViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Health = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Stamina = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxStamina = 0.f;
};

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FWeaponStatusViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHasWeapon = false;

	UPROPERTY(BlueprintReadOnly)
	FText WeaponName;

	UPROPERTY(BlueprintReadOnly)
	float FireRate = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 InClip = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ClipSize = 0;
};

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FWeaponQuickSlotViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EPlayerItemSlot Slot = EPlayerItemSlot::PrimaryWeapon;

	UPROPERTY(BlueprintReadOnly)
	bool bHasWeapon = false;

	UPROPERTY(BlueprintReadOnly)
	FText WeaponName;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 InClip = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ClipSize = 0;
};

USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FStealthDebugViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHasPlayer = false;

	UPROPERTY(BlueprintReadOnly)
	bool bAiming = false;

	UPROPERTY(BlueprintReadOnly)
	EStance Stance = EStance::Standing;

	UPROPERTY(BlueprintReadOnly)
	bool bInCover = false;

	UPROPERTY(BlueprintReadOnly)
	bool bVaulting = false;

	UPROPERTY(BlueprintReadOnly)
	float CoverMoveAxis = 0.f;

	UPROPERTY(BlueprintReadOnly)
	EPlayerItemSlot ActiveWeaponSlot = EPlayerItemSlot::PrimaryWeapon;

	UPROPERTY(BlueprintReadOnly)
	FText WeaponName;

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 InClip = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ClipSize = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bFiredRecently = false;

	UPROPERTY(BlueprintReadOnly)
	float Speed2D = 0.f;
};
