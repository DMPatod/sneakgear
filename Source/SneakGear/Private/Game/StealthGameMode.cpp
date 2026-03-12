#include "Game/StealthGameMode.h"

#include "Player/StealthPlayerCharacter.h"
#include "Player/StealthPlayerController.h"

AStealthGameMode::AStealthGameMode()
{
	DefaultPawnClass = AStealthPlayerCharacter::StaticClass();
	PlayerControllerClass = AStealthPlayerController::StaticClass();
}
