#pragma once

#include "CoreMinimal.h"
#include "Guards/BehaviorTree/BTTask_SetGuardFiring.h"
#include "BTTask_StartGuardFire.generated.h"

UCLASS()
class SNEAKGEAR_API UBTTask_StartGuardFire : public UBTTask_SetGuardFiring
{
	GENERATED_BODY()

public:
	UBTTask_StartGuardFire();
};
