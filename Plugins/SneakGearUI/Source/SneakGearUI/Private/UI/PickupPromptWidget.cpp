#include "UI/PickupPromptWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UPickupPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureRuntimeWidgetTree();
}

void UPickupPromptWidget::SetPickupInfo(const FText& ItemName, const FText& SlotLabel)
{
	if (PromptText)
	{
		PromptText->SetText(FText::Format(PromptFormat, ItemName));
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(ItemName);
	}

	if (SlotText)
	{
		SlotText->SetText(SlotLabel);
	}
}

void UPickupPromptWidget::SetPromptVisible(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UPickupPromptWidget::EnsureRuntimeWidgetTree()
{
	if (PromptText || ItemNameText || SlotText || !WidgetTree)
	{
		return;
	}

	UVerticalBox* Root = Cast<UVerticalBox>(WidgetTree->RootWidget);
	if (!Root)
	{
		Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PickupPromptRoot"));
		WidgetTree->RootWidget = Root;
	}

	PromptText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PromptText"));
	Root->AddChild(PromptText);

	ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemNameText"));
	Root->AddChild(ItemNameText);

	SlotText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotText"));
	Root->AddChild(SlotText);
}
