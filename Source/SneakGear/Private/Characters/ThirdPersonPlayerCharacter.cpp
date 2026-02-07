#include "Characters/ThirdPersonPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/WeaponBase.h"

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
	FirstPersonCamera->SetupAttachment(GetMesh(), FirstPersonCameraSocket);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 8.f, 4.f));

	FirstPersonCamera->SetActive(false);
	ThirdPersonCamera->SetActive(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

UAbilitySystemComponent* AThirdPersonPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AThirdPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StartedWeaponClass)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartedWeaponClass);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToCharacter(GetMesh(), HolsterSocketName);
		}
	}
}

void AThirdPersonPlayerCharacter::InitGAS()
{
}

void AThirdPersonPlayerCharacter::BindHealthDeath()
{
}

void AThirdPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto bTPS = !bAimFirstPerson;

	auto TargetOffset = bIsAiming ? bTPS ? OverTheShouldOffset_Aim : FVector::ZeroVector : OverTheShouldOffset_Normal;

	if (CameraBoom)
	{
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaTime,
		                                            AimInterpolationSpeed);
	}

	auto targetFOV = !bIsAiming ? NormalFOV : bAimFirstPerson ? AimFOV_FirstPerson : AimFOV_ThirdPerson;

	if (ThirdPersonCamera && ThirdPersonCamera->IsActive())
	{
		ThirdPersonCamera->SetFieldOfView(FMath::FInterpTo(ThirdPersonCamera->FieldOfView, targetFOV, DeltaTime,
		                                                   AimInterpolationSpeed));
	}
	if (FirstPersonCamera && FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->SetFieldOfView(FMath::FInterpTo(FirstPersonCamera->FieldOfView, targetFOV, DeltaTime,
		                                                   AimInterpolationSpeed));
	}

	auto TargetTurn = bIsAiming ? AimTurnRate : NormalTurnRate;
	CurrentTurnScalar = FMath::FInterpTo(CurrentTurnScalar, TargetTurn, DeltaTime, 12.f);
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
	AddControllerYawInput(Axis.X * CurrentTurnScalar);
	AddControllerPitchInput(Axis.Y * CurrentTurnScalar);
}

void AThirdPersonPlayerCharacter::StartAim()
{
	bIsAiming = true;

	if (!bAimFirstPerson)
	{
		ThirdPersonCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

void AThirdPersonPlayerCharacter::StopAim()
{
	bIsAiming = false;

	bAimFirstPerson = false;
	ThirdPersonCamera->SetActive(true);
	FirstPersonCamera->SetActive(false);
}

void AThirdPersonPlayerCharacter::ToggleAimView()
{
	if (!bIsAiming)
	{
		return;
	}

	bAimFirstPerson = !bAimFirstPerson;

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

void AThirdPersonPlayerCharacter::ToggleEquip()
{
	if (!CurrentWeapon)
	{
		return;
	}

	auto TargetSocket = CurrentWeapon->GetAttachParentSocketName() == HolsterSocketName
		                    ? HandSocketName
		                    : HolsterSocketName;

	CurrentWeapon->AttachToCharacter(GetMesh(), TargetSocket);
}
