#include "TestUIWidgets.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

FText UTestPlayerVitalsWidget::GetHealthDisplayText() const
{
	return HealthText ? HealthText->GetText() : FText::GetEmpty();
}

void UTestPlayerVitalsWidget::ConstructForTest()
{
	NativeConstruct();
}

FText UTestPlayerVitalsWidget::GetStaminaDisplayText() const
{
	return StaminaText ? StaminaText->GetText() : FText::GetEmpty();
}

void UTestStanceWidget::ConstructForTest()
{
	NativeConstruct();
}

FText UTestStanceWidget::GetStanceDisplayText() const
{
	return StanceText ? StanceText->GetText() : FText::GetEmpty();
}

FText UTestEventFeedWidget::GetEventFeedText() const
{
	return EventText ? EventText->GetText() : FText::GetEmpty();
}

void UTestPickupPromptWidget::ConstructForTest()
{
	NativeConstruct();
}

FText UTestPickupPromptWidget::GetPromptDisplayText() const
{
	return PromptText ? PromptText->GetText() : FText::GetEmpty();
}

float UTestPickupPromptWidget::GetHoldProgressValue() const
{
	return HoldProgressBar ? HoldProgressBar->GetPercent() : 0.f;
}

ESlateVisibility UTestPickupPromptWidget::GetHoldProgressVisibility() const
{
	return HoldProgressBar ? HoldProgressBar->GetVisibility() : ESlateVisibility::Collapsed;
}
