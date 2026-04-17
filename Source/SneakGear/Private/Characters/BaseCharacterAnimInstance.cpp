#include "Characters/BaseCharacterAnimInstance.h"

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

static float CalculateDirectionDegrees(const FVector& Velocity2D, const FTransform& ActorTransform)
{
	const FVector LocalVelocity = ActorTransform.InverseTransformVectorNoScale(Velocity2D);
	return FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
}

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPawn = TryGetPawnOwner();
	BaseCharacter = Cast<ABaseCharacter>(OwnerPawn);
	MovementComponent = BaseCharacter ? BaseCharacter->GetCharacterMovement() : nullptr;
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!BaseCharacter || !MovementComponent)
	{
		NativeInitializeAnimation();
		if (!BaseCharacter || !MovementComponent)
		{
			return;
		}
	}

	const FVector Velocity = BaseCharacter->GetVelocity();
	const FVector Velocity2D(Velocity.X, Velocity.Y, 0.f);

	Speed = Velocity2D.Size();
	Speed2D = Speed;
	bIsInAir = MovementComponent->IsFalling();

	if (Velocity2D.SizeSquared() > 1.f)
	{
		Direction = CalculateDirectionDegrees(Velocity2D, BaseCharacter->GetActorTransform());
	}
	else
	{
		Direction = 0.f;
	}
}
