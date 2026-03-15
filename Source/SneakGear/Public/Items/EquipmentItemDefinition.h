#pragma once

#include "CoreMinimal.h"
#include "Items/PlayerItemDefinition.h"
#include "EquipmentItemDefinition.generated.h"

class UGameplayEffect;
class UMaterialInterface;
class UUserWidget;

UCLASS(BlueprintType)
class SNEAKGEAR_API UEquipmentItemDefinition : public UPlayerItemDefinition
{
	GENERATED_BODY()

public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual bool ActivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;
	virtual void DeactivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipped")
	TSubclassOf<UGameplayEffect> ActiveGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipped")
	TObjectPtr<UMaterialInterface> ActivePostProcessMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipped", meta=(ClampMin="0.0"))
	float PostProcessBlendWeight = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipped")
	TSubclassOf<UUserWidget> ActiveHUDOverlayWidgetClass;
};
