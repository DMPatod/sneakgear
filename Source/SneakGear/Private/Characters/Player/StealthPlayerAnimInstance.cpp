#include "Characters/Player/StealthPlayerAnimInstance.h"

#include "Characters/Player/StealthPlayerCharacter.h"

void UStealthPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	StealthCharacter = Cast<AStealthPlayerCharacter>(TryGetPawnOwner());
}

void UStealthPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!StealthCharacter)
	{
		NativeInitializeAnimation();
		if (!StealthCharacter)
		{
			return;
		}
	}

	bInCover = StealthCharacter->IsInCover();
	CoverMove = StealthCharacter->GetCoverMoveAxis();
}
