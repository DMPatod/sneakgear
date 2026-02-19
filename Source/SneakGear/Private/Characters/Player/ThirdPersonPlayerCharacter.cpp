#include "Characters/Player/ThirdPersonPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PlayerAimComponent.h"
#include "Components/PlayerWeaponComponent.h"
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
}

void AThirdPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseWalkSpeed = GetCharacterMovement() ? GetCharacterMovement()->MaxWalkSpeed : BaseWalkSpeed;
	if (bIsSprinting && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SprintSpeedMultiplier;
	}

	if (AimComponent)
	{
		AimComponent->Initialize(CameraBoom, ThirdPersonCamera, FirstPersonCamera);
	}

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}
}

void AThirdPersonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* Eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Eic)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an enhanced input component"));
		return;
	}

	if (MoveAction)
	{
		Eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonPlayerCharacter::Move);
	}
	else
	{
		ensureMsgf(false, TEXT("MoveAction is not set on %s"), *GetName());
	}

	if (LookAction)
	{
		Eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonPlayerCharacter::Look);
	}
	else
	{
		ensureMsgf(false, TEXT("LookAction is not set on %s"), *GetName());
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

	if (SprintToggleAction)
	{
		Eic->BindAction(SprintToggleAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::ToggleSprint);
	}

	if (JumpAction)
	{
		Eic->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Eic->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	if (ReloadAction)
	{
		Eic->BindAction(ReloadAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::ReloadWeapon);
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

void AThirdPersonPlayerCharacter::ToggleSprint()
{
	auto* MoveComponent = GetCharacterMovement();
	if (!MoveComponent)
	{
		return;
	}

	bIsSprinting = !bIsSprinting;
	MoveComponent->MaxWalkSpeed = bIsSprinting ? BaseWalkSpeed * SprintSpeedMultiplier : BaseWalkSpeed;
}

void AThirdPersonPlayerCharacter::ReloadWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
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
