#pragma once

#include "CoreMinimal.h"
#include "Cover/CoverComponent.h"
#include "GameFramework/Character.h"
#include "StealthPlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SNEAKGEAR_API AStealthPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AStealthPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartCrouch();
	void StopCrouch();

	bool bInCover = false;
	FCoverHit CurrentCover;
};
