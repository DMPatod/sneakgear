#include "Player/Components/PlayerWeaponComponent.h"

#include "Player/ThirdPersonPlayerCharacter.h"

UPlayerWeaponComponent::UPlayerWeaponComponent()
{
}

void UPlayerWeaponComponent::Reload()
{
	auto* PlayerCharacter = Cast<AThirdPersonPlayerCharacter>(GetOwner());
	if (!PlayerCharacter)
	{
		Super::Reload();
		return;
	}

	const int32 MissingAmmo = FMath::Max(GetClipSize() - InClip, 0);
	if (MissingAmmo <= 0)
	{
		return;
	}

	const int32 AvailableFromArmor = FMath::Max(FMath::FloorToInt(GetPlayerArmor()), 0);
	const int32 AmmoToLoad = FMath::Min(MissingAmmo, AvailableFromArmor);
	if (AmmoToLoad <= 0)
	{
		return;
	}

	const float ConsumedArmor = PlayerCharacter->ConsumeAmmo(static_cast<float>(AmmoToLoad));
	const int32 LoadedAmmo = FMath::Clamp(FMath::FloorToInt(ConsumedArmor), 0, AmmoToLoad);
	InClip = FMath::Clamp(InClip + LoadedAmmo, 0, GetClipSize());
}

float UPlayerWeaponComponent::GetPlayerArmor() const
{
	const auto* PlayerCharacter = Cast<AThirdPersonPlayerCharacter>(GetOwner());
	return PlayerCharacter ? PlayerCharacter->GetAmmo() : 0.f;
}
