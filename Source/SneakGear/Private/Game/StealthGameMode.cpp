#include "Game/StealthGameMode.h"

#include "Player/SneakGearPlayerCharacter.h"
#include "Player/StealthPlayerController.h"

AStealthGameMode::AStealthGameMode()
{
	DefaultPawnClass = ASneakGearPlayerCharacter::StaticClass();
	PlayerControllerClass = AStealthPlayerController::StaticClass();
}
