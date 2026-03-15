#include "TestUIWidgets.h"

#include "Components/TextBlock.h"

FText UTestPlayerVitalsWidget::GetHealthDisplayText() const
{
	return HealthText ? HealthText->GetText() : FText::GetEmpty();
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
