#include "Weapon/RealWeaponBase.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ARealWeaponBase::ARealWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentSpreadDegrees = MinSpreadDegrees;
}

void ARealWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentSpreadDegrees = FMath::Max(MinSpreadDegrees, CurrentSpreadDegrees - SpreadRecoveryPerSecond * DeltaSeconds);
}

void ARealWeaponBase::StartFire()
{
	const auto Interval = FireRate > 0.f ? 1.f / FireRate : 0.1f;
	GetWorldTimerManager().SetTimer(FireTimer, this, &ARealWeaponBase::FireOnce, Interval, true);
	FireOnce();
}

void ARealWeaponBase::StopFire()
{
	Super::StopFire();
}

void ARealWeaponBase::FireOnce()
{
	if (!WeaponMesh)
	{
		return;
	}

	auto* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	auto* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter)
	{
		return;
	}

	auto* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		return;
	}

	auto* World = GetWorld();
	if (!World)
	{
		return;
	}

	const auto CameraLocation = CameraComponent->GetComponentLocation();
	const auto CameraDirection = CameraComponent->GetForwardVector().GetSafeNormal();
	const auto CameraEnd = CameraLocation + CameraDirection * Range;

	auto CollisionParams = FCollisionQueryParams(SCENE_QUERY_STAT(RealWeaponCameraTrace), true);
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OwnerPawn);

	auto CameraHit = FHitResult();
	const auto bCameraHit = World->LineTraceSingleByChannel(
		CameraHit,
		CameraLocation,
		CameraEnd,
		ECC_Visibility,
		CollisionParams);
	const auto AimPoint = bCameraHit ? CameraHit.ImpactPoint : CameraEnd;

	const auto MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
	const auto ShotDirection = BuildShotDirection(MuzzleLocation, AimPoint);

	auto TraceStart = MuzzleLocation;
	auto RemainingRange = Range;
	auto Damage = BaseDamage;
	auto PenetrationCount = 0;

	while (RemainingRange > KINDA_SMALL_NUMBER && Damage >= MinimumDamageAfterPenetration)
	{
		auto FireHit = FHitResult();
		const auto TraceEnd = TraceStart + ShotDirection * RemainingRange;
		const auto bHit = World->LineTraceSingleByChannel(
			FireHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			CollisionParams);

		if (bDrawDebug)
		{
			DrawDebugLine(World, TraceStart, bHit ? FireHit.ImpactPoint : TraceEnd, bHit ? FColor::Red : FColor::Green,
			              false, 1.f, 0, 1.f);
			if (bHit)
			{
				DrawDebugSphere(World, FireHit.ImpactPoint, 5.f, 8, FColor::Red, false, 1.f);
			}
		}

		if (!bHit)
		{
			break;
		}

		if (auto* HitActor = FireHit.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(
				HitActor,
				Damage,
				ShotDirection,
				FireHit,
				OwnerPawn->GetController(),
				this,
				DamageType);

			CollisionParams.AddIgnoredActor(HitActor);
		}

		const auto TravelDistance = FVector::Distance(TraceStart, FireHit.ImpactPoint);
		RemainingRange -= TravelDistance;

		if (PenetrationCount >= MaxPenetrations)
		{
			break;
		}

		++PenetrationCount;
		Damage *= PenetrationDamageMultiplier;
		TraceStart = FireHit.ImpactPoint + ShotDirection * PenetrationAdvanceDistance;
		RemainingRange -= PenetrationAdvanceDistance;
	}

	ApplyRecoil();
	CurrentSpreadDegrees = FMath::Min(MaxSpreadDegrees, CurrentSpreadDegrees + SpreadIncreasePerShot);
	OnWeaponFiredEvent().Broadcast();
}

void ARealWeaponBase::ApplyRecoil() const
{
	auto* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	auto* Controller = Cast<APlayerController>(OwnerPawn->GetController());
	if (!Controller)
	{
		return;
	}

	const auto PitchKick = FMath::RandRange(RecoilPitchMin, RecoilPitchMax);
	const auto YawKick = FMath::RandRange(RecoilYawMin, RecoilYawMax);

	Controller->AddPitchInput(-PitchKick);
	Controller->AddYawInput(YawKick);
}

FVector ARealWeaponBase::BuildShotDirection(const FVector& MuzzleLocation, const FVector& AimPoint) const
{
	auto Direction = (AimPoint - MuzzleLocation).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = FVector::ForwardVector;
	}

	const auto SpreadRadians = FMath::DegreesToRadians(CurrentSpreadDegrees);
	return FMath::VRandCone(Direction, SpreadRadians).GetSafeNormal();
}
