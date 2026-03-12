#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UAmmoAttributeSet;
class UHealthAttributeSet;
class UStaminaAttributeSet;
class AWeaponBase;

UCLASS(Abstract)
class SNEAKGEAR_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual AWeaponBase* GetCurrentWeapon() const;

	const UHealthAttributeSet* GetHealthSet() const
	{
		return HealthSet;
	}

	const UStaminaAttributeSet* GetStaminaSet() const
	{
		return StaminaSet;
	}

	const UAmmoAttributeSet* GetAmmoSet() const
	{
		return AmmoSet;
	}

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void InitGAS();
	virtual void BindHealthDeath();
	virtual void OnCharacterDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UStaminaAttributeSet> StaminaSet;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAmmoAttributeSet> AmmoSet;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultHealth;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultStamina;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultAmmo;

	UPROPERTY(Transient)
	bool bHasDied = false;
};
