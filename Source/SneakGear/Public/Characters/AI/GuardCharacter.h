#pragma once

#include "CoreMinimal.h"
#include "AI/GuardAwarenessTypes.h"
#include "Characters/BaseCharacter.h"
#include "GuardCharacter.generated.h"

class UGameplayEffect;
class UHealthAttributeSet;
class APatrolPath;
class UGuardArchetypeData;
class UGuardAwarenessComponent;
class UGuardPatrolComponent;
class UCharacterWeaponComponent;

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AGuardCharacter();

	UFUNCTION(BlueprintCallable, Category="Stealth")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category="Patrol")
	void SetPatrolPath(APatrolPath* NewPatrolPath);

	UFUNCTION(BlueprintCallable, Category="Stealth|Awareness")
	void AddAwareness(float DeltaAwareness);

	UFUNCTION(BlueprintPure, Category="Stealth|Awareness")
	float GetAwareness() const;

	UFUNCTION(BlueprintPure, Category="Stealth|Vision")
	float GetVisionRange() const;

	UFUNCTION(BlueprintPure, Category="Stealth|Hearing")
	float GetHearingRange() const;

	UFUNCTION(BlueprintPure, Category="Stealth|Awareness")
	bool HasLineOfSight() const;

	UFUNCTION(BlueprintPure, Category="Stealth|Awareness")
	EGuardAwarenessState GetAwarenessState() const;

protected:
	UPROPERTY(EditInstanceOnly, Category="Spawn")
	bool bSpawnAtLevelStart = true;

	UPROPERTY(EditDefaultsOnly, Category="Stealth|Archetype")
	TObjectPtr<UGuardArchetypeData> ArchetypeData;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	virtual void InitGAS() override;
	virtual void BindHealthDeath() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGuardAwarenessComponent> AwarenessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGuardPatrolComponent> PatrolComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCharacterWeaponComponent> WeaponComponent;

	bool bIsFiringAtTarget = false;

	void UpdateCombatFromAwareness();
};
