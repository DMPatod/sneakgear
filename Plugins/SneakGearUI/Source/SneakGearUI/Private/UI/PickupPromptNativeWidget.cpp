#include "UI/PickupPromptNativeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UPickupPromptNativeWidget::NativeConstruct()
{
	BuildWidgetTree();
	Super::NativeConstruct();
}

void UPickupPromptNativeWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	UVerticalBox* Root = Cast<UVerticalBox>(WidgetTree->RootWidget);
	if (!Root)
	{
		Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PickupPromptRoot"));
		WidgetTree->RootWidget = Root;
	}

	if (!PromptText)
	{
		PromptText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PromptText"));
		PromptText->SetColorAndOpacity(PromptColor);
		Root->AddChild(PromptText);
	}

	if (!ItemNameText)
	{
		ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemNameText"));
		ItemNameText->SetColorAndOpacity(PromptColor);
		Root->AddChild(ItemNameText);
	}

	if (!SlotText)
	{
		SlotText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotText"));
		SlotText->SetColorAndOpacity(SecondaryColor);
		Root->AddChild(SlotText);
	}

	if (!HoldProgressBar)
	{
		HoldProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HoldProgressBar"));
		HoldProgressBar->SetFillColorAndOpacity(ProgressFillColor);
		Root->AddChild(HoldProgressBar);
	}
}
