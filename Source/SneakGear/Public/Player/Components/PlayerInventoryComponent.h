#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "Types/PlayerInventoryTypes.h"
#include "PlayerInventoryComponent.generated.h"

class AWeaponBase;
class AActor;
class UPlayerItemPickupComponent;
class UPlayerItemDefinition;
class APlayerCharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotUpdated, EPlayerItemSlot, Slot);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveWeaponFired, EPlayerItemSlot);
DECLARE_MULTICAST_DELEGATE(FOnInventoryStateChanged);

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInventoryComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(const FPlayerInventoryItem& Item, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(EPlayerItemSlot Slot, FPlayerInventoryItem& OutItem);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool PickUpFromFloor(AActor* PickupActor, bool bAllowReplace = false);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryPickUpNearbyFloorItem(float SearchRadius = 150.f, bool bAllowReplace = false);

	UFUNCTION(BlueprintPure, Category="Inventory")
	bool HasItem(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	FPlayerInventoryItem GetItem(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetItemCount(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	FPlayerInventoryItem GetItemAt(EPlayerItemSlot Slot, int32 Index) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItemAt(EPlayerItemSlot Slot, int32 Index, FPlayerInventoryItem& OutItem);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SetActiveItemIndex(EPlayerItemSlot Slot, int32 Index);

	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetActiveItemIndex(EPlayerItemSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UseActiveSupportItem();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UseActiveUtilityItem();

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

	void SetActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition, FActiveGameplayEffectHandle EffectHandle);
	FActiveGameplayEffectHandle GetActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition) const;
	void ClearActiveEffectHandleForItem(const UPlayerItemDefinition* ItemDefinition);

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnItemSlotUpdated OnItemSlotUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	TSubclassOf<AWeaponBase> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	TSubclassOf<AWeaponBase> SecondaryWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Weapons")
	TSubclassOf<AWeaponBase> UnarmedWeaponClass;

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
	TArray<FPlayerInventoryItem> EquippedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FPlayerInventoryItem> SupportItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 ActiveEquippedItemIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 ActiveSupportItemIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FPlayerInventoryItem> UtilityItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 ActiveUtilityItemIndex = INDEX_NONE;

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
	UPROPERTY(Transient)
	TObjectPtr<AWeaponBase> UnarmedWeapon = nullptr;
	FOnActiveWeaponFired OnActiveWeaponFired;
	FOnInventoryStateChanged OnInventoryStateChanged;
	float LastActiveWeaponFireTimestamp = -1000.f;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UPlayerItemDefinition>> EquippedItemDefinitions;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UPlayerItemDefinition>> SupportItemDefinitions;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UPlayerItemDefinition>> UtilityItemDefinitions;
	UPROPERTY(Transient)
	TObjectPtr<UPlayerItemDefinition> PrimaryWeaponItemDefinition;
	UPROPERTY(Transient)
	TObjectPtr<UPlayerItemDefinition> SecondaryWeaponItemDefinition;
	TMap<TObjectPtr<const UPlayerItemDefinition>, FActiveGameplayEffectHandle> ActiveItemEffectHandles;

	TArray<FPlayerInventoryItem>* ResolveMutableCollection(EPlayerItemSlot Slot);
	const TArray<FPlayerInventoryItem>* ResolveCollection(EPlayerItemSlot Slot) const;
	TArray<TObjectPtr<UPlayerItemDefinition>>* ResolveMutableDefinitionCollection(EPlayerItemSlot Slot);
	const TArray<TObjectPtr<UPlayerItemDefinition>>* ResolveDefinitionCollection(EPlayerItemSlot Slot) const;
	UPlayerItemDefinition* GetItemDefinition(EPlayerItemSlot Slot) const;
	UPlayerItemDefinition* GetItemDefinitionAt(EPlayerItemSlot Slot, int32 Index) const;
	bool AddItemInternal(const FPlayerInventoryItem& Item, UPlayerItemDefinition* ItemDefinition, bool bAllowReplace);
	void SetItemDefinitionForSlot(EPlayerItemSlot Slot, UPlayerItemDefinition* ItemDefinition);
	void ActivateCurrentEquippedItem();
	void DeactivateCurrentEquippedItem();
	APlayerCharacterBase* GetOwnerPlayerCharacter() const;
	void NormalizeActiveItemIndex(EPlayerItemSlot Slot);
	int32& ResolveActiveItemIndex(EPlayerItemSlot Slot);
	int32 ResolveActiveItemIndex(EPlayerItemSlot Slot) const;
	FPlayerInventoryItem* ResolveMutableSlot(EPlayerItemSlot Slot);
	const FPlayerInventoryItem* ResolveSlot(EPlayerItemSlot Slot) const;
	FWeaponSlotRuntime* ResolveWeaponRuntimeMutable(EPlayerItemSlot Slot);
	const FWeaponSlotRuntime* ResolveWeaponRuntime(EPlayerItemSlot Slot) const;
	bool HasValidWeaponItem(EPlayerItemSlot Slot) const;
	bool HasValidWeaponSelection(EPlayerItemSlot Slot) const;
	bool SetWeaponClassForSlot(EPlayerItemSlot Slot, TSubclassOf<AWeaponBase> WeaponClass);
	AWeaponBase* SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass) const;
	void AttachWeapon(AWeaponBase* Weapon, FName SocketName, bool bUseHolsterOffset) const;
	FName GetHolsterSocketForSlot(EPlayerItemSlot WeaponSlot) const;
	void SyncWeaponAttachments() const;
	void HandleWeaponFired(EPlayerItemSlot Slot);
	void BindRuntimeWeaponDelegates(EPlayerItemSlot Slot, AWeaponBase* WeaponActor);
	void ClearRuntimeWeapon(FWeaponSlotRuntime& Runtime) const;
	void OnPrimaryWeaponFired();
	void OnSecondaryWeaponFired();
};
