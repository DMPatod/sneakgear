#include "TestWeapons.h"

void UTestWeaponFireModeComponent::FireOnce(const FWeaponFireContext& Context)
{
	++FireCount;
	LastContext = Context;
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
