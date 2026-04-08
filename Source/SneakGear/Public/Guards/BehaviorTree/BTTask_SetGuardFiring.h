#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetGuardFiring.generated.h"

UCLASS(Abstract)
class SNEAKGEAR_API UBTTask_SetGuardFiring : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category="Combat")
	bool bEnableFiring = false;
};
