#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReloadGuardWeapon.generated.h"

UCLASS()
class SNEAKGEAR_API UBTTask_ReloadGuardWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ReloadGuardWeapon();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
