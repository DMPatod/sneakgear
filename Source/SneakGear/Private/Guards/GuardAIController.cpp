#include "Guards/GuardAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Guards/GuardCharacter.h"
#include "Guards/Components/GuardAwarenessComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
bool ValidateRequiredBlackboardKeys(const UBlackboardData* BlackboardAsset)
{
	if (!BlackboardAsset)
	{
		return false;
	}

	return BlackboardAsset->GetKeyID(TEXT("TargetActor")) != FBlackboard::InvalidKey &&
		BlackboardAsset->GetKeyID(TEXT("Awareness")) != FBlackboard::InvalidKey &&
		BlackboardAsset->GetKeyID(TEXT("HasLineOfSight")) != FBlackboard::InvalidKey &&
		BlackboardAsset->GetKeyID(TEXT("AwarenessState")) != FBlackboard::InvalidKey;
}
}

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

		if (!ensureAlwaysMsgf(ValidateRequiredBlackboardKeys(BlackboardAsset),
			TEXT("Guard '%s' blackboard is missing one or more required keys: TargetActor, Awareness, HasLineOfSight, AwarenessState."),
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

	const UGuardAwarenessComponent* Awareness = GuardCharacter->GetAwarenessComponent();
	if (!Awareness)
	{
		return;
	}

	if (!TargetActorKey.IsNone())
	{
		Blackboard->SetValueAsObject(TargetActorKey, Awareness->GetTargetActor());
	}
	if (!AwarenessKey.IsNone())
	{
		Blackboard->SetValueAsFloat(AwarenessKey, Awareness->GetAwareness());
	}
	if (!HasLineOfSightKey.IsNone())
	{
		Blackboard->SetValueAsBool(HasLineOfSightKey, Awareness->HasLineOfSight());
	}
	if (!AwarenessStateKey.IsNone())
	{
		Blackboard->SetValueAsInt(AwarenessStateKey, static_cast<int32>(Awareness->GetAwarenessState()));
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
