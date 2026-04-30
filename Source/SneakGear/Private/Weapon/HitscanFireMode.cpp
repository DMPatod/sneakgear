#include "Weapon/HitscanFireMode.h"

#include "Player/SneakGearPlayerCharacter.h"
#include "Player/SneakGearPlayerController.h"
#include "Kismet/GameplayStatics.h"

namespace
{
FCollisionObjectQueryParams BuildWeaponTraceObjectParams()
{
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	return ObjectParams;
}
}

void UHitscanFireMode::FireOnce(const FWeaponFireContext& Context)
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
	const FCollisionObjectQueryParams ObjectParams = BuildWeaponTraceObjectParams();

	auto CameraHit = FHitResult();
	auto bAimHit = World->LineTraceSingleByObjectType(CameraHit, Context.AimOrigin, AimEnd, ObjectParams,
	                                                  CollisionParams);
	auto AimPoint = bAimHit ? CameraHit.ImpactPoint : AimEnd;

	auto MuzzleLocation = Context.WeaponMesh->GetSocketLocation(Context.MuzzleSocket);
	auto ShotDirection = (AimPoint - MuzzleLocation).GetSafeNormal();
	auto ShotEnd = MuzzleLocation + ShotDirection * Range;

	auto FireHit = FHitResult();
	auto bHit = World->LineTraceSingleByObjectType(FireHit, MuzzleLocation, ShotEnd, ObjectParams, CollisionParams);

	if (bHit && FireHit.GetActor())
	{
		UGameplayStatics::ApplyPointDamage(FireHit.GetActor(), Damage, ShotDirection, FireHit,
		                                   Context.InstigatorPawn->GetController(), Context.WeaponActor, DamageType);

		if (auto Controller = Cast<ASneakGearPlayerController>(Context.InstigatorPawn->GetController()))
		{
			Controller->NotifyHitMarker();
		}

	}

	if (bDrawDebug)
	{
		DrawDebugLine(World, MuzzleLocation, bHit ? FireHit.ImpactPoint : ShotEnd,
					  bHit ? FColor::Red : FColor::Green, false, 1.f, 0, 1.f);
		if (bHit)
		{
			DrawDebugSphere(World, FireHit.ImpactPoint, 7.5f, 12, FColor::Red, false, 1.f);
			UE_LOG(LogTemp, Warning, TEXT("HitscanFireMode hit %s"), *GetNameSafe(FireHit.GetActor()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitscanFireMode missed"));
		}
	}
}
