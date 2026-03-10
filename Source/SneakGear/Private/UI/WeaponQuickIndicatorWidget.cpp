#include "UI/WeaponQuickIndicatorWidget.h"

#include "Characters/Player/StealthPlayerCharacter.h"
#include "Components/PlayerItemComponent.h"
#include "Components/TextBlock.h"
#include "Weapon/WeaponBase.h"

void UWeaponQuickIndicatorWidget::UpdateForSlot(EPlayerItemSlot ItemSlot)
{
	const AStealthPlayerCharacter* PlayerCharacter = GetOwningStealthCharacter();
	const UPlayerItemComponent* ItemComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UPlayerItemComponent>() : nullptr;

	if (SlotText)
	{
		SlotText->SetText(GetSlotLabel(ItemSlot));
	}

	AWeaponBase* Weapon = ItemComponent ? ItemComponent->GetWeaponInSlot(ItemSlot) : nullptr;
	if (WeaponNameText)
	{
		if (Weapon)
		{
			const FString WeaponName = Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName();
			WeaponNameText->SetText(FText::FromString(WeaponName));
		}
		else
		{
			WeaponNameText->SetText(NSLOCTEXT("SneakGear", "WeaponQuickIndicatorEmpty", "Empty"));
		}
	}

	if (AmmoText)
	{
		const int32 InClip = ItemComponent ? FMath::Max(ItemComponent->GetInClip(ItemSlot), 0) : 0;
		const int32 ClipSize = ItemComponent ? FMath::Max(ItemComponent->GetClipSize(ItemSlot), 0) : 0;
		const int32 ReserveAmmo = PlayerCharacter ? PlayerCharacter->GetReserveAmmoCount() : 0;

		AmmoText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponQuickIndicatorAmmo", "{0}/{1}  |  Reserve {2}"),
			FText::AsNumber(InClip),
			FText::AsNumber(ClipSize),
			FText::AsNumber(ReserveAmmo)
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

AStealthPlayerCharacter* UWeaponQuickIndicatorWidget::GetOwningStealthCharacter() const
{
	return Cast<AStealthPlayerCharacter>(GetOwningPlayerPawn());
}
