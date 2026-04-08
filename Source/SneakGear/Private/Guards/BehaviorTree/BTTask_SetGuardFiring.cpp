#include "Guards/BehaviorTree/BTTask_SetGuardFiring.h"

#include "AIController.h"
#include "Guards/GuardCharacter.h"

EBTNodeResult::Type UBTTask_SetGuardFiring::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardCharacter* GuardCharacter = OwnerComp.GetAIOwner() ? Cast<AGuardCharacter>(OwnerComp.GetAIOwner()->GetPawn()) : nullptr;
	if (!GuardCharacter)
	{
		return EBTNodeResult::Failed;
	}

	GuardCharacter->SetCombatFiringEnabled(bEnableFiring);
	return EBTNodeResult::Succeeded;
}
