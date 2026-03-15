#include "UI/WeaponSelectionMenuWidget.h"

#include "UI/WeaponQuickIndicatorWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "UI/PlayerUIDataSource.h"
#include "UI/WeaponMenuActions.h"

FText UWeaponSelectionMenuWidget::BuildInventoryListText(const IPlayerUIDataSource* PlayerUIDataSource, EPlayerItemSlot Slot,
                                                         const FText& EmptyFallback, const FText& LabelFormat,
                                                         const FText& ActiveLabelFormat) const
{
	if (!PlayerUIDataSource)
	{
		return EmptyFallback;
	}

	const int32 ItemCount = PlayerUIDataSource->GetInventoryItemCount(Slot);
	if (ItemCount <= 0)
	{
		return EmptyFallback;
	}

	const int32 ActiveIndex = PlayerUIDataSource->GetActiveInventoryItemIndex(Slot);
	FString JoinedEntries;

	for (int32 Index = 0; Index < ItemCount; ++Index)
	{
		const FText ItemName = PlayerUIDataSource->GetInventoryItemDisplayNameAt(Slot, Index);
		if (ItemName.IsEmpty())
		{
			continue;
		}

		const bool bIsActive = Index == ActiveIndex;
		const FText EntryText = FText::Format(
			bIsActive ? ActiveLabelFormat : LabelFormat,
			FText::AsNumber(Index + 1),
			ItemName
		);

		if (!JoinedEntries.IsEmpty())
		{
			JoinedEntries += TEXT("\n");
		}
		JoinedEntries += EntryText.ToString();
	}

	return JoinedEntries.IsEmpty() ? EmptyFallback : FText::FromString(JoinedEntries);
}

void UWeaponSelectionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	if (PrimaryWeaponButton)
	{
		PrimaryWeaponButton->OnClicked.RemoveDynamic(this, &UWeaponSelectionMenuWidget::HandlePrimaryWeaponClicked);
		PrimaryWeaponButton->OnClicked.AddDynamic(this, &UWeaponSelectionMenuWidget::HandlePrimaryWeaponClicked);
	}

	if (SecondaryWeaponButton)
	{
		SecondaryWeaponButton->OnClicked.RemoveDynamic(this, &UWeaponSelectionMenuWidget::HandleSecondaryWeaponClicked);
		SecondaryWeaponButton->OnClicked.AddDynamic(this, &UWeaponSelectionMenuWidget::HandleSecondaryWeaponClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.RemoveDynamic(this, &UWeaponSelectionMenuWidget::HandleCancelClicked);
		CancelButton->OnClicked.AddDynamic(this, &UWeaponSelectionMenuWidget::HandleCancelClicked);
	}
}

void UWeaponSelectionMenuWidget::InitializeMenu(EPlayerItemSlot InitialSlot)
{
	if (InitialSlot == EPlayerItemSlot::PrimaryWeapon || InitialSlot == EPlayerItemSlot::SecondaryWeapon)
	{
		FocusedSelectionSlot = InitialSlot;
	}

	RefreshEntries(InitialSlot);
	ApplyFocusToCurrentSlot();
}

FReply UWeaponSelectionMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	const bool bIsPrevKey = PressedKey == EKeys::Left || PressedKey == EKeys::Up ||
		PressedKey == EKeys::Gamepad_DPad_Left || PressedKey == EKeys::Gamepad_DPad_Up ||
		PressedKey == EKeys::Gamepad_LeftShoulder;

	const bool bIsNextKey = PressedKey == EKeys::Right || PressedKey == EKeys::Down ||
		PressedKey == EKeys::Gamepad_DPad_Right || PressedKey == EKeys::Gamepad_DPad_Down ||
		PressedKey == EKeys::Gamepad_RightShoulder;

	if (bIsPrevKey || bIsNextKey)
	{
		SetFocusedSlot(FocusedSelectionSlot == EPlayerItemSlot::PrimaryWeapon
			               ? EPlayerItemSlot::SecondaryWeapon
			               : EPlayerItemSlot::PrimaryWeapon);
		return FReply::Handled();
	}

	if (PressedKey == EKeys::Enter || PressedKey == EKeys::Gamepad_FaceButton_Bottom)
	{
		ConfirmCurrentSelection();
		return FReply::Handled();
	}

	if (PressedKey == EKeys::Escape || PressedKey == EKeys::Gamepad_FaceButton_Right)
	{
		HandleCancelClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWeaponSelectionMenuWidget::RefreshEntries(EPlayerItemSlot InitialSlot) const
{
	const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();

	if (HeaderText)
	{
		const FText FocusSlotText = InitialSlot == EPlayerItemSlot::SecondaryWeapon
			                            ? NSLOCTEXT("SneakGear", "WeaponSelectionInitialSecondary", "Secondary")
			                            : NSLOCTEXT("SneakGear", "WeaponSelectionInitialPrimary", "Primary");

		HeaderText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponSelectionHeader", "Select Weapon (Focus: {0})"),
			FocusSlotText
		));
	}

	if (EquippedItemText)
	{
		EquippedItemText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponSelectionEquippedItemFmt", "Equipped Items:\n{0}"),
			BuildInventoryListText(
				PlayerUIDataSource,
				EPlayerItemSlot::Equipped,
				NSLOCTEXT("SneakGear", "WeaponSelectionNoEquippedItem", "None"),
				NSLOCTEXT("SneakGear", "WeaponSelectionListEntry", "{0}. {1}"),
				NSLOCTEXT("SneakGear", "WeaponSelectionActiveListEntry", "{0}. {1} [Active]")
			)
		));
	}

	if (SupportItemText)
	{
		SupportItemText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponSelectionSupportItemFmt", "Support Items:\n{0}"),
			BuildInventoryListText(
				PlayerUIDataSource,
				EPlayerItemSlot::Support,
				NSLOCTEXT("SneakGear", "WeaponSelectionNoSupportItem", "None"),
				NSLOCTEXT("SneakGear", "WeaponSelectionListEntry", "{0}. {1}"),
				NSLOCTEXT("SneakGear", "WeaponSelectionActiveListEntry", "{0}. {1} [Active]")
			)
		));
	}

	if (UtilityItemText)
	{
		UtilityItemText->SetText(FText::Format(
			NSLOCTEXT("SneakGear", "WeaponSelectionUtilityItemFmt", "Utility Items:\n{0}"),
			BuildInventoryListText(
				PlayerUIDataSource,
				EPlayerItemSlot::Utility,
				NSLOCTEXT("SneakGear", "WeaponSelectionNoUtilityItem", "None"),
				NSLOCTEXT("SneakGear", "WeaponSelectionListEntry", "{0}. {1}"),
				NSLOCTEXT("SneakGear", "WeaponSelectionActiveListEntry", "{0}. {1} [Active]")
			)
		));
	}

	if (PrimaryWeaponInfoWidget)
	{
		PrimaryWeaponInfoWidget->UpdateForSlot(EPlayerItemSlot::PrimaryWeapon);
	}

	if (SecondaryWeaponInfoWidget)
	{
		SecondaryWeaponInfoWidget->UpdateForSlot(EPlayerItemSlot::SecondaryWeapon);
	}
}

const IPlayerUIDataSource* UWeaponSelectionMenuWidget::GetPlayerUIDataSource() const
{
	return Cast<IPlayerUIDataSource>(GetOwningPlayerPawn());
}

IWeaponMenuActions* UWeaponSelectionMenuWidget::GetWeaponMenuActions() const
{
	return Cast<IWeaponMenuActions>(GetOwningPlayer());
}

void UWeaponSelectionMenuWidget::HandlePrimaryWeaponClicked()
{
	SetFocusedSlot(EPlayerItemSlot::PrimaryWeapon);

	if (IWeaponMenuActions* WeaponMenuActions = GetWeaponMenuActions())
	{
		WeaponMenuActions->SelectWeaponFromSelectionMenu(EPlayerItemSlot::PrimaryWeapon);
	}
}

void UWeaponSelectionMenuWidget::HandleSecondaryWeaponClicked()
{
	SetFocusedSlot(EPlayerItemSlot::SecondaryWeapon);

	if (IWeaponMenuActions* WeaponMenuActions = GetWeaponMenuActions())
	{
		WeaponMenuActions->SelectWeaponFromSelectionMenu(EPlayerItemSlot::SecondaryWeapon);
	}
}

void UWeaponSelectionMenuWidget::HandleCancelClicked()
{
	if (IWeaponMenuActions* WeaponMenuActions = GetWeaponMenuActions())
	{
		WeaponMenuActions->CancelWeaponSelectionMenu();
	}
}

void UWeaponSelectionMenuWidget::SetFocusedSlot(EPlayerItemSlot NewFocusedSlot)
{
	if (NewFocusedSlot != EPlayerItemSlot::PrimaryWeapon && NewFocusedSlot != EPlayerItemSlot::SecondaryWeapon)
	{
		return;
	}

	FocusedSelectionSlot = NewFocusedSlot;
	ApplyFocusToCurrentSlot();
}

void UWeaponSelectionMenuWidget::ApplyFocusToCurrentSlot() const
{
	if (FocusedSelectionSlot == EPlayerItemSlot::PrimaryWeapon)
	{
		if (PrimaryWeaponButton)
		{
			PrimaryWeaponButton->SetKeyboardFocus();
		}
		return;
	}

	if (SecondaryWeaponButton)
	{
		SecondaryWeaponButton->SetKeyboardFocus();
	}
}

void UWeaponSelectionMenuWidget::ConfirmCurrentSelection()
{
	if (FocusedSelectionSlot == EPlayerItemSlot::PrimaryWeapon)
	{
		HandlePrimaryWeaponClicked();
		return;
	}

	HandleSecondaryWeaponClicked();
}
