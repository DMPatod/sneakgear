#include "UI/PlayerVitalsWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "UI/PlayerUIDataSource.h"

void UPlayerVitalsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!CachedPlayer.IsValid())
	{
		CachedPlayer = GetOwningPlayerPawn();
	}
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

void UPlayerVitalsWidget::SetObservedPlayer(APawn* InPawn)
{
	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIVitalsChangedEvent().RemoveAll(this);
	}

	CachedPlayer = InPawn;

	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIVitalsChangedEvent().AddUObject(
			this, &UPlayerVitalsWidget::HandleVitalsChanged);
	}

	UpdateFromPlayer();
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
