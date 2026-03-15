#pragma once

#include "CoreMinimal.h"
#include "Guards/BehaviorTree/BTTask_SetNextPatrolPoint.h"
#include "Guards/GuardAIController.h"
#include "TestAI.generated.h"

class UBlackboardData;

UCLASS()
class SNEAKGEARTESTS_API ATestGuardAIController : public AGuardAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

	bool InitializeBlackboardForTest(UBlackboardData* BlackboardAsset);
	void UpdateBlackboardForTest();
};

UCLASS()
class SNEAKGEARTESTS_API UTestBTTask_SetNextPatrolPoint : public UBTTask_SetNextPatrolPoint
{
	GENERATED_BODY()

public:
	void ConfigureBlackboardKeys(FName InPatrolLocationKeyName, FName InPatrolIndexKeyName);
	void SetLoopingForTest(bool bInLoop);
};
