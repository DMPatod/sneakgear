#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Items/PickupItemInterface.h"
#include "Types/PlayerInventoryTypes.h"
#include "PlayerItemDefinition.generated.h"

class AWeaponBase;
class APlayerCharacterBase;
class UPlayerInventoryComponent;

UCLASS(BlueprintType)
class SNEAKGEAR_API UPlayerItemDefinition : public UPrimaryDataAsset, public IPickupItemInterface
{
	GENERATED_BODY()

public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual bool ActivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const;
	virtual void DeactivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const;
	virtual bool UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const;
	virtual bool ShouldConsumeOnUse() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FName ItemId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	EPlayerItemSlot SlotType = EPlayerItemSlot::Equipped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Pickup")
	TObjectPtr<UStaticMesh> PickupMesh;

	virtual UPlayerItemDefinition* GetPickupItemDefinition_Implementation() const override;
	virtual UStaticMesh* GetPickupDisplayMesh_Implementation() const override;
	virtual EPlayerItemSlot GetPickupSlot_Implementation() const override;

	UFUNCTION(BlueprintPure, Category="Item")
	FPlayerInventoryItem BuildInventoryItem() const
	{
		FPlayerInventoryItem Item;
		Item.ItemId = ItemId;
		Item.DisplayName = DisplayName;
		Item.SlotType = SlotType;
		return Item;
	}
};
