#include "StealthGameMode.h"

#include "StealthPlayerCharacter.h"
#include "StealthPlayerController.h"

AStealthGameMode::AStealthGameMode()
{
	DefaultPawnClass = AStealthPlayerCharacter::StaticClass();
	PlayerControllerClass = AStealthPlayerController::StaticClass();
}
