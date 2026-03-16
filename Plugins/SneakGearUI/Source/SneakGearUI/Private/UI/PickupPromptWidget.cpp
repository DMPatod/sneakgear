#include "UI/PickupPromptWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UPickupPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureRuntimeWidgetTree();
	ApplyCachedState();
}

void UPickupPromptWidget::SetPickupInfo(const FText& ItemName, const FText& SlotLabel)
{
	CachedItemName = ItemName;
	CachedSlotLabel = SlotLabel;
	ApplyCachedState();
}

void UPickupPromptWidget::SetSwapPrompt(bool bInRequiresHoldToSwap)
{
	bRequiresHoldToSwap = bInRequiresHoldToSwap;
	ApplyCachedState();
}

void UPickupPromptWidget::SetHoldProgress(float InProgress)
{
	CachedHoldProgress = FMath::Clamp(InProgress, 0.f, 1.f);
	ApplyCachedState();
}

void UPickupPromptWidget::SetPromptVisible(bool bVisible)
{
	bPromptVisible = bVisible;
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UPickupPromptWidget::ApplyCachedState()
{
	if (PromptText)
	{
		PromptText->SetText(FText::Format(bRequiresHoldToSwap ? SwapPromptFormat : PromptFormat, CachedItemName));
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(CachedItemName);
	}

	if (SlotText)
	{
		SlotText->SetText(CachedSlotLabel);
	}

	if (HoldProgressBar)
	{
		HoldProgressBar->SetPercent(CachedHoldProgress);
		HoldProgressBar->SetVisibility(bRequiresHoldToSwap ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	SetVisibility(bPromptVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UPickupPromptWidget::EnsureRuntimeWidgetTree()
{
	if ((PromptText || ItemNameText || SlotText || HoldProgressBar) || !WidgetTree)
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

	HoldProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HoldProgressBar"));
	Root->AddChild(HoldProgressBar);
}
