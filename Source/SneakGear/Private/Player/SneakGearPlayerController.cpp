#include "Player/SneakGearPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Misc/DataValidation.h"
#include "Player/Components/PlayerHUDComponent.h"
#include "Player/Components/PlayerWeaponSelectionComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/CrosshairNativeWidget.h"
#include "UI/CrosshairWidget.h"
#include "UI/PlayerHUDWidget.h"
#include "UI/WeaponQuickIndicatorWidget.h"
#include "UI/WeaponSelectionMenuWidget.h"

ASneakGearPlayerController::ASneakGearPlayerController()
{
	PlayerHUDComponent = CreateDefaultSubobject<UPlayerHUDComponent>(TEXT("PlayerHUDComponent"));
	PlayerWeaponSelectionComponent = CreateDefaultSubobject<UPlayerWeaponSelectionComponent>(TEXT("PlayerWeaponSelectionComponent"));
	CrosshairWidgetClass = UCrosshairNativeWidget::StaticClass();
}

void ASneakGearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupInputMappings();
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->Initialize(PlayerHUDWidgetClass, CrosshairWidgetClass, CrosshairSpread, bShowCrosshairOnlyWhenAiming);
	}
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->Initialize(WeaponQuickIndicatorWidgetClass, WeaponSelectionWidgetClass,
		                                      WeaponQuickIndicatorDuration);
	}
}

void ASneakGearPlayerController::OnWeaponFired()
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->OnWeaponFired();
	}
}

EDataValidationResult ASneakGearPlayerController::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!DefaultMappingContext)
	{
		Context.AddError(FText::FromString(TEXT("DefaultMappingContext must be assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!PlayerHUDWidgetClass)
	{
		Context.AddWarning(FText::FromString(TEXT("PlayerHUDWidgetClass is not assigned; no main HUD will be created.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!CrosshairWidgetClass)
	{
		Context.AddWarning(FText::FromString(TEXT("CrosshairWidgetClass is not assigned; the crosshair will be unavailable.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!WeaponQuickIndicatorWidgetClass)
	{
		Context.AddWarning(FText::FromString(TEXT("WeaponQuickIndicatorWidgetClass is not assigned; quick-select feedback will be unavailable.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (!WeaponSelectionWidgetClass)
	{
		Context.AddWarning(FText::FromString(TEXT("WeaponSelectionWidgetClass is not assigned; the weapon selection menu cannot open.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

void ASneakGearPlayerController::SetCrosshairVisible(bool bVisible)
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->SetCrosshairVisible(bVisible);
	}
}

void ASneakGearPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASneakGearPlayerController::NotifyHitMarker()
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->NotifyHitMarker();
	}
}

void ASneakGearPlayerController::SetOverlayWidgetVisible(TSubclassOf<UUserWidget> WidgetClass, bool bVisible)
{
	if (PlayerHUDComponent)
	{
		PlayerHUDComponent->SetOverlayWidgetVisible(WidgetClass, bVisible);
	}
}

void ASneakGearPlayerController::ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot)
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->ShowWeaponQuickSelectIndicator(Slot);
	}
}

void ASneakGearPlayerController::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->OpenWeaponSelectionWidget(InitialSlot);
	}
}

void ASneakGearPlayerController::CloseWeaponSelectionWidget()
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->CloseWeaponSelectionWidget();
	}
}

void ASneakGearPlayerController::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->SelectWeaponFromSelectionMenu(Slot);
	}
}

void ASneakGearPlayerController::CancelWeaponSelectionMenu()
{
	if (PlayerWeaponSelectionComponent)
	{
		PlayerWeaponSelectionComponent->CancelWeaponSelectionMenu();
	}
}

bool ASneakGearPlayerController::IsWeaponSelectionWidgetOpen() const
{
	return PlayerWeaponSelectionComponent && PlayerWeaponSelectionComponent->IsWeaponSelectionWidgetOpen();
}

void ASneakGearPlayerController::ApplyMenuInputMode(UUserWidget* FocusWidget)
{
	FInputModeGameAndUI InputMode;
	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ASneakGearPlayerController::ApplyGameplayInputMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ASneakGearPlayerController::SetupInputMappings()
{
	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is not set on %s"), *GetName());
	}
	auto* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && DefaultMappingContext)
	{
		auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

		}
	}
}
