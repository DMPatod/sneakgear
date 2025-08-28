#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerControllerBase.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AMainPlayerCharacterBase;

UCLASS(Abstract, Blueprintable)
class SNEAKGEAR_API AMainPlayerControllerBase : public APlayerController
{
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionMove = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionLook = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionToggleSprint = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionToggleCrouch = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Aiming")
	UInputAction* ActionAimingDown = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Aiming")
	UInputAction* ActionSwitchAimingCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Aiming")
	UInputAction* ActionSwitchAimingShoulder = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input")
	TObjectPtr<UInputMappingContext> InputMappingContent = nullptr;

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	void HandleLook(const FInputActionValue& InputActionValue);
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleToggleSprint();
	void HandleToggleCrouch();
	void HandleToggleAimingDown();
	void HandleSwitchAimingCamera();
	void HandleSwitchAimingShoulder();

private:
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent = nullptr;

	UPROPERTY()
	AMainPlayerCharacterBase* PlayerCharacter = nullptr;

	GENERATED_BODY()
};
