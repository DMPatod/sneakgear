#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GuardCharacter.generated.h"

class APatrolPath;

UCLASS()
class SNEAKGEAR_API AGuardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGuardCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	bool bHasLineOfSight = false;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision")
	float VisionRange = 1500.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0", ClampMax="180"))
	float VisionHalfAngleDeg = 45.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float AwarenessGainPerSecond = 0.65f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float AwarenessDecayPerSecond = 0.35;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness")
	float Awareness = 0.f;

	UPROPERTY(EditAnywhere, Category="Stealth")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditInstanceOnly, Category="Patrol")
	TObjectPtr<APatrolPath> PatrolPath;

	virtual void BeginPlay() override;

	bool CanSeeTarget(const AActor* Target, float& OutVisionScore) const;
	void DrawDebugVision(const AActor* Target, bool bCanSee, float VisionScore) const;
};
