#include "Components/AI/GuardPatrolComponent.h"

#include "AI/GuardAIController.h"
#include "AI/PatrolPath.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

UGuardPatrolComponent::UGuardPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGuardPatrolComponent::SetPatrolPath(APatrolPath* NewPatrolPath)
{
	PatrolPath = NewPatrolPath;
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	ApplyToController(OwnerPawn ? OwnerPawn->GetController() : nullptr);
}

APatrolPath* UGuardPatrolComponent::GetPatrolPath() const
{
	return PatrolPath;
}

void UGuardPatrolComponent::ApplyToController(AController* InController) const
{
	if (auto* GuardController = Cast<AGuardAIController>(InController))
	{
		GuardController->SetPatrolPath(PatrolPath);
		GuardController->MovetoNextPoint();
	}
}

#if WITH_EDITOR
void UGuardPatrolComponent::DrawEditorPreview() const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !bShowPatrolPathInEditor || !PatrolPath || PatrolPath->Num() <= 0)
	{
		return;
	}

	const uint32 Hash = GetTypeHash(Owner->GetFName());
	const FColor PathColor(
		64 + (Hash & 0x7F),
		64 + ((Hash >> 8) & 0x7F),
		64 + ((Hash >> 16) & 0x7F));

	const FVector GuardHead = Owner->GetActorLocation() + FVector(0.f, 0.f, 70.f);
	DrawDebugLine(Owner->GetWorld(), GuardHead, PatrolPath->GetWorldPoint(0), PathColor, false, PatrolPathPreviewDuration,
	              0, 2.f);

	for (int32 Index = 0; Index < PatrolPath->Num(); ++Index)
	{
		const FVector Point = PatrolPath->GetWorldPoint(Index);
		const FColor PointColor = PatrolPath->GetWaypointAction(Index) ? FColor::Cyan : PathColor;
		DrawDebugSphere(Owner->GetWorld(), Point, 30.f, 12, PointColor, false, PatrolPathPreviewDuration, 0, 2.f);

		if (Index + 1 < PatrolPath->Num())
		{
			DrawDebugLine(Owner->GetWorld(), Point, PatrolPath->GetWorldPoint(Index + 1), PointColor, false,
			              PatrolPathPreviewDuration, 0, 2.f);
		}
	}
}
#endif
