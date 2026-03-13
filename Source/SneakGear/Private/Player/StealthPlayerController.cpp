#include "Player/StealthPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Player/Components/PlayerHUDComponent.h"
#include "Player/Components/PlayerWeaponSelectionComponent.h"
#include "Blueprint/UserWidget.h"

AStealthPlayerController::AStealthPlayerController()
{
	PlayerHUDComponent = CreateDefaultSubobject<UPlayerHUDComponent>(TEXT("PlayerHUDComponent"));
	PlayerWeaponSelectionComponent = CreateDefaultSubobject<UPlayerWeaponSelectionComponent>(TEXT("PlayerWeaponSelectionComponent"));
}

void AStealthPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupInputMappings();
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->Initialize(PlayerHUDWidgetClass, CrosshairWidgetClass, CrosshairSpread);
	}
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->Initialize(WeaponQuickIndicatorWidgetClass, WeaponSelectionWidgetClass,
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
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->ShowWeaponQuickSelectIndicator(Slot);
	}
}

void AStealthPlayerController::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->OpenWeaponSelectionWidget(InitialSlot);
	}
}

void AStealthPlayerController::CloseWeaponSelectionWidget()
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->CloseWeaponSelectionWidget();
	}
}

void AStealthPlayerController::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->SelectWeaponFromSelectionMenu(Slot);
	}
}

void AStealthPlayerController::CancelWeaponSelectionMenu()
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->CancelWeaponSelectionMenu();
	}
}

bool AStealthPlayerController::IsWeaponSelectionWidgetOpen() const
{
	return PlayerWeaponSelectionComponent && PlayerWeaponSelectionComponent->IsWeaponSelectionWidgetOpen();
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
