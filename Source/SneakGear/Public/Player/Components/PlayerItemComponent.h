#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemComponent.generated.h"

class AWeaponBase;
class AWorldItemPickup;

UENUM(BlueprintType)
enum class EPlayerItemSlot : uint8
{
	Equipped UMETA(DisplayName="Equipped"),
	Support UMETA(DisplayName="Support"),
	PrimaryWeapon UMETA(DisplayName="Primary Weapon"),
	SecondaryWeapon UMETA(DisplayName="Secondary Weapon")
};

USTRUCT(BlueprintType)
struct FPlayerInventoryItem
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotUpdated, EPlayerItemSlot, Slot);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponFired, EPlayerItemSlot);
DECLARE_MULTICAST_DELEGATE(FOnInventoryStateChanged);

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerItemComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(const FPlayerInventoryItem& Item, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(EPlayerItemSlot Slot, FPlayerInventoryItem& OutItem);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool PickUpFromFloor(AWorldItemPickup* PickupActor, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryPickUpNearbyFloorItem(float SearchRadius = 150.f, bool bAllowReplace = false);

	UFUNCTION(BlueprintPure, Category="Inventory")
	bool HasItem(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	FPlayerInventoryItem GetItem(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	bool SetActiveWeaponSlot(EPlayerItemSlot WeaponSlot, bool bEquipInHand = true);

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	bool SetWeaponEquipped(bool bNewEquipped);

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	bool IsWeaponEquipped() const
	{
		return bWeaponEquipped;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	EPlayerItemSlot GetActiveWeaponSlot() const
	{
		return ActiveWeaponSlot;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	AWeaponBase* GetWeaponInSlot(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	AWeaponBase* GetActiveWeapon() const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void StartActiveWeaponFire();

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	void StopActiveWeaponFire();

	UFUNCTION(BlueprintCallable, Category="Inventory|Weapons")
	bool ReloadActiveWeapon();

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetInClip(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetClipSize(EPlayerItemSlot WeaponSlot) const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetActiveWeaponInClip() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetActiveWeaponClipSize() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	int32 GetReserveAmmoCount() const;

	UFUNCTION(BlueprintPure, Category="Inventory|Weapons")
	bool WasActiveWeaponFiredRecently(float WindowSeconds = 0.12f) const;

	FOnActiveWeaponFired& OnActiveWeaponFiredEvent()
	{
		return OnActiveWeaponFired;
	}

	FOnInventoryStateChanged& OnInventoryStateChangedEvent()
	{
		return OnInventoryStateChanged;
	}

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnItemSlotUpdated OnItemSlotUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	TSubclassOf<AWeaponBase> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	TSubclassOf<AWeaponBase> SecondaryWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	EPlayerItemSlot ActiveWeaponSlot = EPlayerItemSlot::PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	bool bWeaponEquipped = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Weapons")
	FName WeaponHandSocketName = "hand_r_socket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Weapons")
	FName PrimaryWeaponHolsterSocketName = "back_w_socket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Weapons")
	FName SecondaryWeaponHolsterSocketName = "spine_w_socket";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FPlayerInventoryItem EquippedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FPlayerInventoryItem SupportItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FPlayerInventoryItem PrimaryWeaponItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	FPlayerInventoryItem SecondaryWeaponItem;

private:
	struct FWeaponSlotRuntime
	{
		TObjectPtr<AWeaponBase> WeaponActor = nullptr;
		int32 InClip = -1;
	};

	FWeaponSlotRuntime PrimaryWeaponRuntime;
	FWeaponSlotRuntime SecondaryWeaponRuntime;
	FOnActiveWeaponFired OnActiveWeaponFired;
	FOnInventoryStateChanged OnInventoryStateChanged;
	float LastActiveWeaponFireTimestamp = -1000.f;

	FPlayerInventoryItem* ResolveMutableSlot(EPlayerItemSlot Slot);
	const FPlayerInventoryItem* ResolveSlot(EPlayerItemSlot Slot) const;
	FWeaponSlotRuntime* ResolveWeaponRuntimeMutable(EPlayerItemSlot Slot);
	const FWeaponSlotRuntime* ResolveWeaponRuntime(EPlayerItemSlot Slot) const;
	bool HasValidWeaponItem(EPlayerItemSlot Slot) const;
	AWeaponBase* SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass) const;
	void AttachWeapon(AWeaponBase* Weapon, FName SocketName, bool bUseHolsterOffset) const;
	FName GetHolsterSocketForSlot(EPlayerItemSlot WeaponSlot) const;
	void SyncWeaponAttachments() const;
	void HandleWeaponFired(EPlayerItemSlot Slot);
	void OnPrimaryWeaponFired();
	void OnSecondaryWeaponFired();
};
