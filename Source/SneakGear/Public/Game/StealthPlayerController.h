#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StealthPlayerController.generated.h"

class URadarWidget;
class UInputMappingContext;

UCLASS()
class SNEAKGEAR_API AStealthPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<URadarWidget> RadarWidgetClass;

	UPROPERTY()
	TObjectPtr<URadarWidget> RadarWidget;

	virtual void Tick(float DeltaSeconds) override;
};
