#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/PlayerInventoryTypes.h"
#include "WorldItemPickup.generated.h"

class AWeaponBase;
class UPlayerItemDefinition;
class UPlayerItemPickupComponent;
class UStaticMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UPickupPromptWidget;

UCLASS()
class SNEAKGEAR_API AWorldItemPickup : public AActor
{
	GENERATED_BODY()

public:
	AWorldItemPickup();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintPure, Category="Pickup")
	FPlayerInventoryItem GetPickupItem() const;

	UFUNCTION(BlueprintPure, Category="Pickup")
	TSubclassOf<AWeaponBase> GetPickupWeaponClass() const;

	UFUNCTION(BlueprintPure, Category="Pickup")
	UPlayerItemDefinition* GetItemDefinition() const;

	UFUNCTION(BlueprintCallable, Category="Pickup")
	void ConsumePickup();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USphereComponent> PickupTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UPlayerItemPickupComponent> PickupItemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UWidgetComponent> PickupPromptComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	TSubclassOf<UPickupPromptWidget> PickupPromptWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	FVector PickupPromptOffset = FVector(0.f, 0.f, 90.f);

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> PendingPromptActor;

	FTimerHandle PromptRetryTimer;

	UFUNCTION()
	void HandlePickupTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                     const FHitResult& SweepResult);

	UFUNCTION()
	void HandlePickupTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RetryShowPickupPrompt();

	bool UpdatePickupPrompt();
	bool IsLocallyControlledPlayerActor(const AActor* OtherActor) const;
};
