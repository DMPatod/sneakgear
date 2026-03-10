#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RadarTypes.generated.h"


USTRUCT(BlueprintType)
struct SNEAKGEARCORE_API FRadarContact
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D RadarPos = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float Awareness = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float VisionRange = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float HearingRange = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bHasLOS = false;

	UPROPERTY(BlueprintReadOnly)
	float LookYawOnRadarDeg = 0.f;
};
