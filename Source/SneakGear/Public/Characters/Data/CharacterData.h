#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterData.generated.h"

UCLASS()
class SNEAKGEAR_API UCharacterData : public UDataTable
{
public:
	
	
private:
	static constexpr int BaseHealthValue = 3;
	int MaxHealth = BaseHealthValue;
	int CurrentHealth = BaseHealthValue;

	static constexpr float BaseStaminaValue = 100.f;
	float MaxStamina = BaseStaminaValue;
	float CurrentStamina = BaseStaminaValue;

	GENERATED_BODY()
};
