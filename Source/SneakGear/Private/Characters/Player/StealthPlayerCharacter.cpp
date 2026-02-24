#include "Characters/Player/StealthPlayerCharacter.h"

#include "Cover/CoverComponent.h"
#include "Cover/CoverStateComponent.h"

AStealthPlayerCharacter::AStealthPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	CoverStateComponent = CreateDefaultSubobject<UCoverStateComponent>(TEXT("CoverStateComponent"));
}

void AStealthPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CoverStateComponent && CoverStateComponent->HandleMoveInput(this, Value))
	{
		return;
	}

	Super::Move(Value);
}
