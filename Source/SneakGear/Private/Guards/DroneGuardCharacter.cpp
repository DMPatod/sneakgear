#include "Guards/DroneGuardCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ADroneGuardCharacter::ADroneGuardCharacter()
{
	ConfigureFlyingMovement();
}

void ADroneGuardCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureFlyingMovement();
	BaseHoverLocation = GetActorLocation();
}

void ADroneGuardCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bEnableHoverOffset || IsActorBeingDestroyed())
	{
		BaseHoverLocation = GetActorLocation();
		return;
	}

	if (const FVector Velocity = GetVelocity(); !Velocity.IsNearlyZero())
	{
		BaseHoverLocation = GetActorLocation();
		HoverTime = 0.f;
		return;
	}

	HoverTime += DeltaSeconds;

	const float HoverOffset = FMath::Sin(HoverTime * HoverFrequency * 2.f * PI) * HoverAmplitude;
	FVector TargetLocation = BaseHoverLocation;
	TargetLocation.Z += HoverOffset;

	SetActorLocation(TargetLocation);
}

void ADroneGuardCharacter::ConfigureFlyingMovement()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	Movement->GravityScale = 0.f;
	Movement->SetMovementMode(MOVE_Flying);
	Movement->MaxFlySpeed = FlyingMaxSpeed;
	Movement->MaxAcceleration = FlyingAcceleration;
	Movement->BrakingDecelerationFlying = FlyingBrakingDeceleration;
	Movement->bOrientRotationToMovement = true;
	Movement->bUseControllerDesiredRotation = false;
}
