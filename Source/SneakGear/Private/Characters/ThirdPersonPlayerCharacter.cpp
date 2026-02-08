#include "Characters/ThirdPersonPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/PlayerAimComponent.h"
#include "Components/PlayerWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/PlayerTuningData.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AThirdPersonPlayerCharacter::AThirdPersonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	auto MoveComponent = GetCharacterMovement();
	MoveComponent->bOrientRotationToMovement = true;
	MoveComponent->RotationRate = FRotator(0.f, 540.f, 0.f);
	MoveComponent->JumpZVelocity = 600.f;
	MoveComponent->AirControl = 0.35f;
	MoveComponent->MaxWalkSpeed = 450.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.f;

	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("first_person_camera_attachment"));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 8.f, 4.f));

	FirstPersonCamera->SetActive(false);
	ThirdPersonCamera->SetActive(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	WeaponComponent = CreateDefaultSubobject<UPlayerWeaponComponent>(TEXT("WeaponComponent"));
	AimComponent = CreateDefaultSubobject<UPlayerAimComponent>(TEXT("AimComponent"));
	DefaultTuningData = CreateDefaultSubobject<UPlayerTuningData>(TEXT("DefaultTuningData"));
}

UAbilitySystemComponent* AThirdPersonPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AThirdPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	const UPlayerTuningData* ActiveTuning = TuningData ? TuningData : DefaultTuningData;
	if (ActiveTuning)
	{
		const FPlayerMovementTuning& Movement = ActiveTuning->Movement;
		GetCapsuleComponent()->InitCapsuleSize(Movement.CapsuleRadius, Movement.CapsuleHalfHeight);

		auto* MoveComponent = GetCharacterMovement();
		MoveComponent->MaxWalkSpeed = Movement.MaxWalkSpeed;
		MoveComponent->JumpZVelocity = Movement.JumpZVelocity;
		MoveComponent->AirControl = Movement.AirControl;
		MoveComponent->RotationRate = FRotator(0.f, Movement.RotationRateYaw, 0.f);

		const FPlayerCameraTuning& Camera = ActiveTuning->Camera;
		if (CameraBoom)
		{
			CameraBoom->TargetArmLength = Camera.CameraBoomLength;
			CameraBoom->CameraLagSpeed = Camera.CameraLagSpeed;
			CameraBoom->SetRelativeLocation(Camera.CameraBoomOffset);
		}

		if (FirstPersonCamera)
		{
			FirstPersonCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
			                                     Camera.FirstPersonCameraSocket);
			FirstPersonCamera->SetRelativeLocation(Camera.FirstPersonCameraOffset);
		}
	}

	if (AimComponent)
	{
		AimComponent->Initialize(CameraBoom, ThirdPersonCamera, FirstPersonCamera, ActiveTuning);
	}
}

void AThirdPersonPlayerCharacter::InitGAS()
{
}

void AThirdPersonPlayerCharacter::BindHealthDeath()
{
}

void AThirdPersonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* Eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Eic)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an enhanced input component"));
	}

	if (MoveAction)
	{
		Eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonPlayerCharacter::Move);
	}

	if (LookAction)
	{
		Eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonPlayerCharacter::Look);
	}

	if (FireAction)
	{
		Eic->BindAction(FireAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::StartFire);
		Eic->BindAction(FireAction, ETriggerEvent::Completed, this, &AThirdPersonPlayerCharacter::StopFire);
	}

	if (AimAction)
	{
		Eic->BindAction(AimAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::StartAim);
		Eic->BindAction(AimAction, ETriggerEvent::Completed, this, &AThirdPersonPlayerCharacter::StopAim);
	}

	if (AimViewToggleAction)
	{
		Eic->BindAction(AimViewToggleAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::ToggleAimView);
	}

	if (EquipAction)
	{
		Eic->BindAction(EquipAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::ToggleEquip);
	}
}

void AThirdPersonPlayerCharacter::Move(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	auto ControlRot = Controller->GetControlRotation();
	FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	auto Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	auto Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);

	AddMovementInput(Forward, Axis.X);
	AddMovementInput(Right, Axis.Y);
}

void AThirdPersonPlayerCharacter::Look(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	const float TurnScalar = AimComponent ? AimComponent->GetCurrentTurnScalar() : 1.f;
	AddControllerYawInput(Axis.X * TurnScalar);
	AddControllerPitchInput(Axis.Y * TurnScalar);
}

void AThirdPersonPlayerCharacter::StartAim()
{
	if (AimComponent)
	{
		AimComponent->StartAim();
	}
}

void AThirdPersonPlayerCharacter::StopAim()
{
	if (AimComponent)
	{
		AimComponent->StopAim();
	}
}

void AThirdPersonPlayerCharacter::ToggleAimView()
{
	if (AimComponent)
	{
		AimComponent->ToggleAimView();
	}
}

void AThirdPersonPlayerCharacter::ToggleEquip()
{
	if (!WeaponComponent)
	{
		return;
	}

	WeaponComponent->ToggleEquip();
}

void AThirdPersonPlayerCharacter::StartFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->StartFire();
	}
}

void AThirdPersonPlayerCharacter::StopFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->StopFire();
	}
}
