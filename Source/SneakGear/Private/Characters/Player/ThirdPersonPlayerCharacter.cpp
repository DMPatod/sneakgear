#include "Characters/Player/ThirdPersonPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PlayerAimComponent.h"
#include "Components/PlayerLocomotionComponent.h"
#include "Components/PlayerWeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"

AThirdPersonPlayerCharacter::AThirdPersonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SetupViewComponents();
	SetupGameplayComponents();

	if (LocomotionComponent)
	{
		LocomotionComponent->SetupMovementDefaults();
	}
}

bool AThirdPersonPlayerCharacter::IsAiming() const
{
	return AimComponent ? AimComponent->IsAiming() : false;
}

AWeaponBase* AThirdPersonPlayerCharacter::GetCurrentWeapon() const
{
	return WeaponComponent ? WeaponComponent->GetCurrentWeapon() : nullptr;
}

float AThirdPersonPlayerCharacter::GetAmmo() const
{
	return Ammo;
}

float AThirdPersonPlayerCharacter::ConsumeAmmo(float Amount)
{
	const float UsedArmor = FMath::Clamp(Amount, 0.f, Ammo);
	Ammo -= UsedArmor;
	return UsedArmor;
}

float AThirdPersonPlayerCharacter::GetMaxSpeed() const
{
	return LocomotionComponent ? LocomotionComponent->GetMaxSpeed() : 0.f;
}

void AThirdPersonPlayerCharacter::SetupViewComponents()
{
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
}

void AThirdPersonPlayerCharacter::SetupGameplayComponents()
{
	WeaponComponent = CreateDefaultSubobject<UPlayerWeaponComponent>(TEXT("WeaponComponent"));
	AimComponent = CreateDefaultSubobject<UPlayerAimComponent>(TEXT("AimComponent"));
	LocomotionComponent = CreateDefaultSubobject<UPlayerLocomotionComponent>(TEXT("LocomotionComponent"));
}

void AThirdPersonPlayerCharacter::InitializeGameplayState()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->Initialize(CameraBoom);
	}

	if (AimComponent)
	{
		AimComponent->Initialize(CameraBoom, ThirdPersonCamera, FirstPersonCamera);
	}

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}

	if (LocomotionComponent)
	{
		LocomotionComponent->UpdateRotationMode(IsAiming());
	}
}

void AThirdPersonPlayerCharacter::SetStance(EStance NewStance)
{
	if (LocomotionComponent)
	{
		LocomotionComponent->SetStance(NewStance);
	}
}

void AThirdPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeGameplayState();
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

	BindInputActions(Eic);
}

void AThirdPersonPlayerCharacter::BindInputActions(UEnhancedInputComponent* Eic)
{
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

	if (StanceAction)
	{
		Eic->BindAction(StanceAction, ETriggerEvent::Started, this, &AThirdPersonPlayerCharacter::OnStancePressed);
		Eic->BindAction(StanceAction, ETriggerEvent::Completed, this, &AThirdPersonPlayerCharacter::OnStanceReleased);
	}
}

void AThirdPersonPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (LocomotionComponent)
	{
		LocomotionComponent->TickLocomotion(DeltaSeconds);
	}
}

void AThirdPersonPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!LocomotionComponent)
	{
		return;
	}

	LocomotionComponent->Move(Value);
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

		ApplyAimRotationMode(true);
	}
}

void AThirdPersonPlayerCharacter::StopAim()
{
	if (AimComponent)
	{
		AimComponent->StopAim();

		ApplyAimRotationMode(false);
	}
}

void AThirdPersonPlayerCharacter::ApplyAimRotationMode(bool bEnableAimRotation)
{
	bUseControllerRotationYaw = bEnableAimRotation;

	if (LocomotionComponent)
	{
		LocomotionComponent->UpdateRotationMode(IsAiming());
	}

	if (bEnableAimRotation && Controller)
	{
		const FRotator ControlYaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		SetActorRotation(ControlYaw);
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
	if (LocomotionComponent)
	{
		LocomotionComponent->ToggleSprint();
	}
}

void AThirdPersonPlayerCharacter::ReloadWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

void AThirdPersonPlayerCharacter::OnStancePressed()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->OnStancePressed();
	}
}

void AThirdPersonPlayerCharacter::OnStanceReleased()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->OnStanceReleased();
	}
}

void AThirdPersonPlayerCharacter::StartFire()
{
	if (WeaponComponent && IsAiming())
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
