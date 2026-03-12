#include "Guards/Patrol/PatrolPath.h"

#include "DrawDebugHelpers.h"
#include "Misc/DataValidation.h"

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

EDataValidationResult APatrolPath::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (Waypoints.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("Patrol path must define at least one waypoint.")));
		return EDataValidationResult::Invalid;
	}

	for (int32 Index = 0; Index < Waypoints.Num(); ++Index)
	{
		const FVector& Waypoint = Waypoints[Index];
		if (!FMath::IsFinite(Waypoint.X) || !FMath::IsFinite(Waypoint.Y) || !FMath::IsFinite(Waypoint.Z))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Waypoint %d contains a non-finite location."), Index)));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
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
