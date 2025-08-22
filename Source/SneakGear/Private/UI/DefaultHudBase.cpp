#include "UI/DefaultHudBase.h"

#include "PlayerCharacterBase.h"
#include "UI/HealthStaminaBarsBase.h"
#include "UI/StatBarBase.h"
#include "UI/DefaultHudLayoutBase.h"

void ADefaultHudBase::SetCurrentViewMode(EHudViewMode NewViewMode)
{
	CurrentViewMode = NewViewMode;
	UpdateWidgets();
}

void ADefaultHudBase::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	checkf(World, TEXT("Failed to reference World."));

	checkf(DefaultLayoutClass, TEXT("Invalid MinimalLayoutClass reference."));
	// checkf(DebugLayoutClass, TEXT("Invalid OverloadLayoutClass reference."));

	DefaultLayoutWidget = CreateWidget<UDefaultHudLayoutBase>(World, DefaultLayoutClass);
	DefaultLayoutWidget->AddToViewport();
	DefaultLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	// DebugLayoutWidget = CreateWidget<UDebugHudLayoutBase>(World, DebugLayoutClass);
	// DebugLayoutWidget->AddToViewport();
	// DebugLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		PlayerCharacter = Cast<APlayerCharacterBase>(PlayerController->GetPawn());
	}
	checkf(PlayerCharacter, TEXT("Unable to get a reference to the player character."));

	UpdateWidgets();
}

void ADefaultHudBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAllHandlers();
	Super::EndPlay(EndPlayReason);
}

void ADefaultHudBase::UpdateWidgets()
{
	ClearAllHandlers();

	DefaultLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);
	// DebugLayoutWidget->SetVisibility(ESlateVisibility::Collapsed);

	switch (CurrentViewMode)
	{
	case EHudViewMode::Clean:
		break;
	case EHudViewMode::Default:
		PlayerCharacter->OnHealthChanged.AddDynamic(DefaultLayoutWidget->BaseBars->HealthBar,
			&UStatBarBase::OnIntStatUpdated);
		PlayerCharacter->OnStaminaChanged.AddDynamic(DefaultLayoutWidget->BaseBars->StaminaBar,
			&UStatBarBase::OnFloatStatUpdated);
		DefaultLayoutWidget->SetVisibility(ESlateVisibility::Visible);
		break;
	case EHudViewMode::Debug:
		// PlayerCharacter->OnHealthChanged.AddDynamic(DebugLayoutWidget->BaseBars->HealthBar,
		// 	&UStatBarBase::OnIntStatUpdated);
		// PlayerCharacter->OnStaminaChanged.AddDynamic(DebugLayoutWidget->BaseBars->StaminaBar,
		// 	&UStatBarBase::OnFloatStatUpdated);
		// DebugLayoutWidget->SetVisibility(ESlateVisibility::Visible);
		break;
	default:;
	}

	PlayerCharacter->BroadcastCurrentStats();
}

void ADefaultHudBase::ClearAllHandlers()
{
	PlayerCharacter->OnHealthChanged.Clear();
	PlayerCharacter->OnStaminaChanged.Clear();
}
