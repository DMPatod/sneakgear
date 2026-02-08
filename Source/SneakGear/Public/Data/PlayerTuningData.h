#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerTuningData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerAimTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float NormalFOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float AimFOV_FirstPerson = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float AimFOV_ThirdPerson = 72.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float AimInterpolationSpeed = 18.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float NormalTurnRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	float AimTurnRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	FVector OverTheShoulderOffset_Normal = FVector(0.f, 50.f, 75.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aim")
	FVector OverTheShoulderOffset_Aim = FVector(0.f, 70.f, 60.f);
};

USTRUCT(BlueprintType)
struct FPlayerMovementTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float CapsuleRadius = 42.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float CapsuleHalfHeight = 96.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float MaxWalkSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float JumpZVelocity = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float AirControl = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float RotationRateYaw = 540.f;
};

USTRUCT(BlueprintType)
struct FPlayerCameraTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float CameraBoomLength = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float CameraLagSpeed = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	FVector CameraBoomOffset = FVector(0.f, 0.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	FVector FirstPersonCameraOffset = FVector(0.f, 8.f, 4.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	FName FirstPersonCameraSocket = "first_person_camera_attachment";
};

UCLASS(BlueprintType)
class SNEAKGEAR_API UPlayerTuningData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tuning")
	FPlayerAimTuning Aim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tuning")
	FPlayerMovementTuning Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tuning")
	FPlayerCameraTuning Camera;
};
