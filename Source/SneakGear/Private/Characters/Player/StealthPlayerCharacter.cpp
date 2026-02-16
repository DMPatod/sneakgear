#include "Characters/Player/StealthPlayerCharacter.h"

#include "Cover/CoverComponent.h"
#include "Cover/CoverStateComponent.h"
#include "EnhancedInputComponent.h"

AStealthPlayerCharacter::AStealthPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	CoverStateComponent = CreateDefaultSubobject<UCoverStateComponent>(TEXT("CoverStateComponent"));
}

void AStealthPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* Eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Eic)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an enhanced input component"));
		return;
	}

	if (CrouchAction)
	{
		Eic->BindAction(CrouchAction, ETriggerEvent::Started, this, &AStealthPlayerCharacter::StartCrouch);
		Eic->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AStealthPlayerCharacter::StopCrouch);
	}
}

void AStealthPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CoverStateComponent && CoverStateComponent->HandleMoveInput(this, Value))
	{
		return;
	}

	Super::Move(Value);
}

void AStealthPlayerCharacter::StartCrouch()
{
	Crouch();
}

void AStealthPlayerCharacter::StopCrouch()
{
	UnCrouch();
}
