#include "UI/PlayerVitalsNativeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UPlayerVitalsNativeWidget::NativeConstruct()
{
	BuildWidgetTree();
	Super::NativeConstruct();
}

void UPlayerVitalsNativeWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	RootBorder = Cast<UBorder>(WidgetTree->RootWidget);
	if (!RootBorder)
	{
		RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PlayerVitalsBorder"));
		WidgetTree->RootWidget = RootBorder;
	}

	RootBorder->SetBrushColor(PanelColor.GetSpecifiedColor());
	RootBorder->SetPadding(PanelPadding);

	VitalsBox = VitalsBox ? VitalsBox.Get() : Cast<UVerticalBox>(RootBorder->GetContent());
	if (!VitalsBox)
	{
		VitalsBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PlayerVitalsBox"));
	}

	RootBorder->SetContent(VitalsBox);

	if (!HealthText)
	{
		HealthText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealthText"));
		VitalsBox->AddChildToVerticalBox(HealthText);
	}

	HealthText->SetColorAndOpacity(LabelColor);

	if (!HealthBar)
	{
		USizeBox* HealthBarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("HealthBarBox"));
		HealthBarBox->SetWidthOverride(BarSize.X);
		HealthBarBox->SetHeightOverride(BarSize.Y);

		HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
		HealthBarBox->SetContent(HealthBar);
		if (UVerticalBoxSlot* BarSlot = VitalsBox->AddChildToVerticalBox(HealthBarBox))
		{
			BarSlot->SetPadding(FMargin(0.f, 4.f, 0.f, 10.f));
		}
	}

	HealthBar->SetFillColorAndOpacity(HealthFillColor);

	if (!StaminaText)
	{
		StaminaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaminaText"));
		VitalsBox->AddChildToVerticalBox(StaminaText);
	}

	StaminaText->SetColorAndOpacity(LabelColor);

	if (!StaminaBar)
	{
		USizeBox* StaminaBarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("StaminaBarBox"));
		StaminaBarBox->SetWidthOverride(BarSize.X);
		StaminaBarBox->SetHeightOverride(BarSize.Y);

		StaminaBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("StaminaBar"));
		StaminaBarBox->SetContent(StaminaBar);
		if (UVerticalBoxSlot* BarSlot = VitalsBox->AddChildToVerticalBox(StaminaBarBox))
		{
			BarSlot->SetPadding(FMargin(0.f, 4.f, 0.f, 0.f));
		}
	}

	StaminaBar->SetFillColorAndOpacity(StaminaFillColor);
}
