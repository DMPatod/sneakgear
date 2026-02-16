#include "AI/WaypointAction.h"

#include "AIController.h"

float UWaypointActionBase::ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController)
{
	return 0.f;
}

float UWaypointAction_StandBy::ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController)
{
	return FMath::Max(0.f, Duration);
}

float UWaypointAction_LookAround::ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController)
{
	if (GuardPawn)
	{
		auto Rotation = GuardPawn->GetActorRotation();
		Rotation.Yaw += FMath::FRandRange(-YawDelta, YawDelta);
		GuardPawn->SetActorRotation(Rotation);

		if (GuardController)
		{
			GuardController->SetControlRotation(Rotation);
		}
	}

	return FMath::Max(0.f, Duration);
}
