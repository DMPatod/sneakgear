#pragma once

#include "CoreMinimal.h"
#include "PlayerMovementTypes.generated.h"

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing UMETA(DisplayName="Standing"),
	Crouching UMETA(DisplayName="Crouching"),
	Prone UMETA(DisplayName="Prone")
};
