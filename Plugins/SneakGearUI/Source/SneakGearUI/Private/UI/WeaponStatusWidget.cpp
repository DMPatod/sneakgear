#include "UI/WeaponStatusWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Pawn.h"
#include "UI/PlayerUIDataSource.h"

void UWeaponStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WeaponNameText && !FireRateText && !AmmoText)
	{
		if (!WidgetTree)
		{
			return;
		}

		auto Root = WidgetTree->RootWidget;
		if (!Root)
		{
			Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("WeaponRoot"));
			WidgetTree->RootWidget = Root;
		}

		auto* VBox = Cast<UVerticalBox>(Root);
		if (!VBox)
		{
			VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("WeaponRoot"));
			WidgetTree->RootWidget = VBox;
		}

		WeaponNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("WeaponNameText"));
		VBox->AddChild(WeaponNameText);

		FireRateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FireRateText"));
		VBox->AddChild(FireRateText);

		AmmoText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AmmoText"));
		VBox->AddChild(AmmoText);
	}

	TryCachePlayer();
	BindToWeaponStateChanges();
	UpdateFromPlayer();
}

void UWeaponStatusWidget::NativeDestruct()
{
	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIWeaponStateChangedEvent().RemoveAll(this);
	}
	bIsBoundToWeaponState = false;

	Super::NativeDestruct();
}

void UWeaponStatusWidget::HandleInventoryStateChanged()
{
	UpdateFromPlayer();
}

bool UWeaponStatusWidget::TryCachePlayer()
{
	if (CachedPlayer.IsValid())
	{
		return true;
	}

	CachedPlayer = GetOwningPlayerPawn();
	return CachedPlayer.IsValid();
}

void UWeaponStatusWidget::BindToWeaponStateChanges()
{
	if (bIsBoundToWeaponState)
	{
		return;
	}

	if (const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		const_cast<IPlayerUIDataSource*>(PlayerUIDataSource)->OnPlayerUIWeaponStateChangedEvent().AddUObject(
			this, &UWeaponStatusWidget::HandleInventoryStateChanged);
		bIsBoundToWeaponState = true;
	}
}

const IPlayerUIDataSource* UWeaponStatusWidget::GetPlayerUIDataSource() const
{
	return CachedPlayer.IsValid() ? Cast<IPlayerUIDataSource>(CachedPlayer.Get()) : nullptr;
}

void UWeaponStatusWidget::UpdateVisibility(bool bHasWeapon)
{
	SetVisibility((bHideWhenUnarmed && !bHasWeapon) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UWeaponStatusWidget::UpdateFromPlayer()
{
	TryCachePlayer();
	BindToWeaponStateChanges();

	const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();
	if (!PlayerUIDataSource)
	{
		UpdateVisibility(false);
		return;
	}

	FWeaponStatusViewData WeaponStatus;
	if (!PlayerUIDataSource->GetWeaponStatusViewData(WeaponStatus) || !WeaponStatus.bHasWeapon)
	{
		UpdateVisibility(false);
		if (WeaponNameText)
		{
			WeaponNameText->SetText(NSLOCTEXT("SneakGear", "WeaponNone", "Weapon: None"));
		}
		if (FireRateText)
		{
			FireRateText->SetText(FText::GetEmpty());
		}
		if (AmmoText)
		{
			AmmoText->SetText(FText::GetEmpty());
		}
		return;
	}

	UpdateVisibility(true);

	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponNameFmt", "Weapon: {0}"),
		                                      WeaponStatus.WeaponName));
	}

	if (FireRateText)
	{
		FireRateText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponRateFmt", "Fire Rate: {0}"),
		                                    FText::AsNumber(WeaponStatus.FireRate)));
	}

	if (AmmoText)
	{
		AmmoText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponAmmoFmt", "Ammo: {0} | Clip: {1}/{2}"),
		                                FText::AsNumber(WeaponStatus.ReserveAmmo),
		                                FText::AsNumber(WeaponStatus.InClip),
		                                FText::AsNumber(WeaponStatus.ClipSize)));
	}
}
