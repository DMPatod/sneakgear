#include "Characters/MainPlayerCharacterBase.h"

AMainPlayerCharacterBase::AMainPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoomNormal = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomNormal"));
	CameraBoomNormal->SetupAttachment(RootComponent);
	CameraBoomNormal->TargetArmLength = 300.f;
	CameraBoomNormal->bUsePawnControlRotation = true;
	CameraBoomNormal->SocketOffset = FVector(0.f, 50.f, 60.f);
	ThirdPersonCameraNormal = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraNormal"));
	ThirdPersonCameraNormal->SetupAttachment(CameraBoomNormal, USpringArmComponent::SocketName);
	ThirdPersonCameraNormal->bUsePawnControlRotation = false;

	CameraBoomAiming = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomAiming"));
	CameraBoomAiming->SetupAttachment(RootComponent);
	CameraBoomAiming->TargetArmLength = 50;
	CameraBoomAiming->bUsePawnControlRotation = true;
	CameraBoomAiming->SocketOffset = FVector(0.f, 35.f, 60.f);
	ThirdPersonCameraAiming = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraAiming"));
	ThirdPersonCameraAiming->SetupAttachment(CameraBoomAiming, USpringArmComponent::SocketName);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	SetActiveNormalThirdPersonCamera();
}

void AMainPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMainPlayerCharacterBase::SetActiveNormalThirdPersonCamera()
{
	ThirdPersonCameraNormal->SetActive(true);
	ThirdPersonCameraAiming->SetActive(false);
	FirstPersonCamera->SetActive(false);
	bUseControllerRotationYaw = false;
}

void AMainPlayerCharacterBase::SetActiveAimingThirdPersonCamera()
{
	ThirdPersonCameraNormal->SetActive(false);
	ThirdPersonCameraAiming->SetActive(true);
	FirstPersonCamera->SetActive(false);
	bUseControllerRotationYaw = true;
}

void AMainPlayerCharacterBase::SetActiveFirstPersonCamera()
{
	ThirdPersonCameraNormal->SetActive(false);
	ThirdPersonCameraAiming->SetActive(false);
	FirstPersonCamera->SetActive(true);
	bUseControllerRotationYaw = true;
}

void AMainPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainPlayerCharacterBase::ToggleAim()
{
	if (!IsAiming)
	{
		SetActiveAimingThirdPersonCamera();
		IsAiming = true;
	}
	else
	{
		SetActiveNormalThirdPersonCamera();
		IsAiming = false;
	}
}

bool AMainPlayerCharacterBase::GetIsAiming()
{
	return IsAiming;
}

void AMainPlayerCharacterBase::SwitchToFirstPersonCamera()
{
	SetActiveFirstPersonCamera();
}

void AMainPlayerCharacterBase::SwitchToNormalCamera()
{
	if (!IsAiming)
	{
		SetActiveNormalThirdPersonCamera();
	}
	else
	{
		SetActiveAimingThirdPersonCamera();
	}
}
