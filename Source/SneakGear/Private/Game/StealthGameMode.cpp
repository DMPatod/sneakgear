#include "Game/StealthGameMode.h"

#include "Characters/StealthPlayerCharacter.h"
#include "Game/StealthPlayerController.h"

AStealthGameMode::AStealthGameMode()
{
	DefaultPawnClass = AStealthPlayerCharacter::StaticClass();
	PlayerControllerClass = AStealthPlayerController::StaticClass();
}
