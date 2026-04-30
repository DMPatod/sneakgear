#include "Animation/AnimNotify_WeaponReloadFinished.h"

#include "Guards/GuardCharacterAnimInstance.h"
#include "Player/SneakGearPlayerAnimInstance.h"

void UAnimNotify_WeaponReloadFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UAnimInstance* AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	if (USneakGearPlayerAnimInstance* PlayerAnimInstance = Cast<USneakGearPlayerAnimInstance>(AnimInstance))
	{
		PlayerAnimInstance->NotifyWeaponReloadAnimationFinished();
		return;
	}

	if (UGuardCharacterAnimInstance* GuardAnimInstance = Cast<UGuardCharacterAnimInstance>(AnimInstance))
	{
		GuardAnimInstance->NotifyWeaponReloadAnimationFinished();
	}
}
