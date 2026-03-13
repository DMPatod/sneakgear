#include "Player/StealthPlayerAnimInstance.h"

#include "Player/Components/PlayerInventoryComponent.h"
#include "Player/SneakGearPlayerCharacter.h"

void UStealthPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	StealthCharacter = Cast<ASneakGearPlayerCharacter>(TryGetPawnOwner());
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
	if (const UPlayerInventoryComponent* ItemComponent = StealthCharacter->GetItemComponent())
	{
		ActiveWeaponSlot = ItemComponent->GetActiveWeaponSlot();
		bWeaponFiredRecently = ItemComponent->WasActiveWeaponFiredRecently();
	}
}
