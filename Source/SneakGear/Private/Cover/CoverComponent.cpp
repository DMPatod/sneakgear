#include "Cover/CoverComponent.h"

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

	if (Speed2D < 10.f)
	{
		return Out;
	}

	auto MoveDir = Velocity2D / Speed2D;
	auto TowardWallDot = FVector::DotProduct(MoveDir, -Normal);
	if (TowardWallDot < EnterDotThreshold)
	{
		return Out;
	}

	auto Tangent = FVector::CrossProduct(FVector::UpVector, Normal).GetSafeNormal();

	auto Snap = Hit.ImpactPoint + Normal * (CapsuleRadius + 2.f);

	Out.bValid = true;
	Out.ImpactPoint = Hit.ImpactPoint;
	Out.Normal = Normal;
	Out.Tangent = Tangent;
	Out.SnapLocation = Snap;
	Out.SnapRotation = Tangent.Rotation();
	Out.DistanceToWall = FVector::Dist(Hit.ImpactPoint, Start);

	if (bDrawDebug)
	{
		DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + Normal * 80.f, 20.f, FColor::Cyan, false,
		                          0.f, 2.f);
		DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + Tangent * 80.f, 20.f, FColor::Yellow, false,
		                          0.f, 2.f);
	}

	return Out;
}

ACharacter* UCoverComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}
