#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuardAIController.generated.h"

class APatrolPath;
struct FPathFollowingResult;

UCLASS()
class SNEAKGEAR_API AGuardAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

	void SetPatrolPath(APatrolPath* Path)
	{
		PatrolPath = Path;
	}
	
	void MovetoNextPoint();

protected:
	FTimerHandle WaypointActionTimer;

	int32 PatrolIndex = 0;

	UPROPERTY(EditAnywhere, Category="Patrol")
	TObjectPtr<APatrolPath> PatrolPath;

	UPROPERTY(EditAnywhere, Category="Patrol")
	float AcceptanceRadius = 40.f;

	UPROPERTY(EditAnywhere, Category="Patrol")
	bool bLoop = true;

	void ExecuteWaypointAction(int32 ReachedIndex);
	void FinishWaypointAction();
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
