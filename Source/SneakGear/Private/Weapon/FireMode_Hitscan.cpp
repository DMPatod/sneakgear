#include "Weapon/FireMode_Hitscan.h"

#include "Player/StealthPlayerCharacter.h"
#include "Player/StealthPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UFireMode_Hitscan::FireOnce(const FWeaponFireContext& Context)
{
	if (!Context.InstigatorPawn || !Context.WeaponMesh)
	{
		return;
	}

	auto World = GetWorld();
	if (!World)
	{
		return;
	}

	auto AimEnd = Context.AimOrigin + Context.AimDirection * Range;

	auto CollisionParams = FCollisionQueryParams(SCENE_QUERY_STAT(FireModeHitscan), true);
	CollisionParams.AddIgnoredActor(Context.WeaponActor);
	CollisionParams.AddIgnoredActor(Context.InstigatorPawn);

	auto CameraHit = FHitResult();
	auto bAimHit = World->LineTraceSingleByChannel(CameraHit, Context.AimOrigin, AimEnd, ECC_Visibility,
	                                                  CollisionParams);
	auto AimPoint = bAimHit ? CameraHit.ImpactPoint : AimEnd;

	auto MuzzleLocation = Context.WeaponMesh->GetSocketLocation(Context.MuzzleSocket);
	auto ShotDirection = (AimPoint - MuzzleLocation).GetSafeNormal();
	auto ShotEnd = MuzzleLocation + ShotDirection * Range;

	auto FireHit = FHitResult();
	auto bHit = World->LineTraceSingleByChannel(FireHit, MuzzleLocation, ShotEnd, ECC_Visibility, CollisionParams);

	if (bHit && FireHit.GetActor())
	{
		UGameplayStatics::ApplyPointDamage(FireHit.GetActor(), Damage, ShotDirection, FireHit,
		                                   Context.InstigatorPawn->GetController(), Context.WeaponActor, DamageType);

		if (auto Controller = Cast<AStealthPlayerController>(Context.InstigatorPawn->GetController()))
		{
			Controller->NotifyHitMarker();
		}

		if (bDrawDebug)
		{
			DrawDebugLine(World, MuzzleLocation, bHit ? FireHit.ImpactPoint : ShotEnd,
			              bHit ? FColor::Red : FColor::Green, false, 1.f, 0, 1.f);
			if (bHit)
			{
				DrawDebugSphere(World, FireHit.ImpactPoint, 7.5f, 12, FColor::Red, false, 1.f);
			}
		}
	}
}
