#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_WeaponReloadFinished.generated.h"

UCLASS(meta=(DisplayName="Weapon Reload Finished"))
class SNEAKGEAR_API UAnimNotify_WeaponReloadFinished : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
