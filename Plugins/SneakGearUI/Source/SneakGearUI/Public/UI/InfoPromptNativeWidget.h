#pragma once

#include "CoreMinimal.h"
#include "UI/InfoPromptWidget.h"
#include "InfoPromptNativeWidget.generated.h"

class UBorder;

UCLASS()
class SNEAKGEARUI_API UInfoPromptNativeWidget : public UInfoPromptWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category="Info Prompt|Style")
	FSlateColor PromptColor = FLinearColor(0.96f, 0.93f, 0.78f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Info Prompt|Style")
	FSlateColor BackgroundColor = FLinearColor(0.04f, 0.05f, 0.07f, 0.82f);

	UPROPERTY(EditDefaultsOnly, Category="Info Prompt|Style")
	FMargin PromptPadding = FMargin(14.f, 8.f);

private:
	void BuildWidgetTree();

	UPROPERTY(Transient)
	TObjectPtr<UBorder> PromptBorder;
};
