#include "Player/Components/PlayerAimComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UPlayerAimComponent::UPlayerAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerAimComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentTurnScalar = NormalTurnRate;
}

void UPlayerAimComponent::Initialize(USpringArmComponent* InCameraBoom, UCameraComponent* InThirdPersonCamera,
                                     UCameraComponent* InFirstPersonCamera)
{
	CameraBoom = InCameraBoom;
	ThirdPersonCamera = InThirdPersonCamera;
	FirstPersonCamera = InFirstPersonCamera;
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

	const bool bTPS = !bAimFirstPerson;

	const FVector TargetOffset = bIsAiming ? (bTPS ? OverTheShoulderOffsetAim : FVector::ZeroVector)
	                                       : OverTheShoulderOffsetNormal;

	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaTime, AimInterpolationSpeed);

	const float TargetFOV = !bIsAiming ? NormalFOV : (bAimFirstPerson ? AimFOVFirstPerson : AimFOVThirdPerson);

	if (ThirdPersonCamera && ThirdPersonCamera->IsActive())
	{
		ThirdPersonCamera->SetFieldOfView(FMath::FInterpTo(ThirdPersonCamera->FieldOfView, TargetFOV, DeltaTime,
		                                                  AimInterpolationSpeed));
	}
	if (FirstPersonCamera && FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->SetFieldOfView(FMath::FInterpTo(FirstPersonCamera->FieldOfView, TargetFOV, DeltaTime,
		                                                  AimInterpolationSpeed));
	}

	const float TargetTurn = bIsAiming ? AimTurnRate : NormalTurnRate;
	CurrentTurnScalar = FMath::FInterpTo(CurrentTurnScalar, TargetTurn, DeltaTime, 12.f);
}
