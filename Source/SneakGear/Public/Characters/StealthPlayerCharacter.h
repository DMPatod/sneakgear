#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonPlayerCharacter.h"
#include "Cover/CoverComponent.h"
#include "StealthPlayerCharacter.generated.h"

class UInputAction;

UCLASS()
class SNEAKGEAR_API AStealthPlayerCharacter : public AThirdPersonPlayerCharacter
{
	GENERATED_BODY()

	enum class ECoverState : uint8
	{
		None,
		Approaching,
		Locked
	};

public:
	AStealthPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	bool IsInCover() const
	{
		return CoverState == ECoverState::Locked;
	}

	float GetCoverMoveAxis() const
	{
		return CoverMoveAxis;
	}

protected:
	virtual void BeginPlay() override;

	virtual void Move(const FInputActionValue& Value) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

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
	void StartCrouch();
	void StopCrouch();

	void EnterCover(const FCoverHit& Hit);
	void LockCover();
	void ExitCover();
	void UpdateCoverApproach(float DeltaTime);
	void UpdateCoverRotation(float DeltaTime);

	FVector GetCoverTangentAlignedToCamera() const;

	float CoverApproachTime = 0.f;
	float CoverFacingSign = 1.f;
	float CoverMoveAxis = 0.f;
};
