#include "TestAI.h"

#include "BehaviorTree/BlackboardComponent.h"

void ATestGuardAIController::OnPossess(APawn* InPawn)
{
	AAIController::OnPossess(InPawn);
}

bool ATestGuardAIController::InitializeBlackboardForTest(UBlackboardData* BlackboardAsset)
{
	UBlackboardComponent* BlackboardComponent = nullptr;
	return BlackboardAsset && UseBlackboard(BlackboardAsset, BlackboardComponent);
}

void ATestGuardAIController::UpdateBlackboardForTest()
{
	UpdateBlackboardFromGuard();
}

void UTestBTTask_SetNextPatrolPoint::ConfigureBlackboardKeys(FName InPatrolLocationKeyName, FName InPatrolIndexKeyName)
{
	PatrolLocationKey.SelectedKeyName = InPatrolLocationKeyName;
	PatrolIndexKey.SelectedKeyName = InPatrolIndexKeyName;
}

void UTestBTTask_SetNextPatrolPoint::SetLoopingForTest(bool bInLoop)
{
	bLoop = bInLoop;
}
