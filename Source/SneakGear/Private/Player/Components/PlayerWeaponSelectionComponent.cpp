#include "Player/Components/PlayerWeaponSelectionComponent.h"

#include "Player/Components/PlayerWeaponComponent.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Player/SneakGearPlayerController.h"
#include "TimerManager.h"
#include "UI/WeaponQuickIndicatorWidget.h"
#include "UI/WeaponSelectionMenuWidget.h"

UPlayerWeaponSelectionComponent::UPlayerWeaponSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerWeaponSelectionComponent::Initialize(TSubclassOf<UWeaponQuickIndicatorWidget> InWeaponQuickIndicatorWidgetClass,
                                            TSubclassOf<UWeaponSelectionMenuWidget> InWeaponSelectionWidgetClass,
                                            float InWeaponQuickIndicatorDuration)
{
	WeaponQuickIndicatorWidgetClass = InWeaponQuickIndicatorWidgetClass;
	WeaponSelectionWidgetClass = InWeaponSelectionWidgetClass;
	WeaponQuickIndicatorDuration = InWeaponQuickIndicatorDuration;
}

void UPlayerWeaponSelectionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerWeaponSelectionComponent::ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot)
{
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
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

void UPlayerWeaponSelectionComponent::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
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

void UPlayerWeaponSelectionComponent::CloseWeaponSelectionWidget()
{
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
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

void UPlayerWeaponSelectionComponent::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (Slot != EPlayerItemSlot::PrimaryWeapon && Slot != EPlayerItemSlot::SecondaryWeapon)
	{
		return;
	}

	if (ASneakGearPlayerCharacter* PlayerCharacter = GetOwningSneakGearPlayerCharacter())
	{
		if (UPlayerWeaponComponent* WeaponComponent = PlayerCharacter->GetWeaponComponent())
		{
			WeaponComponent->SetActiveWeaponSlot(Slot, true);
		}
	}

	ShowWeaponQuickSelectIndicator(Slot);
	CloseWeaponSelectionWidget();
}

void UPlayerWeaponSelectionComponent::CancelWeaponSelectionMenu()
{
	CloseWeaponSelectionWidget();
}

ASneakGearPlayerController* UPlayerWeaponSelectionComponent::GetOwningSneakGearPlayerController() const
{
	return Cast<ASneakGearPlayerController>(GetOwner());
}

ASneakGearPlayerCharacter* UPlayerWeaponSelectionComponent::GetOwningSneakGearPlayerCharacter() const
{
	if (const ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController())
	{
		return Cast<ASneakGearPlayerCharacter>(Controller->GetPawn());
	}

	return nullptr;
}
