#include "Guards/BehaviorTree/BTTask_ReloadGuardWeapon.h"

#include "AIController.h"
#include "Guards/GuardCharacter.h"

UBTTask_ReloadGuardWeapon::UBTTask_ReloadGuardWeapon()
{
	NodeName = TEXT("Reload Guard Weapon");
}

EBTNodeResult::Type UBTTask_ReloadGuardWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardCharacter* GuardCharacter = OwnerComp.GetAIOwner() ? Cast<AGuardCharacter>(OwnerComp.GetAIOwner()->GetPawn()) : nullptr;
	if (!GuardCharacter)
	{
		return EBTNodeResult::Failed;
	}

	return GuardCharacter->ReloadWeaponIfNeeded() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
