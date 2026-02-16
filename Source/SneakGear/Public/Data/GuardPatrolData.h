#pragma once

#include "AI/WaypointAction.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GuardPatrolData.generated.h"

USTRUCT(BlueprintType)
struct FGuardPatrolWaypoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Patrol")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Instanced, Category="Patrol")
	TObjectPtr<UWaypointActionBase> Action = nullptr;
};

UCLASS(BlueprintType)
class SNEAKGEAR_API UGuardPatrolData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol")
	TArray<FGuardPatrolWaypoint> Waypoints;
};
