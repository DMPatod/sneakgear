#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StealthPlayerController.generated.h"

class URadarWidget;
class UMainHUDWidget;
class UInputMappingContext;

UCLASS()
class SNEAKGEAR_API AStealthPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStealthPlayerController();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainHUDWidget> MainHUDWidget;

	virtual void Tick(float DeltaSeconds) override;
};
