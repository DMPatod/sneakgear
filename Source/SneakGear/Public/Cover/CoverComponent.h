#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoverComponent.generated.h"

USTRUCT(BlueprintType)
struct FCoverHit
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bValid = false;

	UPROPERTY(BlueprintReadOnly)
	FVector ImpactPoint = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector Normal = FVector::ForwardVector;

	UPROPERTY(BlueprintReadOnly)
	FVector Tangent = FVector::RightVector;

	UPROPERTY(BlueprintReadOnly)
	FVector SnapLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FRotator SnapRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly)
	float DistanceToWall = 99999.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UCoverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCoverComponent();

	UPROPERTY(EditAnywhere, Category="Cover")
	float ScanDistance = 120.f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float ScanRadius = 30.f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float ScanHeight = 60.f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float MaxWallSlopeZ = 0.2f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float EnterDotThreshold = 0.35;

	UPROPERTY(EditAnywhere, Category="Cover")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category="Cover")
	bool bDrawDebug = true;
	
	FCoverHit FindCover() const;
private:
	ACharacter* GetOwnerCharacter() const;
};
