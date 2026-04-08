#include "Weapon/HitscanWeaponBase.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"
#include "Weapon/WeaponFireModeComponent.h"

AHitscanWeaponBase::AHitscanWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentSpreadDegrees = MinSpreadDegrees;
}

void AHitscanWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentSpreadDegrees = FMath::Max(MinSpreadDegrees, CurrentSpreadDegrees - SpreadRecoveryPerSecond * DeltaSeconds);
}

void AHitscanWeaponBase::StartFire()
{
	Super::StartFire();
}

void AHitscanWeaponBase::StopFire()
{
	Super::StopFire();
}

float AHitscanWeaponBase::Reload()
{
	return Super::Reload();
}

EDataValidationResult AHitscanWeaponBase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (BaseDamage <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("BaseDamage must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (Range <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("Range must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (MaxSpreadDegrees < MinSpreadDegrees)
	{
		Context.AddError(FText::FromString(TEXT("MaxSpreadDegrees must be greater than or equal to MinSpreadDegrees.")));
		Result = EDataValidationResult::Invalid;
	}

	if (PenetrationDamageMultiplier <= 0.f || PenetrationDamageMultiplier > 1.f)
	{
		Context.AddError(FText::FromString(TEXT("PenetrationDamageMultiplier must be in the range (0, 1].")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void AHitscanWeaponBase::FireOnce()
{
	if (!WeaponMesh)
	{
		return;
	}

	FWeaponFireContext Context;
	if (!BuildFireContext(Context))
	{
		return;
	}

	auto* World = GetWorld();
	if (!World)
	{
		return;
	}

	const auto AimEnd = Context.AimOrigin + Context.AimDirection * Range;

	auto CollisionParams = FCollisionQueryParams(SCENE_QUERY_STAT(RealWeaponCameraTrace), true);
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Context.InstigatorPawn);

	auto CameraHit = FHitResult();
	const auto bAimHit = World->LineTraceSingleByChannel(
		CameraHit,
		Context.AimOrigin,
		AimEnd,
		ECC_Visibility,
		CollisionParams);
	const auto AimPoint = bAimHit ? CameraHit.ImpactPoint : AimEnd;

	const auto MuzzleLocation = WeaponMesh->GetSocketLocation(Context.MuzzleSocket);
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
				Context.InstigatorPawn->GetController(),
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

void AHitscanWeaponBase::ApplyRecoil() const
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

FVector AHitscanWeaponBase::BuildShotDirection(const FVector& MuzzleLocation, const FVector& AimPoint) const
{
	auto Direction = (AimPoint - MuzzleLocation).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = FVector::ForwardVector;
	}

	const auto SpreadRadians = FMath::DegreesToRadians(CurrentSpreadDegrees);
	return FMath::VRandCone(Direction, SpreadRadians).GetSafeNormal();
}
