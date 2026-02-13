#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GuardPatrolData.generated.h"

USTRUCT()
struct FWaypoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Stop")
	FVector StopLocation;

	UPROPERTY(EditAnywhere, Category="Stop")
	float StopWaitTime;
};

UCLASS(BlueprintType)
class SNEAKGEAR_API UGuardPatrolData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol")
	TArray<FVector> Waypoints;
};
