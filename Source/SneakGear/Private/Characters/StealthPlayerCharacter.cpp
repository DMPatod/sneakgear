#include "Characters/StealthPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AStealthPlayerCharacter::AStealthPlayerCharacter()
{
	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
}

void AStealthPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AStealthPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CoverComponent)
	{
		return;
	}

	if (CoverState == ECoverState::None)
	{
		auto Hit = CoverComponent->FindCover();
		if (Hit.bValid)
		{
			EnterCover(Hit);
		}
	}
	else
	{
		if (!CoverComponent->ValidateCover(CurrentCover.Normal, 90.f))
		{
			ExitCover();
			return;
		}

		if (CoverState == ECoverState::Approaching)
		{
			UpdateCoverApproach(DeltaTime);
		}
		else if (CoverState == ECoverState::Locked)
		{
			UpdateCoverRotation(DeltaTime);
		}
	}

	auto Velocity = GetVelocity();
	FVector Velocity2D(Velocity.X, Velocity.Y, 0.f);
	if (Velocity2D.Size() > 10.f)
	{
		const FVector MoveDirection = Velocity2D.GetSafeNormal();
		const float AwayDot = FVector::DotProduct(MoveDirection, CurrentCover.Normal);
		if (AwayDot > CoverExitBackDotThreshold)
		{
			ExitCover();
			return;
		}
	}
}

void AStealthPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* Eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Eic)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an enhanced input component"));
	}

	if (JumpAction)
	{
		Eic->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Eic->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	if (CrouchAction)
	{
		Eic->BindAction(CrouchAction, ETriggerEvent::Started, this, &AStealthPlayerCharacter::StartCrouch);
		Eic->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AStealthPlayerCharacter::StopCrouch);
	}
}

void AStealthPlayerCharacter::Move(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}
	
	if (CoverState == ECoverState::Locked)
	{
		auto Tangent = CurrentCover.Tangent;
		Tangent.Z = 0.f;
		Tangent = Tangent.GetSafeNormal();

		CoverMoveAxis = Axis.Y;
		AddMovementInput(Tangent, CoverMoveAxis);

		if (FMath::Abs(CoverMoveAxis) > 0.1f)
		{
			CoverFacingSign = CoverMoveAxis >= 0.f ? 1.f : -1.f;
		}

		if (Axis.X < -0.4f)
		{
			ExitCover();
		}

		return;
	}

	Super::Move(Value);
}

void AStealthPlayerCharacter::StartCrouch()
{
	Crouch();
}

void AStealthPlayerCharacter::StopCrouch()
{
	UnCrouch();
}

void AStealthPlayerCharacter::EnterCover(const FCoverHit& Hit)
{
	if (CoverState != ECoverState::None)
	{
		return;
	}

	CurrentCover = Hit;
	CoverState = ECoverState::Approaching;

	auto Move = GetCharacterMovement();
	Move->MaxWalkSpeed = CoverMaxSpeed;

	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	Move->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;

	Move->MaxWalkSpeed = 300.f;
}

void AStealthPlayerCharacter::LockCover()
{
	CurrentCover.Tangent *= -1.f;
	CoverState = ECoverState::Locked;

	auto Move = GetCharacterMovement();
	Move->MaxWalkSpeed = CoverMaxSpeed;

	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(CurrentCover.Normal);
	Move->SetPlaneConstraintOrigin(CurrentCover.SnapLocation);
	Move->bConstrainToPlane = true;

	Move->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
}

void AStealthPlayerCharacter::ExitCover()
{
	if (CoverState == ECoverState::None)
	{
		return;
	}

	CoverState = ECoverState::None;
	CoverApproachTime = 0.f;

	auto Move = GetCharacterMovement();

	Move->SetPlaneConstraintEnabled(false);
	Move->bConstrainToPlane = false;

	Move->bOrientRotationToMovement = true;
	Move->MaxWalkSpeed = 450;
}

void AStealthPlayerCharacter::UpdateCoverApproach(float DeltaTime)
{
	CoverApproachTime += DeltaTime;

	if (CoverApproachTime > CoverApproachTimeout)
	{
		ExitCover();
		return;
	}

	auto CurrentLocation = GetActorLocation();
	auto TargetLocation = FVector(CurrentCover.SnapLocation.X, CurrentCover.SnapLocation.Y, CurrentLocation.Z);

	auto NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, CoverAssistStrength);
	SetActorLocation(NewLocation, true);

	auto NewRotation = FMath::RInterpTo(GetActorRotation(), CurrentCover.SnapRotation, DeltaTime, CoverTurnSpeed);
	SetActorRotation(NewRotation);

	if (FVector::Dist2D(NewLocation, TargetLocation) <= CoverLockDistance)
	{
		LockCover();
	}
}

void AStealthPlayerCharacter::UpdateCoverRotation(float DeltaTime)
{
	auto FaceDir = CurrentCover.Tangent * CoverFacingSign;
	FaceDir.Z = 0.f;
	FaceDir.Normalize();

	auto TargetRotation = FaceDir.Rotation();
	auto NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, CoverTurnSpeed);
	SetActorRotation(NewRotation);
}

FVector AStealthPlayerCharacter::GetCoverTangentAlignedToCamera() const
{
	auto Tangent = CurrentCover.Tangent;
	Tangent.Z = 0.f;
	Tangent.Normalize();

	if (!Controller)
	{
		return Tangent;
	}

	FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	auto CamRight = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	CamRight.Z = 0.f;
	CamRight.Normalize();

	if (FVector::DotProduct(CamRight, Tangent) < 0.f)
	{
		Tangent *= -1.f;
	}
	return Tangent;
}
