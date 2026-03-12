#include "UI/StealthPlayerDebugWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Player/StealthPlayerCharacter.h"
#include "Player/Components/PlayerItemComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "TimerManager.h"
#include "Weapon/WeaponBase.h"

void UStealthPlayerDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!DebugText && WidgetTree)
	{
		auto RootWidget = WidgetTree->RootWidget;
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

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RefreshTimer, this, &UStealthPlayerDebugWidget::UpdateDebugText, 0.1f, true);
	}
}

void UStealthPlayerDebugWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
	}

	Super::NativeDestruct();
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
	TryCachePlayer();

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
	const bool bVaulting = Player->IsVaulting();
	const float CoverMoveAxis = Player->GetCoverMoveAxis();
	const bool bAiming = Player->IsAiming();
	const UPlayerItemComponent* ItemComponent = Player->GetItemComponent();
	const bool bFiredRecently = ItemComponent && ItemComponent->WasActiveWeaponFiredRecently();
	const EStance CurrentStance = Player->Stance;
	const EPlayerItemSlot ActiveSlot = ItemComponent ? ItemComponent->GetActiveWeaponSlot() : EPlayerItemSlot::PrimaryWeapon;
	const int32 InClip = ItemComponent ? ItemComponent->GetActiveWeaponInClip() : 0;
	const int32 ClipSize = ItemComponent ? ItemComponent->GetActiveWeaponClipSize() : 0;
	const int32 ReserveAmmo = ItemComponent ? ItemComponent->GetReserveAmmoCount() : 0;
	const float Speed2D = Player->GetVelocity().Size2D();

	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	const UEnum* StanceEnum = StaticEnum<EStance>();
	const FString SlotLabel = SlotEnum
		                          ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(ActiveSlot)).ToString()
		                          : TEXT("Unknown");
	const FString StanceLabel = StanceEnum
		                            ? StanceEnum->GetDisplayNameTextByValue(static_cast<int64>(CurrentStance)).ToString()
		                            : TEXT("Unknown");

	FString WeaponName = TEXT("None");
	if (const AWeaponBase* Weapon = Player->GetCurrentWeapon())
	{
		WeaponName = Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName();
	}

	const FString DebugString = FString::Printf(
		TEXT("Stealth Debug\n")
		TEXT("Aiming: %s\n")
		TEXT("Stance: %s\n")
		TEXT("InCover: %s\n")
		TEXT("Vaulting: %s\n")
		TEXT("CoverMoveAxis: %.2f\n")
		TEXT("WeaponSlot: %s\n")
		TEXT("Weapon: %s\n")
		TEXT("Ammo: %d  Clip: %d/%d\n")
		TEXT("FiredRecently: %s\n")
		TEXT("Speed2D: %.1f"),
		bAiming ? TEXT("true") : TEXT("false"),
		*StanceLabel,
		bInCover ? TEXT("true") : TEXT("false"),
		bVaulting ? TEXT("true") : TEXT("false"),
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
