#include "Items/EquipmentItemDefinition.h"

#include "GameplayEffect.h"
#include "Misc/DataValidation.h"
#include "Blueprint/UserWidget.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/PlayerCharacterBase.h"

EDataValidationResult UEquipmentItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (SlotType != EPlayerItemSlot::Equipped)
	{
		Context.AddError(FText::FromString(TEXT("Equipment items must use the Equipped slot type.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!ActiveGameplayEffect && !ActivePostProcessMaterial && !ActiveHUDOverlayWidgetClass)
	{
		Context.AddWarning(FText::FromString(TEXT("No gameplay effect, post-process material, or HUD overlay is configured.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (ActivePostProcessMaterial && PostProcessBlendWeight <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("PostProcessBlendWeight must be greater than 0 when a post-process material is assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool UEquipmentItemDefinition::ActivateItem(APlayerCharacterBase* PlayerCharacter,
                                            UPlayerInventoryComponent* InventoryComponent) const
{
	if (!PlayerCharacter || !InventoryComponent)
	{
		return false;
	}

	if (ActiveGameplayEffect)
	{
		InventoryComponent->SetActiveEffectHandleForItem(this, PlayerCharacter->ApplyGameplayEffectToSelf(ActiveGameplayEffect));
	}

	if (ActivePostProcessMaterial)
	{
		PlayerCharacter->SetCameraPostProcessMaterialEnabled(ActivePostProcessMaterial, true, PostProcessBlendWeight);
	}

	if (ActiveHUDOverlayWidgetClass)
	{
		PlayerCharacter->SetHUDOverlayWidgetVisible(ActiveHUDOverlayWidgetClass, true);
	}

	return ActiveGameplayEffect || ActivePostProcessMaterial || ActiveHUDOverlayWidgetClass;
}

void UEquipmentItemDefinition::DeactivateItem(APlayerCharacterBase* PlayerCharacter,
                                              UPlayerInventoryComponent* InventoryComponent) const
{
	if (!PlayerCharacter || !InventoryComponent)
	{
		return;
	}

	if (ActiveGameplayEffect)
	{
		PlayerCharacter->RemoveGameplayEffectFromSelf(InventoryComponent->GetActiveEffectHandleForItem(this));
		InventoryComponent->ClearActiveEffectHandleForItem(this);
	}

	if (ActivePostProcessMaterial)
	{
		PlayerCharacter->SetCameraPostProcessMaterialEnabled(ActivePostProcessMaterial, false, 0.f);
	}

	if (ActiveHUDOverlayWidgetClass)
	{
		PlayerCharacter->SetHUDOverlayWidgetVisible(ActiveHUDOverlayWidgetClass, false);
	}
}
