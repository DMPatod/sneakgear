#include "UI/PlayerDebugWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "UI/PlayerUIDataSource.h"

void UPlayerDebugWidget::NativeConstruct()
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
		World->GetTimerManager().SetTimer(RefreshTimer, this, &UPlayerDebugWidget::UpdateDebugText, 0.1f, true);
	}
}

void UPlayerDebugWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
	}

	Super::NativeDestruct();
}

bool UPlayerDebugWidget::TryCachePlayer()
{
	if (CachedPlayer.IsValid())
	{
		return true;
	}

	CachedPlayer = GetOwningPlayerPawn();
	return CachedPlayer.IsValid();
}

const IPlayerUIDataSource* UPlayerDebugWidget::GetPlayerUIDataSource() const
{
	return CachedPlayer.IsValid() ? Cast<IPlayerUIDataSource>(CachedPlayer.Get()) : nullptr;
}

void UPlayerDebugWidget::UpdateDebugText()
{
	TryCachePlayer();

	if (!DebugText)
	{
		return;
	}

	const IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();
	FStealthDebugViewData DebugData;
	if (!PlayerUIDataSource || !PlayerUIDataSource->GetStealthDebugViewData(DebugData) || !DebugData.bHasPlayer)
	{
		DebugText->SetText(NSLOCTEXT("SneakGear", "StealthDebugNoPlayer", "Stealth Debug\nPlayer: None"));
		return;
	}

	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	const UEnum* StanceEnum = StaticEnum<EStance>();
	const FString SlotLabel = SlotEnum
		                          ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(DebugData.ActiveWeaponSlot)).ToString()
		                          : TEXT("Unknown");
	const FString StanceLabel = StanceEnum
		                            ? StanceEnum->GetDisplayNameTextByValue(static_cast<int64>(DebugData.Stance)).ToString()
		                            : TEXT("Unknown");
	const FString WeaponName = DebugData.WeaponName.IsEmpty() ? TEXT("None") : DebugData.WeaponName.ToString();

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
		DebugData.bAiming ? TEXT("true") : TEXT("false"),
		*StanceLabel,
		DebugData.bInCover ? TEXT("true") : TEXT("false"),
		DebugData.bVaulting ? TEXT("true") : TEXT("false"),
		DebugData.CoverMoveAxis,
		*SlotLabel,
		*WeaponName,
		DebugData.ReserveAmmo,
		DebugData.InClip,
		DebugData.ClipSize,
		DebugData.bFiredRecently ? TEXT("true") : TEXT("false"),
		DebugData.Speed2D
	);

	DebugText->SetText(FText::FromString(DebugString));
}
