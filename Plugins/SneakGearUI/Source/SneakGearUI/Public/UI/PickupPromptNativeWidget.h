#pragma once

#include "CoreMinimal.h"
#include "UI/PickupPromptWidget.h"
#include "PickupPromptNativeWidget.generated.h"

UCLASS()
class SNEAKGEARUI_API UPickupPromptNativeWidget : public UPickupPromptWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category="Pickup|Style")
	FSlateColor PromptColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Pickup|Style")
	FSlateColor SecondaryColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Pickup|Style")
	FLinearColor ProgressFillColor = FLinearColor(0.9f, 0.8f, 0.25f, 1.f);

private:
	void BuildWidgetTree();
};
