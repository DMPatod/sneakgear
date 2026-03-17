#include "Items/PlayerItemDefinition.h"

#include "Misc/DataValidation.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/PlayerCharacterBase.h"
#include "Weapon/WeaponBase.h"

EDataValidationResult UPlayerItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (ItemId.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("ItemId must be assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (DisplayName.IsEmpty())
	{
		Context.AddWarning(FText::FromString(TEXT("DisplayName is empty; UI will fall back to ItemId.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	const bool bIsWeaponSlot = SlotType == EPlayerItemSlot::PrimaryWeapon || SlotType == EPlayerItemSlot::SecondaryWeapon;
	if (bIsWeaponSlot && !WeaponClass)
	{
		Context.AddError(FText::FromString(TEXT("WeaponClass is required for primary and secondary weapon items.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!bIsWeaponSlot && WeaponClass)
	{
		Context.AddWarning(FText::FromString(TEXT("WeaponClass is assigned for a non-weapon item and will be ignored.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!PickupMesh)
	{
		Context.AddWarning(FText::FromString(TEXT("PickupMesh is not assigned; world pickups using this item will not display a mesh.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

bool UPlayerItemDefinition::ActivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	return true;
}

void UPlayerItemDefinition::DeactivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
}

bool UPlayerItemDefinition::UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	return false;
}

bool UPlayerItemDefinition::ShouldConsumeOnUse() const
{
	return false;
}

UPlayerItemDefinition* UPlayerItemDefinition::GetPickupItemDefinition_Implementation() const
{
	return const_cast<UPlayerItemDefinition*>(this);
}

UStaticMesh* UPlayerItemDefinition::GetPickupDisplayMesh_Implementation() const
{
	return PickupMesh;
}

EPlayerItemSlot UPlayerItemDefinition::GetPickupSlot_Implementation() const
{
	return SlotType;
}
