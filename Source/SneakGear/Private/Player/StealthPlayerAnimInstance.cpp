#include "Player/StealthPlayerAnimInstance.h"

#include "Player/Components/PlayerItemComponent.h"
#include "Player/StealthPlayerCharacter.h"

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
	if (const UPlayerItemComponent* ItemComponent = StealthCharacter->GetItemComponent())
	{
		ActiveWeaponSlot = ItemComponent->GetActiveWeaponSlot();
		bWeaponFiredRecently = ItemComponent->WasActiveWeaponFiredRecently();
	}
}
