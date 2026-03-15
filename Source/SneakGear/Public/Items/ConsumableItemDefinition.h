#pragma once

#include "CoreMinimal.h"
#include "Items/PlayerItemDefinition.h"
#include "ConsumableItemDefinition.generated.h"

class UGameplayEffect;

UCLASS(BlueprintType)
class SNEAKGEAR_API UConsumableItemDefinition : public UPlayerItemDefinition
{
	GENERATED_BODY()

public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual bool UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;
	virtual bool ShouldConsumeOnUse() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Support")
	TSubclassOf<UGameplayEffect> UseGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Support", meta=(ClampMin="0.0"))
	float DirectHealthRestore = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Support")
	bool bConsumeAfterUse = true;
};
