#include "UI/PlayerVitalsWidget.h"

#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GAS/HealthAttributeSet.h"
#include "GAS/StaminaAttributeSet.h"

void UPlayerVitalsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HealthBar || StaminaBar || HealthText || StaminaText)
	{
		return;
	}

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

void UPlayerVitalsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedPlayer.IsValid())
	{
		CachedPlayer = Cast<AThirdPersonPlayerCharacter>(GetOwningPlayerPawn());
	}

	UpdateFromPlayer();
}

void UPlayerVitalsWidget::UpdateFromPlayer()
{
	auto* Player = CachedPlayer.Get();
	if (!Player)
	{
		return;
	}

	const auto* HealthSet = Player->GetHealthSet();
	const auto* StaminaSet = Player->GetStaminaSet();

	const float Health = HealthSet ? HealthSet->GetHealth() : 0.f;
	const float MaxHealth = HealthSet ? HealthSet->GetMaxHealth() : 0.f;
	const float Stamina = StaminaSet ? StaminaSet->GetStamina() : 0.f;
	const float MaxStamina = StaminaSet ? StaminaSet->GetMaxStamina() : 0.f;

	if (HealthBar)
	{
		HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	}

	if (StaminaBar)
	{
		StaminaBar->SetPercent(MaxStamina > 0.f ? Stamina / MaxStamina : 0.f);
	}

	if (HealthText)
	{
		HealthText->SetText(FText::Format(NSLOCTEXT("SneakGear", "HealthTextFmt", "Health: {0} / {1}"),
		                                  FText::AsNumber(FMath::RoundToInt(Health)),
		                                  FText::AsNumber(FMath::RoundToInt(MaxHealth))));
	}

	if (StaminaText)
	{
		StaminaText->SetText(FText::Format(NSLOCTEXT("SneakGear", "StaminaTextFmt", "Stamina: {0} / {1}"),
		                                   FText::AsNumber(FMath::RoundToInt(Stamina)),
		                                   FText::AsNumber(FMath::RoundToInt(MaxStamina))));
	}
}
