#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsGuardClipEmpty.generated.h"

UCLASS()
class SNEAKGEAR_API UBTDecorator_IsGuardClipEmpty : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsGuardClipEmpty();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
