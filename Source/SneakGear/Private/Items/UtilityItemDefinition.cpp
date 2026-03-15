#include "Items/UtilityItemDefinition.h"

#include "Misc/DataValidation.h"

EDataValidationResult UUtilityItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (SlotType != EPlayerItemSlot::Utility)
	{
		Context.AddError(FText::FromString(TEXT("Utility items must use the Utility slot type.")));
		Result = EDataValidationResult::Invalid;
	}

	if (GetClass() == StaticClass())
	{
		Context.AddWarning(FText::FromString(TEXT("Base UtilityItemDefinition does not implement any behavior on use.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

bool UUtilityItemDefinition::UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const
{
	return ExecuteUtility(PlayerCharacter, InventoryComponent);
}

bool UUtilityItemDefinition::ShouldConsumeOnUse() const
{
	return bConsumeAfterUse;
}

bool UUtilityItemDefinition::ExecuteUtility(APlayerCharacterBase* PlayerCharacter,
                                            UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	return false;
}
