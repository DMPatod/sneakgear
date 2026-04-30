#pragma once

#include "CoreMinimal.h"
#include "Types/GuardAwarenessTypes.h"
#include "Characters/NonPlayerCharacter.h"
#include "Weapon/WeaponAimProvider.h"
#include "GuardCharacter.generated.h"

class APatrolPath;
class UGuardArchetypeData;
class UGuardAwarenessComponent;
class UGuardPatrolComponent;
class UCharacterWeaponComponent;

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ANonPlayerCharacter, public IWeaponAimProvider
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

	UFUNCTION(BlueprintCallable, Category="AI|Combat")
	void SetCombatFiringEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category="AI|Combat")
	void UpdateCombatState(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category="AI|Combat")
	bool CanStartCombatFiring() const;

	UFUNCTION(BlueprintPure, Category="AI|Combat")
	bool IsWeaponClipEmpty() const;

	UFUNCTION(BlueprintCallable, Category="AI|Combat")
	bool ReloadWeaponIfNeeded();

	UFUNCTION(BlueprintCallable, Category="AI|Combat|Animation")
	bool NotifyWeaponFireAnimation();

	UFUNCTION(BlueprintCallable, Category="AI|Combat|Animation")
	bool NotifyWeaponReloadAnimationFinished();

	UFUNCTION(BlueprintPure, Category="AI|Combat|Animation")
	bool WasWeaponFireRequestedRecently(float WindowSeconds = 0.12f) const;

	UFUNCTION(BlueprintPure, Category="AI|Combat|Animation")
	bool IsWeaponFireNotifyPending() const;

	UFUNCTION(BlueprintPure, Category="AI|Combat|Animation")
	bool IsWeaponReloading() const;

	virtual AWeaponBase* GetCurrentWeapon() const override;
	virtual bool GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const override;

	UGuardAwarenessComponent* GetAwarenessComponent() const { return AwarenessComponent; }
	UGuardPatrolComponent* GetPatrolComponent() const { return PatrolComponent; }

protected:
	UPROPERTY(EditDefaultsOnly, Category="Stealth|Archetype")
	TObjectPtr<UGuardArchetypeData> ArchetypeData;

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
	bool bHadLineOfSightLastTick = false;
	float LastLineOfSightAcquiredTimestamp = -1.f;
	float ReactionTimeSeconds = 0.2f;
	float AimErrorDegrees = 2.0f;
};
