#pragma once

#include "CoreMinimal.h"
#include "Types/GuardAwarenessTypes.h"
#include "Components/ActorComponent.h"
#include "GuardAwarenessComponent.generated.h"

class UGuardArchetypeData;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UGuardAwarenessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGuardAwarenessComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeFromArchetype(const UGuardArchetypeData* ArchetypeData);
	void SetTargetActor(AActor* NewTarget);
	void AddAwareness(float DeltaAwareness);

	float GetAwareness() const
	{
		return Awareness;
	}

	float GetVisionRange() const
	{
		return VisionRange;
	}

	float GetHearingRange() const
	{
		return HearingRange;
	}

	bool HasLineOfSight() const
	{
		return bHasLineOfSight;
	}

	AActor* GetTargetActor() const
	{
		return TargetActor;
	}

	EGuardAwarenessState GetAwarenessState() const
	{
		return AwarenessState;
	}

private:
	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0", ClampMax="1.0"))
	float Awareness = 0.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0.0"))
	float VisionRange = 1500.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Hearing", meta=(ClampMin="0.0"))
	float HearingRange = 1200.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0", ClampMax="180"))
	float VisionHalfAngleDeg = 45.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0"))
	float AwarenessGainPerSecond = 0.65f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0"))
	float AwarenessDecayPerSecond = 0.35f;

	UPROPERTY(EditAnywhere, Category="Stealth")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, Category="Stealth|Awareness")
	bool bHasLineOfSight = false;

	UPROPERTY(VisibleAnywhere, Category="Stealth|Awareness")
	EGuardAwarenessState AwarenessState = EGuardAwarenessState::Calm;

	EGuardAwarenessState ResolveAwarenessState(float InAwareness) const;
	void UpdateAwarenessStateAndEmitEvent();
	bool CanSeeTarget(const AActor* Target, float& OutVisionScore) const;
	void DrawDebugVision(const AActor* Target, bool bCanSee) const;
};
