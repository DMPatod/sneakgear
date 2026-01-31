#include "AI/PatrolPath.h"

FVector APatrolPath::GetWorldPoint(int32 Index) const
{
	if (!Waypoints.IsValidIndex(Index))
	{
		return GetActorLocation();
	}
	return GetActorTransform().TransformPosition(Waypoints[Index]);
}

#if WITH_EDITOR
void APatrolPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	for (auto i = 0; i < Waypoints.Num(); i++)
	{
		auto P = GetWorldPoint(i);
		DrawDebugSphere(GetWorld(), P, 25.f, 12, FColor::Yellow, false, 0.f, 0.f, 2.f);
		
		if (i + 1 < Waypoints.Num())
		{
			DrawDebugLine(GetWorld(), P, GetWorldPoint(i + 1), FColor::Yellow, false, 0.f, 0.f, 2.f);
		}
	}
}
#endif
