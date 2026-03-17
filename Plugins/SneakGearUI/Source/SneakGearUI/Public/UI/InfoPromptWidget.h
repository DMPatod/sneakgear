#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoPromptWidget.generated.h"

class UTextBlock;

UCLASS()
class SNEAKGEARUI_API UInfoPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category="UI|Prompt")
	void SetPromptText(const FText& InPromptText);

	UFUNCTION(BlueprintCallable, Category="UI|Prompt")
	void SetPromptVisible(bool bVisible);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Prompt")
	FText PromptText = FText::GetEmpty();

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PromptLabel;

private:
	bool bPromptVisible = false;

	void ApplyCachedState();
	void EnsureRuntimeWidgetTree();
};
