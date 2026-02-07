#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "StaminaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SNEAKGEAR_API UStaminaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category="Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, MaxStamina)

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
