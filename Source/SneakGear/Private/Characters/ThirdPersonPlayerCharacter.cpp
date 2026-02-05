#include "Characters/ThirdPersonPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
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

	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AThirdPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AThirdPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	auto Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	auto Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.X);
	AddMovementInput(Right, Axis.Y);
}

void AThirdPersonPlayerCharacter::Look(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

