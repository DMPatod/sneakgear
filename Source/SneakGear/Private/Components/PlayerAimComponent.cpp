#include "Components/PlayerAimComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Data/PlayerTuningData.h"

UPlayerAimComponent::UPlayerAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerAimComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentTurnScalar = 1.f;
}

void UPlayerAimComponent::Initialize(USpringArmComponent* InCameraBoom, UCameraComponent* InThirdPersonCamera,
                                     UCameraComponent* InFirstPersonCamera, const UPlayerTuningData* InTuningData)
{
	CameraBoom = InCameraBoom;
	ThirdPersonCamera = InThirdPersonCamera;
	FirstPersonCamera = InFirstPersonCamera;
	TuningData = InTuningData;
}

void UPlayerAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateAim(DeltaTime);
}

void UPlayerAimComponent::StartAim()
{
	bIsAiming = true;

	if (!bAimFirstPerson && ThirdPersonCamera && FirstPersonCamera)
	{
		ThirdPersonCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

void UPlayerAimComponent::StopAim()
{
	bIsAiming = false;
	bAimFirstPerson = false;

	if (ThirdPersonCamera && FirstPersonCamera)
	{
		ThirdPersonCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

void UPlayerAimComponent::ToggleAimView()
{
	if (!bIsAiming)
	{
		return;
	}

	bAimFirstPerson = !bAimFirstPerson;

	if (ThirdPersonCamera && FirstPersonCamera)
	{
		if (bAimFirstPerson)
		{
			FirstPersonCamera->SetActive(true);
			ThirdPersonCamera->SetActive(false);
		}
		else
		{
			ThirdPersonCamera->SetActive(true);
			FirstPersonCamera->SetActive(false);
		}
	}
}

void UPlayerAimComponent::UpdateAim(float DeltaTime)
{
	if (!CameraBoom || (!ThirdPersonCamera && !FirstPersonCamera))
	{
		return;
	}

	const FPlayerAimTuning& Aim = TuningData ? TuningData->Aim : FPlayerAimTuning();
	const bool bTPS = !bAimFirstPerson;

	const FVector TargetOffset = bIsAiming ? (bTPS ? Aim.OverTheShoulderOffset_Aim : FVector::ZeroVector)
	                                       : Aim.OverTheShoulderOffset_Normal;

	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaTime, Aim.AimInterpolationSpeed);

	const float TargetFOV = !bIsAiming ? Aim.NormalFOV : (bAimFirstPerson ? Aim.AimFOV_FirstPerson : Aim.AimFOV_ThirdPerson);

	if (ThirdPersonCamera && ThirdPersonCamera->IsActive())
	{
		ThirdPersonCamera->SetFieldOfView(FMath::FInterpTo(ThirdPersonCamera->FieldOfView, TargetFOV, DeltaTime,
		                                                  Aim.AimInterpolationSpeed));
	}
	if (FirstPersonCamera && FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->SetFieldOfView(FMath::FInterpTo(FirstPersonCamera->FieldOfView, TargetFOV, DeltaTime,
		                                                  Aim.AimInterpolationSpeed));
	}

	const float TargetTurn = bIsAiming ? Aim.AimTurnRate : Aim.NormalTurnRate;
	CurrentTurnScalar = FMath::FInterpTo(CurrentTurnScalar, TargetTurn, DeltaTime, 12.f);
}
