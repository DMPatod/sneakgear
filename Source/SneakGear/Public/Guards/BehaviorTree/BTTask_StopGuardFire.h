#pragma once

#include "CoreMinimal.h"
#include "Guards/BehaviorTree/BTTask_SetGuardFiring.h"
#include "BTTask_StopGuardFire.generated.h"

UCLASS()
class SNEAKGEAR_API UBTTask_StopGuardFire : public UBTTask_SetGuardFiring
{
	GENERATED_BODY()

public:
	UBTTask_StopGuardFire();
};
