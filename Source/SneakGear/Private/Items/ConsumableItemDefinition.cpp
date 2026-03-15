#include "Items/ConsumableItemDefinition.h"

#include "GameplayEffect.h"
#include "Misc/DataValidation.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/PlayerCharacterBase.h"

EDataValidationResult UConsumableItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (SlotType != EPlayerItemSlot::Support)
	{
		Context.AddError(FText::FromString(TEXT("Consumable items must use the Support slot type.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!UseGameplayEffect && DirectHealthRestore <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("Consumable items need either a gameplay effect or a direct health restore value.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool UConsumableItemDefinition::UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const
{
	(void)InventoryComponent;

	if (!PlayerCharacter)
	{
		return false;
	}

	bool bApplied = false;
	if (UseGameplayEffect)
	{
		bApplied |= PlayerCharacter->ApplyGameplayEffectToSelf(UseGameplayEffect).IsValid();
	}

	if (DirectHealthRestore > 0.f)
	{
		bApplied |= PlayerCharacter->ApplyHealthDelta(DirectHealthRestore);
	}

	return bApplied;
}

bool UConsumableItemDefinition::ShouldConsumeOnUse() const
{
	return bConsumeAfterUse;
}
