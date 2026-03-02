#include "UI/StealthPlayerDebugWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/Player/StealthPlayerCharacter.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Weapon/WeaponBase.h"

void UStealthPlayerDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!DebugText && WidgetTree)
	{
		auto* RootWidget = WidgetTree->RootWidget;
		if (!RootWidget)
		{
			RootWidget = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DebugRoot"));
			WidgetTree->RootWidget = RootWidget;
		}

		UVerticalBox* VerticalBox = Cast<UVerticalBox>(RootWidget);
		if (!VerticalBox)
		{
			VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DebugRoot"));
			WidgetTree->RootWidget = VerticalBox;
		}

		if (VerticalBox)
		{
			DebugText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DebugText"));
			VerticalBox->AddChild(DebugText);
		}
	}

	TryCachePlayer();
	UpdateDebugText();
}

void UStealthPlayerDebugWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TryCachePlayer();
	UpdateDebugText();
}

bool UStealthPlayerDebugWidget::TryCachePlayer()
{
	if (CachedPlayer.IsValid())
	{
		return true;
	}

	CachedPlayer = Cast<AStealthPlayerCharacter>(GetOwningPlayerPawn());
	return CachedPlayer.IsValid();
}

void UStealthPlayerDebugWidget::UpdateDebugText()
{
	if (!DebugText)
	{
		return;
	}

	AStealthPlayerCharacter* Player = CachedPlayer.Get();
	if (!Player)
	{
		DebugText->SetText(NSLOCTEXT("SneakGear", "StealthDebugNoPlayer", "Stealth Debug\nPlayer: None"));
		return;
	}

	const bool bInCover = Player->IsInCover();
	const float CoverMoveAxis = Player->GetCoverMoveAxis();
	const bool bAiming = Player->IsAiming();
	const bool bFiredRecently = Player->WasWeaponFiredRecently();
	const EPlayerItemSlot ActiveSlot = Player->GetActiveWeaponSlot();
	const int32 InClip = Player->GetActiveWeaponInClip();
	const int32 ClipSize = Player->GetActiveWeaponClipSize();
	const int32 ReserveAmmo = Player->GetReserveAmmoCount();
	const float Speed2D = Player->GetVelocity().Size2D();

	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	const FString SlotLabel = SlotEnum
		                          ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(ActiveSlot)).ToString()
		                          : TEXT("Unknown");

	FString WeaponName = TEXT("None");
	if (const AWeaponBase* Weapon = Player->GetCurrentWeapon())
	{
		WeaponName = Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName();
	}

	const FString DebugString = FString::Printf(
		TEXT("Stealth Debug\n")
		TEXT("Aiming: %s\n")
		TEXT("InCover: %s\n")
		TEXT("CoverMoveAxis: %.2f\n")
		TEXT("WeaponSlot: %s\n")
		TEXT("Weapon: %s\n")
		TEXT("Ammo: %d  Clip: %d/%d\n")
		TEXT("FiredRecently: %s\n")
		TEXT("Speed2D: %.1f"),
		bAiming ? TEXT("true") : TEXT("false"),
		bInCover ? TEXT("true") : TEXT("false"),
		CoverMoveAxis,
		*SlotLabel,
		*WeaponName,
		ReserveAmmo,
		InClip,
		ClipSize,
		bFiredRecently ? TEXT("true") : TEXT("false"),
		Speed2D
	);

	DebugText->SetText(FText::FromString(DebugString));
}
