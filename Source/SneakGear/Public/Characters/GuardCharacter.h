#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GuardCharacter.generated.h"

class UGameplayEffect;
class UHealthAttributeSet;
class APatrolPath;

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGuardCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystem;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float Awareness = 0.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision")
	float VisionRange = 1500.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Hearing")
	float HearingRange = 1200.f;

	bool bHasLineOfSight = false;

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0", ClampMax="180"))
	float VisionHalfAngleDeg = 45.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float AwarenessGainPerSecond = 0.65f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float AwarenessDecayPerSecond = 0.35;

	UPROPERTY(EditAnywhere, Category="Stealth")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditInstanceOnly, Category="Patrol")
	TObjectPtr<APatrolPath> PatrolPath;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UHealthAttributeSet> HealthSet;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> GE_DefaultHealth;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool CanSeeTarget(const AActor* Target, float& OutVisionScore) const;
	void DrawDebugVision(const AActor* Target, bool bCanSee, float VisionScore) const;
	
private:
	void InitGAS();
	void BindHealthDeath();
};
