#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupPromptWidget.generated.h"

class UTextBlock;

UCLASS()
class SNEAKGEARUI_API UPickupPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetPickupInfo(const FText& ItemName, const FText& SlotLabel);

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetPromptVisible(bool bVisible);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Pickup")
	FText PromptFormat = NSLOCTEXT("SneakGearUI", "PickupPromptFormat", "Pick up {0}");

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PromptText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotText;

private:
	void EnsureRuntimeWidgetTree();
};
