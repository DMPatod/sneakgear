#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GuardArchetypeData.generated.h"

UCLASS(BlueprintType)
class SNEAKGEAR_API UGuardArchetypeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0", ClampMax="1.0"))
	float InitialAwareness = 0.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0.0"))
	float VisionRange = 1500.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Vision", meta=(ClampMin="0.0", ClampMax="180.0"))
	float VisionHalfAngleDeg = 45.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Hearing", meta=(ClampMin="0.0"))
	float HearingRange = 1200.f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0"))
	float AwarenessGainPerSecond = 0.65f;

	UPROPERTY(EditAnywhere, Category="Stealth|Awareness", meta=(ClampMin="0.0"))
	float AwarenessDecayPerSecond = 0.35f;

	UPROPERTY(EditAnywhere, Category="Combat", meta=(ClampMin="0.0"))
	float ReactionTimeSeconds = 0.2f;

	UPROPERTY(EditAnywhere, Category="Combat", meta=(ClampMin="0.0", ClampMax="45.0"))
	float AimErrorDegrees = 2.0f;

	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
};
