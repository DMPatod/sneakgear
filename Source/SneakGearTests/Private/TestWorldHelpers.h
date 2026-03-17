#pragma once

#include "CoreMinimal.h"

class APlayerController;
class UBlackboardData;
class UEventLogSubsystem;
class UObject;
class UWorld;

UWorld* CreateTestWorld();
UBlackboardData* CreatePatrolBlackboard(UObject* Outer);
UBlackboardData* CreateGuardBlackboard(UObject* Outer);
APlayerController* EnsureTestPlayerController(UWorld* World);
bool EventLogContains(UEventLogSubsystem* EventLog, const FString& Substring);
