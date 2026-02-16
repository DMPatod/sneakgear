#include "Weapon/FireMode_Projectile.h"

#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UFireMode_Projectile::FireOnce(const FWeaponFireContext& Context)
{
	if (!Context.InstigatorPawn || !Context.WeaponMesh || !ProjectileClass)
	{
		return;
	}

	auto* World = GetWorld();
	if (!World)
	{
		return;
	}

	const auto CameraEnd = Context.CameraLocation + Context.CameraDirection * Range;

	auto CollisionParams = FCollisionQueryParams(SCENE_QUERY_STAT(FireModeProjectile), true);
	CollisionParams.AddIgnoredActor(Context.WeaponActor);
	CollisionParams.AddIgnoredActor(Context.InstigatorPawn);

	auto CameraHit = FHitResult();
	const auto bCameraHit = World->LineTraceSingleByChannel(
		CameraHit,
		Context.CameraLocation,
		CameraEnd,
		ECC_Visibility,
		CollisionParams);
	const auto AimPoint = bCameraHit ? CameraHit.ImpactPoint : CameraEnd;

	const auto MuzzleLocation = Context.WeaponMesh->GetSocketLocation(Context.MuzzleSocket);
	auto ShotDirection = (AimPoint - MuzzleLocation).GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = Context.CameraDirection.GetSafeNormal();
	}

	auto SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = Context.WeaponActor;
	SpawnParams.Instigator = Context.InstigatorPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto* Projectile = World->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, ShotDirection.Rotation(), SpawnParams);
	if (!Projectile)
	{
		return;
	}

	if (auto* ProjectileMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		const auto Speed = ProjectileMovement->InitialSpeed > 0.f
			                   ? ProjectileMovement->InitialSpeed
			                   : ProjectileMovement->MaxSpeed;
		ProjectileMovement->Velocity = ShotDirection * Speed;
	}
	else if (auto* RootPrimitive = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
		RootPrimitive && RootPrimitive->IsSimulatingPhysics())
	{
		RootPrimitive->SetPhysicsLinearVelocity(ShotDirection * 3000.f);
	}

	if (bDrawDebug)
	{
		DrawDebugLine(World, MuzzleLocation, MuzzleLocation + ShotDirection * 500.f, FColor::Cyan, false, 1.f, 0, 1.f);
		DrawDebugSphere(World, MuzzleLocation, 6.f, 12, FColor::Yellow, false, 1.f);
	}
}
