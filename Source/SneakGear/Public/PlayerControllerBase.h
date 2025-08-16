#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

class ADefaultHubBase;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
struct FInputActionValue;
class APlayerCharacterBase;

UCLASS(Abstract)
class SNEAKGEAR_API APlayerControllerBase : public APlayerController
{
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionMove = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionLook = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionToggleSprint = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	UInputAction* ActionToggleCrouch = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputMappingContext> InputMappingContent = nullptr;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	void HandleLook(const FInputActionValue& InputActionValue);
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleToggleSprint();
	void HandleToggleCrouch();

private:
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent = nullptr;

	UPROPERTY()
	APlayerCharacterBase* PlayerCharacter = nullptr;

	GENERATED_BODY()
};
