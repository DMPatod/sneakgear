#pragma once

#include "CoreMinimal.h"
#include "GuardAwarenessTypes.generated.h"

UENUM(BlueprintType)
enum class EGuardAwarenessState : uint8
{
	Calm,
	Suspicious,
	Alerted
};
