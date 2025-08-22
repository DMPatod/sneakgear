#pragma once

#include "CoreMinimal.h"
#include "EquipableToolBase.h"
#include "WeaponBase.generated.h"


UCLASS(BlueprintType, Blueprintable,Abstract)
class SNEAKGEAR_API AWeaponBase : public AEquipableToolBase
{
	public:
 
	virtual void Use() override;
 
	virtual void Tick(float DeltaTime) override;
 
	virtual void BindInputAction(const UInputAction* InputToBind) override;

	GENERATED_BODY()
	
};
