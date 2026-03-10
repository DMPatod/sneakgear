#include "AI/BTTask_SetNextPatrolPoint.h"

#include "AIController.h"
#include "AI/PatrolPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AI/GuardPatrolComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_SetNextPatrolPoint::UBTTask_SetNextPatrolPoint()
{
	NodeName = TEXT("Set Next Patrol Point");

	PatrolLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetNextPatrolPoint, PatrolLocationKey));
	PatrolIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetNextPatrolPoint, PatrolIndexKey));
}

EBTNodeResult::Type UBTTask_SetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Blackboard || !ControlledPawn || PatrolLocationKey.SelectedKeyName.IsNone() || PatrolIndexKey.SelectedKeyName.IsNone())
	{
		return EBTNodeResult::Failed;
	}

	const UGuardPatrolComponent* PatrolComponent = ControlledPawn->FindComponentByClass<UGuardPatrolComponent>();
	const APatrolPath* PatrolPath = PatrolComponent ? PatrolComponent->GetPatrolPath() : nullptr;
	if (!PatrolPath)
	{
		return EBTNodeResult::Failed;
	}

	const int32 NumWaypoints = PatrolPath->Num();
	if (NumWaypoints <= 0)
	{
		return EBTNodeResult::Failed;
	}

	int32 PatrolIndex = Blackboard->GetValueAsInt(PatrolIndexKey.SelectedKeyName);
	if (bLoop)
	{
		PatrolIndex = (PatrolIndex % NumWaypoints + NumWaypoints) % NumWaypoints;
	}
	else
	{
		PatrolIndex = FMath::Clamp(PatrolIndex, 0, NumWaypoints - 1);
	}

	Blackboard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, PatrolPath->GetWorldPoint(PatrolIndex));

	int32 NextPatrolIndex = PatrolIndex + 1;
	if (bLoop)
	{
		NextPatrolIndex %= NumWaypoints;
	}
	else
	{
		NextPatrolIndex = FMath::Min(NextPatrolIndex, NumWaypoints - 1);
	}

	Blackboard->SetValueAsInt(PatrolIndexKey.SelectedKeyName, NextPatrolIndex);
	return EBTNodeResult::Succeeded;
}
