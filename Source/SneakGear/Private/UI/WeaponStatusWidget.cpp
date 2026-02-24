#include "UI/WeaponStatusWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/Player/StealthPlayerCharacter.h"
#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Components/PlayerWeaponComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Weapon/WeaponBase.h"

void UWeaponStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WeaponNameText || FireRateText || AmmoText)
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

void UWeaponStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedPlayer.IsValid())
	{
		CachedPlayer = Cast<AThirdPersonPlayerCharacter>(GetOwningPlayerPawn());
	}

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
		InClip = StealthPlayer->GetActiveWeaponInClip();
		ClipSize = StealthPlayer->GetActiveWeaponClipSize();
		ReserveAmmo = StealthPlayer->GetReserveAmmoCount();
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
