#pragma once

#include "CoreMinimal.h"
#include "Items/PlayerItemDefinition.h"
#include "UtilityItemDefinition.generated.h"

UCLASS(BlueprintType)
class SNEAKGEAR_API UUtilityItemDefinition : public UPlayerItemDefinition
{
	GENERATED_BODY()

public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual bool UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;
	virtual bool ShouldConsumeOnUse() const override;

protected:
	virtual bool ExecuteUtility(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Utility")
	bool bConsumeAfterUse = false;
};
