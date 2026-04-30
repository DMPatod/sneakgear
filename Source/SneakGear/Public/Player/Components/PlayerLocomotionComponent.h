#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PlayerCharacterBase.h"
#include "PlayerLocomotionComponent.generated.h"

class USpringArmComponent;
struct FInputActionValue;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerLocomotionComponent();

	void Initialize(USpringArmComponent* InCameraBoom);
	void SetupMovementDefaults();
	void TickLocomotion(float DeltaSeconds);

	void Move(const FInputActionValue& Value);
	void SetStance(EStance NewStance);
	void ToggleSprint();
	void OnStancePressed();
	void OnStanceReleased();
	void UpdateRotationMode(bool bIsAiming);
	void RefreshMovementState();

	float GetMaxSpeed() const;

private:
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Input")
	float ProneHoldTime = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Capsule")
	float StandingHalfHeight = 88.f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Capsule")
	float CrouchHalfHeight = 58.f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Capsule")
	float ProneHalfHeight = 35.f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Speed")
	float WalkSpeed = 480.f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Speed")
	float CrouchSpeed = 140.f;

	UPROPERTY(EditDefaultsOnly, Category="Stance|Speed")
	float ProneSpeed = 80.f;

	bool bIsSprinting = false;
	FTimerHandle StanceHoldTimer;
	bool bStanceHoldTriggered = false;
	bool bStanceButtonDown = false;
	FVector StandingMeshRelativeLocation = FVector::ZeroVector;
	float StandingMeshCapsuleHalfHeight = 0.f;
	bool bHasStandingMeshRelativeLocation = false;

	void HandleStanceHold();
	void UpdateCameraSocketOffset(float DeltaSeconds);
	void UpdateMeshRelativeLocation(float TargetHalfHeight);
	void UpdateMovementSpeed();
	float GetStanceBaseSpeed() const;
	bool IsCoverActive() const;
	bool CanResizeCapsuleTo(float TargetHalfHeight) const;
	APlayerCharacterBase* GetPlayerCharacter() const;
};
