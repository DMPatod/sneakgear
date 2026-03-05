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

	UPROPERTY(BlueprintReadOnly)
	float ObstacleHeight = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouchHeightCover = false;
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
	float ValidateCoverNormalThreshold = 0.85f;

	UPROPERTY(EditAnywhere, Category="Cover|Vault")
	float CrouchCoverMinHeight = 40.f;

	UPROPERTY(EditAnywhere, Category="Cover|Vault")
	float CrouchCoverMaxHeight = 90.f;

	UPROPERTY(EditAnywhere, Category="Cover")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category="Cover")
	bool bDrawDebug = true;

	FCoverHit FindCover() const;

	UFUNCTION(BlueprintCallable, Category="Cover")
	bool ValidateCover(const FVector& ExpectedNormal, float MaxDistance = 80.f) const;

private:
	ACharacter* GetOwnerCharacter() const;
};
