#include "UI/InfoPromptNativeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

void UInfoPromptNativeWidget::NativeConstruct()
{
	BuildWidgetTree();
	Super::NativeConstruct();
}

void UInfoPromptNativeWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* Root = Cast<UCanvasPanel>(WidgetTree->RootWidget);
	if (!Root)
	{
		Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("InfoPromptRoot"));
		WidgetTree->RootWidget = Root;
	}

	if (!PromptBorder)
	{
		PromptBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("InfoPromptBorder"));
		UCanvasPanelSlot* BorderSlot = Root->AddChildToCanvas(PromptBorder);
		if (BorderSlot)
		{
			BorderSlot->SetAutoSize(true);
			BorderSlot->SetAnchors(FAnchors(0.5f, 0.78f));
			BorderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		}
	}

	PromptBorder->SetBrushColor(BackgroundColor.GetSpecifiedColor());
	PromptBorder->SetPadding(PromptPadding);

	if (!PromptLabel)
	{
		PromptLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InfoPromptLabel"));
	}

	PromptLabel->SetColorAndOpacity(PromptColor);
	PromptBorder->SetContent(PromptLabel);
}
