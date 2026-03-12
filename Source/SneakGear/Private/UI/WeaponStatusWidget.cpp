#include "UI/WeaponStatusWidget.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Player/Components/PlayerItemComponent.h"
#include "Player/StealthPlayerCharacter.h"
#include "Player/ThirdPersonPlayerCharacter.h"
#include "Player/Components/PlayerWeaponComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Game/GAS/AmmoAttributeSet.h"
#include "Weapon/WeaponBase.h"

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

	CachedPlayer = Cast<AThirdPersonPlayerCharacter>(GetOwningPlayerPawn());
	if (AThirdPersonPlayerCharacter* Player = CachedPlayer.Get())
	{
		if (AStealthPlayerCharacter* StealthPlayer = Cast<AStealthPlayerCharacter>(Player))
		{
			if (UPlayerItemComponent* ItemComponent = StealthPlayer->GetItemComponent())
			{
				ItemComponent->OnInventoryStateChangedEvent().AddUObject(this, &UWeaponStatusWidget::HandleInventoryStateChanged);
			}
		}

		if (UAbilitySystemComponent* AbilitySystem = Player->GetAbilitySystemComponent())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetAmmoAttribute()).AddUObject(
				this, &UWeaponStatusWidget::HandleAmmoChanged);
		}
	}

	UpdateFromPlayer();
}

void UWeaponStatusWidget::NativeDestruct()
{
	if (AThirdPersonPlayerCharacter* Player = CachedPlayer.Get())
	{
		if (AStealthPlayerCharacter* StealthPlayer = Cast<AStealthPlayerCharacter>(Player))
		{
			if (UPlayerItemComponent* ItemComponent = StealthPlayer->GetItemComponent())
			{
				ItemComponent->OnInventoryStateChangedEvent().RemoveAll(this);
			}
		}

		if (UAbilitySystemComponent* AbilitySystem = Player->GetAbilitySystemComponent())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetAmmoAttribute()).RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void UWeaponStatusWidget::HandleInventoryStateChanged()
{
	UpdateFromPlayer();
}

void UWeaponStatusWidget::HandleAmmoChanged(const FOnAttributeChangeData& Data)
{
	(void)Data;
	UpdateFromPlayer();
}

void UWeaponStatusWidget::UpdateFromPlayer()
{
	auto* Player = CachedPlayer.Get();
	if (!Player)
	{
		return;
	}

	auto* Weapon = Player->GetCurrentWeapon();
	if (!Weapon)
	{
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

	const FString WeaponName = Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName();

	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponNameFmt", "Weapon: {0}"),
		                                      FText::FromString(WeaponName)));
	}

	if (FireRateText)
	{
		FireRateText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponRateFmt", "Fire Rate: {0}"),
		                                    FText::AsNumber(Weapon->GetFireRate())));
	}

	int32 InClip = 0;
	int32 ClipSize = 0;
	int32 ReserveAmmo = FMath::Max(FMath::FloorToInt(Player->GetAmmo()), 0);

	if (const auto* StealthPlayer = Cast<AStealthPlayerCharacter>(Player))
	{
		if (const UPlayerItemComponent* ItemComponent = StealthPlayer->GetItemComponent())
		{
			InClip = ItemComponent->GetActiveWeaponInClip();
			ClipSize = ItemComponent->GetActiveWeaponClipSize();
			ReserveAmmo = ItemComponent->GetReserveAmmoCount();
		}
	}
	else
	{
		const auto* PlayerWeaponComponent = Player->FindComponentByClass<UPlayerWeaponComponent>();
		InClip = PlayerWeaponComponent ? FMath::Max(PlayerWeaponComponent->GetInClip(), 0) : 0;
		ClipSize = PlayerWeaponComponent ? FMath::Max(PlayerWeaponComponent->GetClipSize(), 0) : 0;
	}

	if (AmmoText)
	{
		AmmoText->SetText(FText::Format(NSLOCTEXT("SneakGear", "WeaponAmmoFmt", "Ammo: {0} | Clip: {1}/{2}"),
		                                FText::AsNumber(ReserveAmmo),
		                                FText::AsNumber(InClip),
		                                FText::AsNumber(ClipSize)));
	}
}
