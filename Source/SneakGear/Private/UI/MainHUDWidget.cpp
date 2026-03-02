#include "UI/MainHUDWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "UI/EventFeedWidget.h"
#include "UI/PlayerVitalsWidget.h"
#include "UI/RadarWidget.h"
#include "UI/StealthPlayerDebugWidget.h"
#include "UI/StanceWidget.h"
#include "UI/WeaponStatusWidget.h"

void UMainHUDWidget::NativeConstruct()
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
	else if (!RuntimeStealthDebugWidget && GetOwningPlayer())
	{
		RuntimeStealthDebugWidget = CreateWidget<UStealthPlayerDebugWidget>(
			GetOwningPlayer(),
			UStealthPlayerDebugWidget::StaticClass()
		);

		if (RuntimeStealthDebugWidget)
		{
			RuntimeStealthDebugWidget->AddToViewport(StealthDebugZOrder);
		}
	}
}

void UMainHUDWidget::NativeDestruct()
{
	if (RuntimeStealthDebugWidget)
	{
		RuntimeStealthDebugWidget->RemoveFromParent();
		RuntimeStealthDebugWidget = nullptr;
	}

	Super::NativeDestruct();
}
