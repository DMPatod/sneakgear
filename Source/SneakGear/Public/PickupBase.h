#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacterBase.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UItemDefinition;

UCLASS(BlueprintType, Blueprintable, Abstract)
class SNEAKGEAR_API APickupBase : public AActor
{
public:
	APickupBase();

	void InitializePickup();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	FName ItemID;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	TSoftObjectPtr<UDataTable> DataTable;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Reference Item")
	TObjectPtr<UItemDefinition> ReferenceItem;

	UPROPERTY(VisibleDefaultsOnly, Category = "Pickup | Mesh")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Components")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	bool bRespawnable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	float RespawnTime = 5.f;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


private:
	FTimerHandle RespawnTimerHandle;

	TSoftObjectPtr<UDataTable> GetDataTable();

	GENERATED_BODY()
};
