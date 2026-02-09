#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cover/CoverComponent.h"
#include "CoverStateComponent.generated.h"

struct FInputActionValue;

UENUM(BlueprintType)
enum class ECoverState : uint8
{
	None,
	Approaching,
	Locked
};

UCLASS(ClassGroup=(Cover), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UCoverStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCoverStateComponent();

	bool IsInCover() const
	{
		return CoverState == ECoverState::Locked;
	}

	float GetCoverMoveAxis() const
	{
		return CoverMoveAxis;
	}

	bool HandleMoveInput(ACharacter* OwnerCharacter, const FInputActionValue& Value);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	ECoverState CoverState = ECoverState::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	FCoverHit CurrentCover;

	UPROPERTY(EditAnywhere, Category="Cover")
	float CoverMaxSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverTurnSpeed = 18.f; // 12-20

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverAssistStrength = 8.f; // 8-12

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverLockDistance = 12.f; // 10-18

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverApproachTimeout = 0.5f; // 0.35-0.6

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverExitBackDotThreshold = 0.2; // 0.15-0.3

	UPROPERTY(EditAnywhere, Category="Cover|Tuning")
	float CoverFaceTurnSpeed = 12.f;

private:
	void EnterCover(ACharacter* OwnerCharacter, const FCoverHit& Hit);
	void LockCover(ACharacter* OwnerCharacter);
	void ExitCover(ACharacter* OwnerCharacter);
	void UpdateCoverApproach(ACharacter* OwnerCharacter, float DeltaTime);
	void UpdateCoverRotation(ACharacter* OwnerCharacter, float DeltaTime);
	FVector GetCoverTangentAlignedToCamera(const ACharacter* OwnerCharacter) const;

	UPROPERTY(Transient)
	TObjectPtr<UCoverComponent> CoverComponent;

	float CoverApproachTime = 0.f;
	float CoverFacingSign = 1.f;
	float CoverMoveAxis = 0.f;
};
