#include "Weapon/UnarmedWeapon.h"

#include "Misc/DataValidation.h"

AUnarmedWeapon::AUnarmedWeapon()
{
	ClipSize = 1;
	AmmoType = EAmmoType::None;
}

void AUnarmedWeapon::StartFire()
{
}

void AUnarmedWeapon::StopFire()
{
}

float AUnarmedWeapon::Reload()
{
	StopFire();
	HandleReloadFinished();
	return 0.f;
}

EDataValidationResult AUnarmedWeapon::IsDataValid(FDataValidationContext& Context) const
{
	(void)Context;
	return EDataValidationResult::Valid;
}
