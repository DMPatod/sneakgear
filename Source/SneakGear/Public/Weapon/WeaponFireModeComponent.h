#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFireModeComponent.generated.h"


USTRUCT()
struct FWeaponFireContext
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APawn> InstigatorPawn = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> WeaponActor = nullptr;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY()
	FVector CameraLocation = FVector::ZeroVector;

	UPROPERTY()
	FVector CameraDirection = FVector::ForwardVector;

	UPROPERTY()
	FName MuzzleSocket = "muzzle_socket";
};

UCLASS(Abstract, Blueprintable, ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UWeaponFireModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void FireOnce(const FWeaponFireContext& Context)
	PURE_VIRTUAL(UWeaponFireModeComponent::FireOnce,);

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float Range = 8000.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float NoiseRadius = 1800.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDrawDebug = true;
};
