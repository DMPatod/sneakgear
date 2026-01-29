#include "StealthPlayerController.h"

#include "EnhancedInputSubsystems.h"

void AStealthPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is not set on %s"), *GetName());
		return;
	}

	auto* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}
