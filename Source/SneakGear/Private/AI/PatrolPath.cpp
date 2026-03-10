#include "AI/PatrolPath.h"

#include "DrawDebugHelpers.h"

int32 APatrolPath::Num() const
{
	return Waypoints.Num();
}

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

	for (auto i = 0; i < Num(); i++)
	{
		auto P = GetWorldPoint(i);
		const auto Color = FColor::Yellow;

		DrawDebugSphere(GetWorld(), P, 25.f, 12, Color, false, EditorPreviewDuration, 0.f, 2.f);
		
		if (i + 1 < Num())
		{
			DrawDebugLine(GetWorld(), P, GetWorldPoint(i + 1), Color, false, EditorPreviewDuration, 0.f, 2.f);
		}
	}
}
#endif
