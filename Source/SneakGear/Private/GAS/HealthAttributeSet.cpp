#include "GAS/HealthAttributeSet.h"
#include "GameplayEffectExtension.h"

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float ClampedHealth = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		SetHealth(ClampedHealth);
	}
}
