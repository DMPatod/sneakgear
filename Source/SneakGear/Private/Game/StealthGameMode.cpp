#include "Game/StealthGameMode.h"

#include "Characters/Player/StealthPlayerCharacter.h"
#include "Game/StealthPlayerController.h"

AStealthGameMode::AStealthGameMode()
{
	DefaultPawnClass = AStealthPlayerCharacter::StaticClass();
	PlayerControllerClass = AStealthPlayerController::StaticClass();
}
