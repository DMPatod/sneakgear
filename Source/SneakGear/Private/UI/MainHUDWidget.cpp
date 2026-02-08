#include "UI/MainHUDWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "UI/PlayerVitalsWidget.h"
#include "UI/RadarWidget.h"
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
}
