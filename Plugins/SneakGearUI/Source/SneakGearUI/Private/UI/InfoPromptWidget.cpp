#include "UI/InfoPromptWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

void UInfoPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureRuntimeWidgetTree();
	ApplyCachedState();
}

void UInfoPromptWidget::SetPromptText(const FText& InPromptText)
{
	PromptText = InPromptText;
	ApplyCachedState();
}

void UInfoPromptWidget::SetPromptVisible(bool bVisible)
{
	bPromptVisible = bVisible;
	ApplyCachedState();
}

void UInfoPromptWidget::ApplyCachedState()
{
	if (PromptLabel)
	{
		PromptLabel->SetText(PromptText);
	}

	SetVisibility(bPromptVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UInfoPromptWidget::EnsureRuntimeWidgetTree()
{
	if (PromptLabel || !WidgetTree)
	{
		return;
	}

	PromptLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PromptLabel"));
	WidgetTree->RootWidget = PromptLabel;
}
