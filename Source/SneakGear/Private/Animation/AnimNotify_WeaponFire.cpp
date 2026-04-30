#include "Animation/AnimNotify_WeaponFire.h"

#include "Guards/GuardCharacterAnimInstance.h"
#include "Player/SneakGearPlayerAnimInstance.h"
#include "UI/EventLogSubsystem.h"

void UAnimNotify_WeaponFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UAnimInstance* AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	if (USneakGearPlayerAnimInstance* PlayerAnimInstance = Cast<USneakGearPlayerAnimInstance>(AnimInstance))
	{
		PlayerAnimInstance->NotifyWeaponFireAnimation();
		return;
	}

	if (UGuardCharacterAnimInstance* GuardAnimInstance = Cast<UGuardCharacterAnimInstance>(AnimInstance))
	{
		GuardAnimInstance->NotifyWeaponFireAnimation();
	}
}
