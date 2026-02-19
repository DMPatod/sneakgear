#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "GuardCharacter.generated.h"

class UGameplayEffect;
class UHealthAttributeSet;
class APatrolPath;
class UGuardArchetypeData;

UENUM(BlueprintType)
enum class EGuardAwarenessState : uint8
{
	Calm,
	Suspicious,
	Alerted
};

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AGuardCharacter();

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float Awareness = 0.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision")
	float VisionRange = 1500.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Hearing")
	float HearingRange = 1200.f;

	bool bHasLineOfSight = false;

	UFUNCTION(BlueprintCallable, Category="Stealth")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category="Patrol")
	void SetPatrolPath(APatrolPath* NewPatrolPath);

	UFUNCTION(BlueprintCallable, Category="Stealth|Awareness")
	void AddAwareness(float DeltaAwareness);

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditInstanceOnly, Category="Spawn")
	bool bSpawnAtLevelStart = true;

	UPROPERTY(EditInstanceOnly, Category="Patrol|Editor")
	bool bShowPatrolPathInEditor = false;

	UPROPERTY(EditInstanceOnly, Category="Patrol|Editor", meta=(ClampMin="0.0"))
	float PatrolPathPreviewDuration = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Stealth|Archetype")
	TObjectPtr<UGuardArchetypeData> ArchetypeData;

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
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	bool CanSeeTarget(const AActor* Target, float& OutVisionScore) const;
	void DrawDebugVision(const AActor* Target, bool bCanSee, float VisionScore) const;
	void ApplyArchetypeData();
	EGuardAwarenessState ResolveAwarenessState(float InAwareness) const;
	void UpdateAwarenessStateAndEmitEvent();
	
private:
	virtual void InitGAS() override;
	virtual void BindHealthDeath() override;

	UPROPERTY(VisibleAnywhere, Category="Stealth|Awareness")
	EGuardAwarenessState AwarenessState = EGuardAwarenessState::Calm;
};
