#include "Guards/BehaviorTree/BTDecorator_IsGuardClipEmpty.h"

#include "AIController.h"
#include "Guards/GuardCharacter.h"

UBTDecorator_IsGuardClipEmpty::UBTDecorator_IsGuardClipEmpty()
{
	NodeName = TEXT("Is Guard Clip Empty");
}

bool UBTDecorator_IsGuardClipEmpty::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AGuardCharacter* GuardCharacter = OwnerComp.GetAIOwner() ? Cast<AGuardCharacter>(OwnerComp.GetAIOwner()->GetPawn()) : nullptr;
	return GuardCharacter && GuardCharacter->IsWeaponClipEmpty();
}
