#include "PlayerControllerBase.h"

#include "InputAction.h"
#include "InputActionValue.h"
#include "PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PlayerCharacter = Cast<APlayerCharacterBase>(aPawn);
	checkf(PlayerCharacter, TEXT("PlayerControllerBase::OnPossess: PlayerControllerBase can only possess APlayerCharacterBase derived pawns."));

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("PlayerControllerBase::OnPossess: Unable to get reference to the EnhancedInputComponent."));

	UEnhancedInputLocalPlayerSubsystem* InputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubSystem, TEXT("PlayerControllerBase::OnPossess: Unable to get reference to the EnhancedInputLocalPlayerSubsystem."));

	checkf(InputMappingContent, TEXT("InputMappingContext was not specified."));
	InputSubSystem->ClearAllMappings();
	InputSubSystem->AddMappingContext(InputMappingContent, 0);

	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this,
			&APlayerControllerBase::HandleMove);
	}

	if (ActionLook)
	{
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this,
			&APlayerControllerBase::HandleLook);
	}

	if (ActionToggleSprint)
	{
		EnhancedInputComponent->BindAction(ActionToggleSprint, ETriggerEvent::Triggered, this,
			&APlayerControllerBase::HandleToggleSprint);
	}

	if (ActionToggleCrouch)
	{
		EnhancedInputComponent->BindAction(ActionToggleCrouch, ETriggerEvent::Triggered, this,
			&APlayerControllerBase::HandleToggleCrouch);
	}
}

void APlayerControllerBase::OnUnPossess()
{
	EnhancedInputComponent->ClearActionBindings();

	Super::OnUnPossess();
}

void APlayerControllerBase::HandleLook(const FInputActionValue& InputActionValue)
{
	const FVector2D AxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(AxisVector.X);
	AddPitchInput(AxisVector.Y);
}

void APlayerControllerBase::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D AxisVector = InputActionValue.Get<FVector2D>();

	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), AxisVector.Y);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), AxisVector.X);
}

void APlayerControllerBase::HandleToggleSprint()
{
	PlayerCharacter->ToggleSprint();
}

void APlayerControllerBase::HandleToggleCrouch()
{
	if (PlayerCharacter->IsCrouched())
	{
		PlayerCharacter->UnCrouch();
	}
	else
	{
		PlayerCharacter->Crouch();
	}
}