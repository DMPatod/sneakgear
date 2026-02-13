#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UHealthAttributeSet;
class UStaminaAttributeSet;

UCLASS(Abstract)
class SNEAKGEAR_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	const UHealthAttributeSet* GetHealthSet() const
	{
		return HealthSet;
	}

	const UStaminaAttributeSet* GetStaminaSet() const
	{
		return StaminaSet;
	}

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

	virtual void InitGAS();
	virtual void BindHealthDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UStaminaAttributeSet> StaminaSet;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultHealth;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultStamina;
};
