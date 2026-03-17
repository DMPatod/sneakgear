#include "TestWorldHelpers.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardData.h"
#include "Tests/AutomationEditorCommon.h"
#include "UI/EventLogSubsystem.h"

UWorld* CreateTestWorld()
{
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
	if (World && !World->HasBegunPlay())
	{
		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
	}

	return World;
}

UBlackboardData* CreatePatrolBlackboard(UObject* Outer)
{
	UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>(Outer);
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(TEXT("PatrolLocation"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(TEXT("PatrolIndex"));
	BlackboardAsset->UpdateKeyIDs();
	return BlackboardAsset;
}

UBlackboardData* CreateGuardBlackboard(UObject* Outer)
{
	UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>(Outer);
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(TEXT("TargetActor"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Float>(TEXT("Awareness"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(TEXT("HasLineOfSight"));
	BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Int>(TEXT("AwarenessState"));
	BlackboardAsset->UpdateKeyIDs();
	return BlackboardAsset;
}

APlayerController* EnsureTestPlayerController(UWorld* World)
{
	if (APlayerController* Existing = World ? World->GetFirstPlayerController() : nullptr)
	{
		return Existing;
	}

	if (!World)
	{
		return nullptr;
	}

	return World->SpawnActor<APlayerController>();
}

bool EventLogContains(UEventLogSubsystem* EventLog, const FString& Substring)
{
	if (!EventLog)
	{
		return false;
	}

	TArray<FGameEventEntry> Entries;
	EventLog->GetEventsAfter(0, Entries);
	for (const FGameEventEntry& Entry : Entries)
	{
		if (Entry.Message.ToString().Contains(Substring))
		{
			return true;
		}
	}

	return false;
}
