#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipableToolBase.generated.h"

class APlayerCharacterBase;
class UInputAction;
class UInputMappingContext;

UCLASS(Abstract)
class SNEAKGEAR_API AEquipableToolBase : public AActor
{

public:
	AEquipableToolBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<APlayerCharacterBase> OwningCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> FirstPersonToolAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> ToolMappingContext;

	UFUNCTION()
	virtual void BindInputAction(const UInputAction* ActionToBind);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> ThirdPersonToolAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ToolMeshComponent;
	
	UFUNCTION()
	virtual void Use();



	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UseCooldown;

private:
	GENERATED_BODY()

};
