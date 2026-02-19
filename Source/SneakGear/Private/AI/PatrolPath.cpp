#include "AI/PatrolPath.h"

#include "DrawDebugHelpers.h"

const FGuardPatrolWaypoint* APatrolPath::GetDataWaypoint(int32 Index) const
{
	if (!PatrolData || !PatrolData->Waypoints.IsValidIndex(Index))
	{
		return nullptr;
	}

	return &PatrolData->Waypoints[Index];
}

int32 APatrolPath::Num() const
{
	return PatrolData && PatrolData->Waypoints.Num() > 0 ? PatrolData->Waypoints.Num() : Waypoints.Num();
}

FVector APatrolPath::GetWorldPoint(int32 Index) const
{
	if (const auto* DataWaypoint = GetDataWaypoint(Index))
	{
		return GetActorTransform().TransformPosition(DataWaypoint->Location);
	}

	if (!Waypoints.IsValidIndex(Index))
	{
		return GetActorLocation();
	}

	return GetActorTransform().TransformPosition(Waypoints[Index]);
}

UWaypointActionBase* APatrolPath::GetWaypointAction(int32 Index) const
{
	if (const auto* DataWaypoint = GetDataWaypoint(Index))
	{
		return DataWaypoint->Action;
	}

	return nullptr;
}

#if WITH_EDITOR
void APatrolPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	for (auto i = 0; i < Num(); i++)
	{
		auto P = GetWorldPoint(i);
		const auto Color = GetWaypointAction(i) ? FColor::Cyan : FColor::Yellow;

		DrawDebugSphere(GetWorld(), P, 25.f, 12, Color, false, EditorPreviewDuration, 0.f, 2.f);
		
		if (i + 1 < Num())
		{
			DrawDebugLine(GetWorld(), P, GetWorldPoint(i + 1), Color, false, EditorPreviewDuration, 0.f, 2.f);
		}
	}
}
#endif
