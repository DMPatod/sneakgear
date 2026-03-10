#include "AI/GuardAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AI/GuardCharacter.h"
#include "Kismet/GameplayStatics.h"

AGuardAIController::AGuardAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	bBehaviorTreeActive = false;

	const AGuardCharacter* GuardCharacter = Cast<AGuardCharacter>(InPawn);
	if (!GuardCharacter)
	{
		return;
	}

	if (!ensureAlwaysMsgf(GuardCharacter->GetBehaviorTreeAsset(),
		TEXT("Guard '%s' must define a BehaviorTreeAsset."), *GetNameSafe(GuardCharacter)))
	{
		return;
	}

	if (GuardCharacter->GetBehaviorTreeAsset())
	{
		UBlackboardData* BlackboardAsset = GuardCharacter->GetBlackboardAsset();
		if (!BlackboardAsset)
		{
			BlackboardAsset = GuardCharacter->GetBehaviorTreeAsset()->BlackboardAsset;
		}

		if (!ensureAlwaysMsgf(BlackboardAsset,
			TEXT("Guard '%s' must define a blackboard, either explicitly or through its behavior tree."),
			*GetNameSafe(GuardCharacter)))
		{
			return;
		}

		if (BlackboardAsset)
		{
			UBlackboardComponent* InitializedBlackboard = nullptr;
			const bool bBlackboardReady = UseBlackboard(BlackboardAsset, InitializedBlackboard);
			if (bBlackboardReady && RunBehaviorTree(GuardCharacter->GetBehaviorTreeAsset()))
			{
				bBehaviorTreeActive = true;
				UpdateBlackboardFromGuard();
			}
			else
			{
				ensureAlwaysMsgf(false, TEXT("Guard '%s' failed to initialize its behavior tree."), *GetNameSafe(GuardCharacter));
			}
		}
	}
}

void AGuardAIController::UpdateBlackboardFromGuard()
{
	UBlackboardComponent* Blackboard = GetBlackboardComponent();
	const AGuardCharacter* GuardCharacter = Cast<AGuardCharacter>(GetPawn());
	if (!Blackboard || !GuardCharacter)
	{
		return;
	}

	if (!TargetActorKey.IsNone())
	{
		Blackboard->SetValueAsObject(TargetActorKey, UGameplayStatics::GetPlayerPawn(this, 0));
	}
	if (!AwarenessKey.IsNone())
	{
		Blackboard->SetValueAsFloat(AwarenessKey, GuardCharacter->GetAwareness());
	}
	if (!HasLineOfSightKey.IsNone())
	{
		Blackboard->SetValueAsBool(HasLineOfSightKey, GuardCharacter->HasLineOfSight());
	}
	if (!AwarenessStateKey.IsNone())
	{
		Blackboard->SetValueAsInt(AwarenessStateKey, static_cast<int32>(GuardCharacter->GetAwarenessState()));
	}
}

void AGuardAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bBehaviorTreeActive)
	{
		UpdateBlackboardFromGuard();
	}
}
