#include "AI/GuardAIController.h"

#include "AI/PatrolPath.h"
#include "AI/WaypointAction.h"
#include "Navigation/PathFollowingComponent.h"

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MovetoNextPoint();
}

void AGuardAIController::MovetoNextPoint()
{
	GetWorldTimerManager().ClearTimer(WaypointActionTimer);

	if (!PatrolPath || PatrolPath->Num() <= 0)
	{
		return;
	}

	if (bLoop)
	{
		PatrolIndex = PatrolIndex % PatrolPath->Num();
	}
	else if (PatrolIndex >= PatrolPath->Num())
	{
		return;
	}

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
		const auto ReachedIndex = bLoop ? PatrolIndex % PatrolPath->Num() : PatrolIndex;
		ExecuteWaypointAction(ReachedIndex);
		return;
	}

	MovetoNextPoint();
}

void AGuardAIController::ExecuteWaypointAction(int32 ReachedIndex)
{
	if (!PatrolPath)
	{
		return;
	}

	float ActionDuration = 0.f;
	if (auto* WaypointAction = PatrolPath->GetWaypointAction(ReachedIndex);
		WaypointAction && WaypointAction->GetClass()->ImplementsInterface(UWaypointAction::StaticClass()))
	{
		ActionDuration = IWaypointAction::Execute_ExecuteAtWaypoint(WaypointAction, GetPawn(), this);
	}
	ActionDuration = FMath::Max(0.f, ActionDuration);

	PatrolIndex++;

	if (!bLoop && PatrolIndex >= PatrolPath->Num())
	{
		return;
	}

	if (ActionDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(WaypointActionTimer, this, &AGuardAIController::FinishWaypointAction,
		                                ActionDuration, false);
	}
	else
	{
		FinishWaypointAction();
	}
}

void AGuardAIController::FinishWaypointAction()
{
	MovetoNextPoint();
}
