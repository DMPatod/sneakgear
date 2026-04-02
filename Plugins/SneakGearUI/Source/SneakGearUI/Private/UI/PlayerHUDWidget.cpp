#include "UI/PlayerHUDWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/EventFeedWidget.h"
#include "UI/PlayerVitalsNativeWidget.h"
#include "UI/PlayerVitalsWidget.h"
#include "UI/RadarWidget.h"
#include "UI/PlayerDebugWidget.h"
#include "UI/StanceWidget.h"
#include "UI/WeaponStatusWidget.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto* RadarSlot = RadarWidget ? Cast<UCanvasPanelSlot>(RadarWidget->Slot) : nullptr)
	{
		RadarSlot->SetZOrder(RadarZOrder);
	}

	if (auto* VitalsSlot = PlayerVitalsWidget ? Cast<UCanvasPanelSlot>(PlayerVitalsWidget->Slot) : nullptr)
	{
		VitalsSlot->SetZOrder(PlayerVitalsZOrder);
	}

	if (auto* WeaponSlot = WeaponStatusWidget ? Cast<UCanvasPanelSlot>(WeaponStatusWidget->Slot) : nullptr)
	{
		WeaponSlot->SetZOrder(WeaponStatusZOrder);
	}

	if (auto* EventSlot = EventFeedWidget ? Cast<UCanvasPanelSlot>(EventFeedWidget->Slot) : nullptr)
	{
		EventSlot->SetZOrder(EventFeedZOrder);
	}
	
	if (auto* StanceSlot = StanceWidget ? Cast<UCanvasPanelSlot>(StanceWidget->Slot) : nullptr)
	{
		StanceSlot->SetZOrder(StanceZOrder);
	}

	if (auto* DebugSlot = StealthDebugWidget ? Cast<UCanvasPanelSlot>(StealthDebugWidget->Slot) : nullptr)
	{
		DebugSlot->SetZOrder(StealthDebugZOrder);
	}

	ApplyDisplaySettings();
}

void UPlayerHUDWidget::NativeDestruct()
{
	if (RuntimePlayerVitalsWidget)
	{
		RuntimePlayerVitalsWidget->RemoveFromParent();
		RuntimePlayerVitalsWidget = nullptr;
	}

	if (RuntimeStealthDebugWidget)
	{
		RuntimeStealthDebugWidget->RemoveFromParent();
		RuntimeStealthDebugWidget = nullptr;
	}

	Super::NativeDestruct();
}

void UPlayerHUDWidget::ApplyDisplaySettings()
{
	SetWidgetVisible(RadarWidget, DisplaySettings.bShowRadar);
	SetWidgetVisible(PlayerVitalsWidget, DisplaySettings.bShowPlayerVitals);
	SetWidgetVisible(WeaponStatusWidget, DisplaySettings.bShowWeaponStatus);
	SetWidgetVisible(EventFeedWidget, DisplaySettings.bShowEventFeed);
	SetWidgetVisible(StanceWidget, DisplaySettings.bShowStance);
	SetWidgetVisible(StealthDebugWidget, DisplaySettings.bShowStealthDebug);

	if (StealthDebugWidget)
	{
		if (RuntimeStealthDebugWidget)
		{
			RuntimeStealthDebugWidget->RemoveFromParent();
			RuntimeStealthDebugWidget = nullptr;
		}
	}

	if (PlayerVitalsWidget)
	{
		if (RuntimePlayerVitalsWidget)
		{
			RuntimePlayerVitalsWidget->RemoveFromParent();
			RuntimePlayerVitalsWidget = nullptr;
		}
	}
	else if (DisplaySettings.bShowPlayerVitals)
	{
		if (!RuntimePlayerVitalsWidget && GetOwningPlayer())
		{
			RuntimePlayerVitalsWidget = CreateWidget<UPlayerVitalsWidget>(
				GetOwningPlayer(),
				UPlayerVitalsNativeWidget::StaticClass()
			);

			if (RuntimePlayerVitalsWidget)
			{
				RuntimePlayerVitalsWidget->AddToViewport(PlayerVitalsZOrder);
			}
		}
	}
	else if (RuntimePlayerVitalsWidget)
	{
		RuntimePlayerVitalsWidget->RemoveFromParent();
		RuntimePlayerVitalsWidget = nullptr;
	}

	if (DisplaySettings.bShowStealthDebug)
	{
		if (!RuntimeStealthDebugWidget && GetOwningPlayer())
		{
			RuntimeStealthDebugWidget = CreateWidget<UPlayerDebugWidget>(
				GetOwningPlayer(),
				UPlayerDebugWidget::StaticClass()
			);

			if (RuntimeStealthDebugWidget)
			{
				RuntimeStealthDebugWidget->AddToViewport(StealthDebugZOrder);
			}
		}
	}
	else if (RuntimeStealthDebugWidget)
	{
		RuntimeStealthDebugWidget->RemoveFromParent();
		RuntimeStealthDebugWidget = nullptr;
	}
}

void UPlayerHUDWidget::SetWidgetVisible(UWidget* Widget, bool bVisible) const
{
	if (!Widget)
	{
		return;
	}

	Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
