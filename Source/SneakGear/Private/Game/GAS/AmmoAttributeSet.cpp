#include "Game/GAS/AmmoAttributeSet.h"

#include "GameplayEffectExtension.h"

void UAmmoAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetMaxAmmoAttribute())
	{
		SetMaxAmmo(FMath::Max(0.f, GetMaxAmmo()));
		SetAmmo(FMath::Clamp(GetAmmo(), 0.f, GetMaxAmmo()));
	}
	else if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		SetAmmo(FMath::Clamp(GetAmmo(), 0.f, GetMaxAmmo()));
	}
}
