#include "Weapon/UnarmedWeapon.h"

#include "Misc/DataValidation.h"

AUnarmedWeapon::AUnarmedWeapon()
{
	ClipSize = 1;
	FireRate = 1.f;
	AmmoType = EAmmoType::None;
}

void AUnarmedWeapon::StartFire()
{
}

void AUnarmedWeapon::StopFire()
{
}

EDataValidationResult AUnarmedWeapon::IsDataValid(FDataValidationContext& Context) const
{
	(void)Context;
	return EDataValidationResult::Valid;
}
