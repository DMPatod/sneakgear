#include "Characters/MainPlayerControllerBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/MainPlayerCharacterBase.h"

void AMainPlayerControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PlayerCharacter = Cast<AMainPlayerCharacterBase>(aPawn);
	checkf(PlayerCharacter,
	       TEXT(
		       "MainPlayerControllerBase::OnPossess: MainPlayerControllerBase can only possess MainPlayerCharacterBase derived pawns."
	       ));

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent,
	       TEXT("MainPlayerControllerBase::OnPossess: Unable to get reference to the EnhancedInputComponent."));

	auto InputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubSystem,
	       TEXT("MainPlayerControllerBase::OnPossess: Unable to get reference to the EnhancedInputLocalPlayerSubsystem."
	       ));

	checkf(InputMappingContent, TEXT("MainPlayerControllerBase::OnPossess: InputMappingContext was not specified."));
	InputSubSystem->ClearAllMappings();
	InputSubSystem->AddMappingContext(InputMappingContent, 0);

	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleMove);
	}

	if (ActionLook)
	{
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleLook);
	}

	if (ActionToggleSprint)
	{
		EnhancedInputComponent->BindAction(ActionToggleSprint, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleToggleSprint);
	}

	if (ActionToggleCrouch)
	{
		EnhancedInputComponent->BindAction(ActionToggleCrouch, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleToggleCrouch);
	}

	if (ActionAimingDown)
	{
		EnhancedInputComponent->BindAction(ActionAimingDown, ETriggerEvent::Ongoing, this,
		                                   &AMainPlayerControllerBase::HandleToggleAimingDown);
		EnhancedInputComponent->BindAction(ActionAimingDown, ETriggerEvent::Completed, this,
		                                   &AMainPlayerControllerBase::HandleToggleAimingDown);
	}

	if (ActionSwitchAimingCamera)
	{
		EnhancedInputComponent->BindAction(ActionSwitchAimingCamera, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleSwitchAimingCamera);
	}

	if (ActionSwitchAimingShoulder)
	{
		EnhancedInputComponent->BindAction(ActionSwitchAimingShoulder, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerControllerBase::HandleSwitchAimingShoulder);
	}
}

void AMainPlayerControllerBase::OnUnPossess()
{
	EnhancedInputComponent->ClearActionBindings();

	Super::OnUnPossess();
}

void AMainPlayerControllerBase::HandleLook(const FInputActionValue& InputActionValue)
{
	const FVector2D AxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(AxisVector.X);
	AddPitchInput(AxisVector.Y);
}

void AMainPlayerControllerBase::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D AxisVector = InputActionValue.Get<FVector2D>();

	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), AxisVector.Y);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), AxisVector.X);
}

void AMainPlayerControllerBase::HandleToggleSprint()
{
}

void AMainPlayerControllerBase::HandleToggleCrouch()
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

void AMainPlayerControllerBase::HandleToggleAimingDown()
{
	PlayerCharacter->ToggleAim();
}

void AMainPlayerControllerBase::HandleSwitchAimingCamera()
{
	if (PlayerCharacter->GetIsAiming())
	{
		PlayerCharacter->SwitchToFirstPersonCamera();
	}
	else
	{
		PlayerCharacter->SwitchToNormalCamera();
	}
}

void AMainPlayerControllerBase::HandleSwitchAimingShoulder()
{
}
