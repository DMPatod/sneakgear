#include "UI/WeaponQuickIndicatorWidget.h"

#include "Components/TextBlock.h"
#include "UI/PlayerUIDataSource.h"

void UWeaponQuickIndicatorWidget::UpdateForSlot(EPlayerItemSlot ItemSlot)
{
	const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();
	FWeaponQuickSlotViewData SlotData;
	const bool bHasSlotData = PlayerUIDataSource && PlayerUIDataSource->GetWeaponQuickSlotViewData(ItemSlot, SlotData);

	if (SlotText)
	{
		SlotText->SetText(GetSlotLabel(ItemSlot));
	}

	if (WeaponNameText)
	{
		WeaponNameText->SetText(bHasSlotData ? SlotData.WeaponName : NSLOCTEXT("SneakGear", "WeaponQuickIndicatorEmpty", "Empty"));
	}

	if (AmmoText)
	{
		AmmoText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponQuickIndicatorAmmo", "{0}/{1}  |  Reserve {2}"),
			FText::AsNumber(bHasSlotData ? SlotData.InClip : 0),
			FText::AsNumber(bHasSlotData ? SlotData.ClipSize : 0),
			FText::AsNumber(bHasSlotData ? SlotData.ReserveAmmo : 0)
		));
	}
}

FText UWeaponQuickIndicatorWidget::GetSlotLabel(EPlayerItemSlot Slot) const
{
	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	if (!SlotEnum)
	{
		return FText::GetEmpty();
	}

	return SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(Slot));
}

const IPlayerUIDataSource* UWeaponQuickIndicatorWidget::GetPlayerUIDataSource() const
{
	return Cast<IPlayerUIDataSource>(GetOwningPlayerPawn());
}
