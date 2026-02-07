#include "Weapon/DefaultPistolWeapon.h"

#include "Weapon/FireMode_Hitscan.h"

ADefaultPistolWeapon::ADefaultPistolWeapon()
{
	PrimaryFireModeClass = UFireMode_Hitscan::StaticClass();
}
