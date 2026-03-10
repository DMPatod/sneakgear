#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuardAIController.generated.h"

UCLASS()
class SNEAKGEAR_API AGuardAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuardAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	bool bBehaviorTreeActive = false;

	UPROPERTY(EditDefaultsOnly, Category="BehaviorTree|Blackboard")
	FName TargetActorKey = TEXT("TargetActor");

	UPROPERTY(EditDefaultsOnly, Category="BehaviorTree|Blackboard")
	FName AwarenessKey = TEXT("Awareness");

	UPROPERTY(EditDefaultsOnly, Category="BehaviorTree|Blackboard")
	FName HasLineOfSightKey = TEXT("HasLineOfSight");

	UPROPERTY(EditDefaultsOnly, Category="BehaviorTree|Blackboard")
	FName AwarenessStateKey = TEXT("AwarenessState");

	void UpdateBlackboardFromGuard();
};
