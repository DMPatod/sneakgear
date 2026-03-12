#include "Player/StealthPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Player/Components/PlayerHUDComponent.h"
#include "Player/Components/PlayerWeaponMenuComponent.h"
#include "Blueprint/UserWidget.h"

AStealthPlayerController::AStealthPlayerController()
{
	PlayerHUDComponent = CreateDefaultSubobject<UPlayerHUDComponent>(TEXT("PlayerHUDComponent"));
	PlayerWeaponMenuComponent = CreateDefaultSubobject<UPlayerWeaponMenuComponent>(TEXT("PlayerWeaponMenuComponent"));
}

void AStealthPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupInputMappings();
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->Initialize(MainHUDWidgetClass, CrosshairWidgetClass, CrosshairSpread);
	}
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->Initialize(WeaponQuickIndicatorWidgetClass, WeaponSelectionWidgetClass,
		                                      WeaponQuickIndicatorDuration);
	}
}

void AStealthPlayerController::OnWeaponFired()
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->OnWeaponFired();
	}
}

void AStealthPlayerController::SetCrosshairVisible(bool bVisible)
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->SetCrosshairVisible(bVisible);
	}
}

void AStealthPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AStealthPlayerController::NotifyHitMarker()
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->NotifyHitMarker();
	}
}

void AStealthPlayerController::ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot)
{
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->ShowWeaponQuickSelectIndicator(Slot);
	}
}

void AStealthPlayerController::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->OpenWeaponSelectionWidget(InitialSlot);
	}
}

void AStealthPlayerController::CloseWeaponSelectionWidget()
{
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->CloseWeaponSelectionWidget();
	}
}

void AStealthPlayerController::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->SelectWeaponFromSelectionMenu(Slot);
	}
}

void AStealthPlayerController::CancelWeaponSelectionMenu()
{
	if (PlayerWeaponMenuComponent)
	{
		PlayerWeaponMenuComponent->CancelWeaponSelectionMenu();
	}
}

bool AStealthPlayerController::IsWeaponSelectionWidgetOpen() const
{
	return PlayerWeaponMenuComponent && PlayerWeaponMenuComponent->IsWeaponSelectionWidgetOpen();
}

void AStealthPlayerController::ApplyMenuInputMode(UUserWidget* FocusWidget)
{
	FInputModeGameAndUI InputMode;
	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AStealthPlayerController::ApplyGameplayInputMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void AStealthPlayerController::SetupInputMappings()
{
	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is not set on %s"), *GetName());
	}
	if (!DebugMappingContext && bDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugMappingContext is not set on %s"), *GetName());
	}

	auto* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && DefaultMappingContext)
	{
		auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

			if (bDebug)
			{
				Subsystem->AddMappingContext(DebugMappingContext, 0);
			}
		}
	}
}
