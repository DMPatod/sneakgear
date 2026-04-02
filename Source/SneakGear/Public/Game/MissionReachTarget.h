#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionReachTarget.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UInfoPromptNativeWidget;

UCLASS()
class SNEAKGEAR_API AMissionReachTarget : public AActor
{
	GENERATED_BODY()

public:
	AMissionReachTarget();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintCallable, Category="Mission")
	void StartMissionFromTarget();

	UFUNCTION(BlueprintCallable, Category="Mission")
	void CompleteMissionFromTarget(AActor* OverlappingActor = nullptr);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	TObjectPtr<UStaticMeshComponent> ObjectiveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	TObjectPtr<USphereComponent> ReachTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	TObjectPtr<UWidgetComponent> ObjectivePromptComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	FText MissionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	FText ObjectiveText = FText::FromString(TEXT("Reach the extraction point"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	FText CompletionText = FText::FromString(TEXT("Mission complete"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	bool bStartMissionOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	bool bCompleteMissionOnOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	bool bRequireMissionToBeActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission")
	bool bDestroyAfterMissionComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission|UI")
	bool bShowObjectivePrompt = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission|UI")
	FText ObjectivePromptText = FText::FromString(TEXT("Reach objective"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mission|UI")
	FVector ObjectivePromptOffset = FVector(0.f, 0.f, 110.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mission|UI")
	TSubclassOf<UInfoPromptNativeWidget> ObjectivePromptWidgetClass;

private:
	void UpdatePrompt();
	bool IsPlayerActor(const AActor* OtherActor) const;

	UFUNCTION()
	void HandleReachTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
