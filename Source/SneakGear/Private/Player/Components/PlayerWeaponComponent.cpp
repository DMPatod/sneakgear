#include "Player/Components/PlayerWeaponComponent.h"

#include "Player/PlayerCharacterBase.h"

UPlayerWeaponComponent::UPlayerWeaponComponent()
{
}

bool UPlayerWeaponComponent::CanReload() const
{
	if (!Super::CanReload())
	{
		return false;
	}

	const int32 MissingAmmo = FMath::Max(GetClipSize() - InClip, 0);
	return MissingAmmo > 0 && FMath::FloorToInt(GetPlayerArmor()) > 0;
}

void UPlayerWeaponComponent::FinishReload()
{
	auto* PlayerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!PlayerCharacter)
	{
		Super::FinishReload();
		return;
	}

	const int32 MissingAmmo = FMath::Max(GetClipSize() - InClip, 0);
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
	const auto* PlayerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	return PlayerCharacter ? PlayerCharacter->GetAmmo() : 0.f;
}
