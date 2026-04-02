#include "Components/Cover/CoverComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UCoverComponent::UCoverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FCoverHit UCoverComponent::FindCover() const
{
	FCoverHit Out;

	auto Character = GetOwnerCharacter();
	if (!Character)
	{
		return Out;
	}

	auto World = GetWorld();
	if (!World)
	{
		return Out;
	}

	auto Capsule = Character->GetCapsuleComponent();
	auto CapsuleRadius = Capsule ? Capsule->GetScaledCapsuleRadius() : 42.f;

	auto ActorLocation = Character->GetActorLocation();
	auto Start = ActorLocation + FVector(0, 0, ScanHeight);
	auto Forward = Character->GetActorForwardVector();
	auto End = Start + Forward * ScanDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CoverScan), false);
	Params.AddIgnoredActor(Character);

	FHitResult Hit;
	auto bHit = World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, TraceChannel,
	                                        FCollisionShape::MakeSphere(ScanRadius), Params);

	if (bDrawDebug)
	{
		auto Color = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(World, Start, End, Color, false, 0.f, 0, 1.f);
		DrawDebugSphere(World, bHit ? Hit.ImpactPoint : End, ScanRadius, 12, Color, false, 0.f, 0.f);
	}

	if (!bHit)
	{
		return Out;
	}

	auto Normal = Hit.ImpactNormal.GetSafeNormal();

	if (FMath::Abs(Normal.Z) >= MaxWallSlopeZ)
	{
		return Out;
	}

	auto Velocity = Character->GetVelocity();
	auto Velocity2D = FVector(Velocity.X, Velocity.Y, 0.f);
	auto Speed2D = Velocity2D.Size();

	if (Speed2D < 7.5f)
	{
		return Out;
	}

	auto MoveDir = Velocity2D / Speed2D;
	auto TowardWallDot = FVector::DotProduct(MoveDir, -Normal);
	
	UE_LOG(LogTemp, Warning, TEXT("TowardWallDot: %f"), TowardWallDot);
	if (TowardWallDot < EnterDotThreshold)
	{
		return Out;
	}

	auto Tangent = FVector::CrossProduct(FVector::UpVector, Normal).GetSafeNormal();

	auto Snap = Hit.ImpactPoint + Normal * (CapsuleRadius + 2.f);

	const float CapsuleHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 96.f;
	const float FeetZ = ActorLocation.Z - CapsuleHalfHeight;
	float ObstacleHeight = 0.f;

	{
		FCollisionQueryParams TopParams(SCENE_QUERY_STAT(CoverTopScan), false);
		TopParams.AddIgnoredActor(Character);

		FHitResult TopHit;
		const FVector TopTraceBase = Hit.ImpactPoint - Normal * 10.f;
		const FVector TopStart = TopTraceBase + FVector(0.f, 0.f, ScanHeight + 120.f);
		const FVector TopEnd = TopTraceBase - FVector(0.f, 0.f, 40.f);
		const bool bTopHit = World->LineTraceSingleByChannel(TopHit, TopStart, TopEnd, TraceChannel, TopParams);

		if (bDrawDebug)
		{
			DrawDebugLine(World, TopStart, TopEnd, bTopHit ? FColor::Blue : FColor::Silver, false, 0.f, 0, 1.f);
		}

		if (bTopHit)
		{
			ObstacleHeight = FMath::Max(0.f, TopHit.ImpactPoint.Z - FeetZ);
		}
	}

	Out.bValid = true;
	Out.ImpactPoint = Hit.ImpactPoint;
	Out.Normal = Normal;
	Out.Tangent = Tangent;
	Out.SnapLocation = Snap;
	Out.SnapRotation = Tangent.Rotation();
	Out.DistanceToWall = FVector::Dist(Hit.ImpactPoint, Start);
	Out.ObstacleHeight = ObstacleHeight;

	if (bDrawDebug)
	{
		DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + Normal * 80.f, 20.f, FColor::Cyan, false,
		                          0.f, 2.f);
		DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + Tangent * 80.f, 20.f, FColor::Yellow, false,
		                          0.f, 2.f);
	}

	return Out;
}

bool UCoverComponent::ValidateCover(const FVector& ExpectedNormal, float MaxDistance) const
{
	auto Character = GetOwnerCharacter();
	if (!Character)
	{
		return false;
	}

	auto World = GetWorld();
	if (!World)
	{
		return false;
	}

	auto Normal = ExpectedNormal.GetSafeNormal();

	auto Start = Character->GetActorLocation();
	auto End = Start - Normal * MaxDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CoverValidate), false);
	Params.AddIgnoredActor(Character);

	FHitResult Hit;
	auto bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);
	if (bDrawDebug)
	{
		DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);
	}

	if (!bHit)
	{
		return false;
	}

	auto HitNormal = Hit.ImpactNormal.GetSafeNormal();
	if (FMath::Abs(HitNormal.Z) >= MaxWallSlopeZ)
	{
		return false;
	}

	auto NormalDot = FVector::DotProduct(HitNormal, Normal);
	return NormalDot > ValidateCoverNormalThreshold;
}

ACharacter* UCoverComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}
