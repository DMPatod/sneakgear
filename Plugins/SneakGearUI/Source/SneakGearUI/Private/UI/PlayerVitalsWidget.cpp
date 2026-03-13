#include "UI/PlayerVitalsWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Pawn.h"
#include "UI/PlayerUIDataSource.h"

void UPlayerVitalsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!HealthBar && !StaminaBar && !HealthText && !StaminaText)
	{
		if (!WidgetTree)
		{
			return;
		}

		auto Root = WidgetTree->RootWidget;
		if (!Root)
		{
			Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VitalsRoot"));
			WidgetTree->RootWidget = Root;
		}

		auto* VBox = Cast<UVerticalBox>(Root);
		if (!VBox)
		{
			VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VitalsRoot"));
			WidgetTree->RootWidget = VBox;
		}

		HealthText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealthText"));
		VBox->AddChild(HealthText);

		HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
		VBox->AddChild(HealthBar);

		StaminaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StaminaText"));
		VBox->AddChild(StaminaText);

		StaminaBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("StaminaBar"));
		VBox->AddChild(StaminaBar);
	}

	CachedPlayer = GetOwningPlayerPawn();
	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIVitalsChangedEvent().AddUObject(
			this, &UPlayerVitalsWidget::HandleVitalsChanged);
	}

	UpdateFromPlayer();
}

void UPlayerVitalsWidget::NativeDestruct()
{
	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIVitalsChangedEvent().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UPlayerVitalsWidget::HandleVitalsChanged()
{
	UpdateFromPlayer();
}

const IPlayerUIDataSource* UPlayerVitalsWidget::GetPlayerUIDataSource() const
{
	return CachedPlayer.IsValid() ? Cast<IPlayerUIDataSource>(CachedPlayer.Get()) : nullptr;
}

void UPlayerVitalsWidget::UpdateFromPlayer()
{
	const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();
	if (!PlayerUIDataSource)
	{
		return;
	}

	FPlayerVitalsViewData Vitals;
	if (!PlayerUIDataSource->GetPlayerVitalsViewData(Vitals))
	{
		return;
	}

	if (HealthBar)
	{
		HealthBar->SetPercent(Vitals.MaxHealth > 0.f ? Vitals.Health / Vitals.MaxHealth : 0.f);
	}

	if (StaminaBar)
	{
		StaminaBar->SetPercent(Vitals.MaxStamina > 0.f ? Vitals.Stamina / Vitals.MaxStamina : 0.f);
	}

	if (HealthText)
	{
		HealthText->SetText(FText::Format(NSLOCTEXT("SneakGear", "HealthTextFmt", "Health: {0} / {1}"),
		                                  FText::AsNumber(FMath::RoundToInt(Vitals.Health)),
		                                  FText::AsNumber(FMath::RoundToInt(Vitals.MaxHealth))));
	}

	if (StaminaText)
	{
		StaminaText->SetText(FText::Format(NSLOCTEXT("SneakGear", "StaminaTextFmt", "Stamina: {0} / {1}"),
		                                   FText::AsNumber(FMath::RoundToInt(Vitals.Stamina)),
		                                   FText::AsNumber(FMath::RoundToInt(Vitals.MaxStamina))));
	}
}
