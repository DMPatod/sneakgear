#include "UI/WeaponStatusWidget.h"

#include "Characters/ThirdPersonPlayerCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Weapon/WeaponBase.h"

void UWeaponStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WeaponNameText || FireRateText)
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
}
