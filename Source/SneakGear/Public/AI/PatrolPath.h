#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/GuardPatrolData.h"
#include "PatrolPath.generated.h"

UCLASS()
class SNEAKGEAR_API APatrolPath : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol")
	TObjectPtr<UGuardPatrolData> PatrolData;

	UPROPERTY(EditAnywhere, Category="Patrol")
	TArray<FVector> Waypoints;

	UPROPERTY(EditAnywhere, Category="Patrol|Editor", meta=(ClampMin="0.0"))
	float EditorPreviewDuration = 60.f;

	UFUNCTION(BlueprintCallable, Category="Patrol")
	FVector GetWorldPoint(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category="Patrol")
	UWaypointActionBase* GetWaypointAction(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category="Patrol")
	int32 Num() const;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	const FGuardPatrolWaypoint* GetDataWaypoint(int32 Index) const;
};
