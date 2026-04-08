#include "TestWeapons.h"

#include "Kismet/GameplayStatics.h"

void UTestWeaponFireModeComponent::FireOnce(const FWeaponFireContext& Context)
{
	++FireCount;
	LastContext = Context;
}

void UTestDamageWeaponFireModeComponent::FireOnce(const FWeaponFireContext& Context)
{
	if (!Context.InstigatorPawn)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FHitResult Hit;
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(TestDamageWeaponFire), true);
	CollisionParams.AddIgnoredActor(Context.InstigatorPawn);
	CollisionParams.AddIgnoredActor(Context.WeaponActor);

	const FVector TraceEnd = Context.AimOrigin + Context.AimDirection.GetSafeNormal() * Range;
	if (!World->LineTraceSingleByChannel(Hit, Context.AimOrigin, TraceEnd, ECC_Visibility, CollisionParams))
	{
		return;
	}

	if (AActor* HitActor = Hit.GetActor())
	{
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			Context.AimDirection.GetSafeNormal(),
			Hit,
			Context.InstigatorPawn->GetController(),
			Context.WeaponActor,
			UDamageType::StaticClass());
	}
}

ATestWeapon::ATestWeapon()
{
	PrimaryFireModeClass = UTestWeaponFireModeComponent::StaticClass();
	ClipSize = 3;
	FireRate = 10.f;
	AmmoType = EAmmoType::Light;
}

UTestWeaponFireModeComponent* ATestWeapon::GetTestFireMode() const
{
	return Cast<UTestWeaponFireModeComponent>(PrimaryFireMode);
}

ATestDelayedReloadWeapon::ATestDelayedReloadWeapon()
{
}

float ATestDelayedReloadWeapon::Reload()
{
	StopFire();
	ScheduleReloadCompletion(DelayedReloadDuration);
	return DelayedReloadDuration;
}

float ATestDelayedFireWeapon::GetFireInterval() const
{
	return 10.f;
}

ATestDamageWeapon::ATestDamageWeapon()
{
	PrimaryFireModeClass = UTestDamageWeaponFireModeComponent::StaticClass();
	ClipSize = 3;
	FireRate = 10.f;
	AmmoType = EAmmoType::Light;
}
