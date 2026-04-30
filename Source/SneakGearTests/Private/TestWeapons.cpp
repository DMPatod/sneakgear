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
	AmmoType = EAmmoType::Light;
}

UTestWeaponFireModeComponent* ATestWeapon::GetTestFireMode() const
{
	return Cast<UTestWeaponFireModeComponent>(PrimaryFireMode);
}

ATestDelayedReloadWeapon::ATestDelayedReloadWeapon()
{
}

ATestDamageWeapon::ATestDamageWeapon()
{
	PrimaryFireModeClass = UTestDamageWeaponFireModeComponent::StaticClass();
	ClipSize = 3;
	AmmoType = EAmmoType::Light;
}

ATestHitscanWeapon::ATestHitscanWeapon()
{
	PrimaryFireModeClass = UTestWeaponFireModeComponent::StaticClass();
	ClipSize = 3;
	AmmoType = EAmmoType::Light;
	bDrawDebug = false;
}

UTestWeaponFireModeComponent* ATestHitscanWeapon::GetTestFireMode() const
{
	return Cast<UTestWeaponFireModeComponent>(PrimaryFireMode);
}
