#pragma once

#include "CoreMinimal.h"
#include "Items/UtilityItemDefinition.h"
#include "ScannerItemDefinition.generated.h"

UCLASS(BlueprintType)
class SNEAKGEAR_API UScannerItemDefinition : public UUtilityItemDefinition
{
	GENERATED_BODY()

public:
	UScannerItemDefinition();

	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

protected:
	virtual bool ExecuteUtility(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Scanner", meta=(ClampMin="1.0"))
	float ScanRadius = 1500.f;
};
