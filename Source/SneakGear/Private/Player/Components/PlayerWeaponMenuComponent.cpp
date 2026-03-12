#include "Player/Components/PlayerWeaponMenuComponent.h"

#include "Player/Components/PlayerItemComponent.h"
#include "Player/StealthPlayerCharacter.h"
#include "Player/StealthPlayerController.h"
#include "TimerManager.h"
#include "UI/WeaponQuickIndicatorWidget.h"
#include "UI/WeaponSelectionMenuWidget.h"

UPlayerWeaponMenuComponent::UPlayerWeaponMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerWeaponMenuComponent::Initialize(TSubclassOf<UWeaponQuickIndicatorWidget> InWeaponQuickIndicatorWidgetClass,
                                            TSubclassOf<UWeaponSelectionMenuWidget> InWeaponSelectionWidgetClass,
                                            float InWeaponQuickIndicatorDuration)
{
	WeaponQuickIndicatorWidgetClass = InWeaponQuickIndicatorWidgetClass;
	WeaponSelectionWidgetClass = InWeaponSelectionWidgetClass;
	WeaponQuickIndicatorDuration = InWeaponQuickIndicatorDuration;
}

void UPlayerWeaponMenuComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerWeaponMenuComponent::ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot)
{
	AStealthPlayerController* Controller = GetOwningStealthPlayerController();
	if (!Controller)
	{
		return;
	}

	Controller->BP_OnWeaponQuickSelectIndicator(Slot);

	if (!WeaponQuickIndicatorWidget && WeaponQuickIndicatorWidgetClass)
	{
		WeaponQuickIndicatorWidget = CreateWidget<UWeaponQuickIndicatorWidget>(Controller, WeaponQuickIndicatorWidgetClass);
	}

	if (WeaponQuickIndicatorWidget)
	{
		WeaponQuickIndicatorWidget->UpdateForSlot(Slot);
		WeaponQuickIndicatorWidget->AddToViewport();
	}

	GetWorld()->GetTimerManager().ClearTimer(WeaponQuickIndicatorTimer);
	GetWorld()->GetTimerManager().SetTimer(
		WeaponQuickIndicatorTimer,
		[this]()
		{
			if (WeaponQuickIndicatorWidget)
			{
				WeaponQuickIndicatorWidget->RemoveFromParent();
			}
		},
		FMath::Max(WeaponQuickIndicatorDuration, 0.05f),
		false
	);
}

void UPlayerWeaponMenuComponent::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	AStealthPlayerController* Controller = GetOwningStealthPlayerController();
	if (!Controller || bWeaponSelectionOpen)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(WeaponQuickIndicatorTimer);
	if (WeaponQuickIndicatorWidget)
	{
		WeaponQuickIndicatorWidget->RemoveFromParent();
	}

	bWeaponSelectionOpen = true;
	bWasGamePausedBeforeWeaponSelection = Controller->IsPaused();
	Controller->SetPause(true);

	if (!WeaponSelectionWidget && WeaponSelectionWidgetClass)
	{
		WeaponSelectionWidget = CreateWidget<UWeaponSelectionMenuWidget>(Controller, WeaponSelectionWidgetClass);
	}

	if (WeaponSelectionWidget)
	{
		WeaponSelectionWidget->AddToViewport(100);
		WeaponSelectionWidget->InitializeMenu(InitialSlot);
	}

	FInputModeGameAndUI InputMode;
	(void)InputMode;
	Controller->ApplyMenuInputMode(WeaponSelectionWidget);
	Controller->BP_OnWeaponSelectionOpened(InitialSlot);
}

void UPlayerWeaponMenuComponent::CloseWeaponSelectionWidget()
{
	AStealthPlayerController* Controller = GetOwningStealthPlayerController();
	if (!Controller || !bWeaponSelectionOpen)
	{
		return;
	}

	bWeaponSelectionOpen = false;

	if (WeaponSelectionWidget)
	{
		WeaponSelectionWidget->RemoveFromParent();
	}

	if (!bWasGamePausedBeforeWeaponSelection)
	{
		Controller->SetPause(false);
	}

	Controller->ApplyGameplayInputMode();
	Controller->BP_OnWeaponSelectionClosed();
}

void UPlayerWeaponMenuComponent::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (Slot != EPlayerItemSlot::PrimaryWeapon && Slot != EPlayerItemSlot::SecondaryWeapon)
	{
		return;
	}

	if (AStealthPlayerCharacter* PlayerCharacter = GetOwningStealthPlayerCharacter())
	{
		if (UPlayerItemComponent* ItemComponent = PlayerCharacter->GetItemComponent())
		{
			ItemComponent->SetActiveWeaponSlot(Slot, true);
		}
	}

	ShowWeaponQuickSelectIndicator(Slot);
	CloseWeaponSelectionWidget();
}

void UPlayerWeaponMenuComponent::CancelWeaponSelectionMenu()
{
	CloseWeaponSelectionWidget();
}

AStealthPlayerController* UPlayerWeaponMenuComponent::GetOwningStealthPlayerController() const
{
	return Cast<AStealthPlayerController>(GetOwner());
}

AStealthPlayerCharacter* UPlayerWeaponMenuComponent::GetOwningStealthPlayerCharacter() const
{
	if (const AStealthPlayerController* Controller = GetOwningStealthPlayerController())
	{
		return Cast<AStealthPlayerCharacter>(Controller->GetPawn());
	}

	return nullptr;
}
