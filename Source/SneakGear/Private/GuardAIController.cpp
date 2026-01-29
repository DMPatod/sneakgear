#include "GuardAIController.h"

#include "PatrolPath.h"
#include "Navigation/PathFollowingComponent.h"

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MovetoNextPoint();
}

void AGuardAIController::MovetoNextPoint()
{
	if (!PatrolPath || PatrolPath->Num() <= 0)
	{
		return;
	}

	PatrolIndex = PatrolIndex % PatrolPath->Num();
	auto Target = PatrolPath->GetWorldPoint(PatrolIndex);

	MoveToLocation(Target, AcceptanceRadius);
}

void AGuardAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!PatrolPath || PatrolPath->Num() <= 0)
	{
		return;
	}

	if (Result.IsSuccess())
	{
		PatrolIndex++;

		if (!bLoop && PatrolIndex >= PatrolPath->Num())
		{
			return;
		}
	}

	MovetoNextPoint();
}
