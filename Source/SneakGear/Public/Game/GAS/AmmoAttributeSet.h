#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AmmoAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SNEAKGEAR_API UAmmoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Ammo")
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UAmmoAttributeSet, Ammo)

	UPROPERTY(BlueprintReadOnly, Category="Ammo")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UAmmoAttributeSet, MaxAmmo)

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
