#pragma once

#include "CoreMinimal.h"
#include "Types/GuardAwarenessTypes.h"
#include "Characters/BaseCharacter.h"
#include "Weapon/WeaponAimProvider.h"
#include "GuardCharacter.generated.h"

class UGameplayEffect;
class UHealthAttributeSet;
class APatrolPath;
class UGuardArchetypeData;
class UGuardAwarenessComponent;
class UGuardPatrolComponent;
class UCharacterWeaponComponent;
class UBehaviorTree;
class UBlackboardData;

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ABaseCharacter, public IWeaponAimProvider
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

	UFUNCTION(BlueprintPure, Category="Stealth")
	AActor* GetTargetActor() const;

	virtual AWeaponBase* GetCurrentWeapon() const override;
	virtual bool GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const override;

	UFUNCTION(BlueprintPure, Category="AI|BehaviorTree")
	UBehaviorTree* GetBehaviorTreeAsset() const
	{
		return BehaviorTreeAsset;
	}

	UFUNCTION(BlueprintPure, Category="AI|BehaviorTree")
	UBlackboardData* GetBlackboardAsset() const
	{
		return BlackboardAsset;
	}

protected:
	UPROPERTY(EditInstanceOnly, Category="Spawn")
	bool bSpawnAtLevelStart = true;

	UPROPERTY(EditDefaultsOnly, Category="Stealth|Archetype")
	TObjectPtr<UGuardArchetypeData> ArchetypeData;

	UPROPERTY(EditDefaultsOnly, Category="AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category="AI|BehaviorTree")
	TObjectPtr<UBlackboardData> BlackboardAsset;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnCharacterDeath() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGuardAwarenessComponent> AwarenessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGuardPatrolComponent> PatrolComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCharacterWeaponComponent> WeaponComponent;

	bool bIsFiringAtTarget = false;

	void UpdateCombatFromAwareness();
};
