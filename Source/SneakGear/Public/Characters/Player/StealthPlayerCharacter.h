#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Cover/CoverStateComponent.h"
#include "StealthPlayerCharacter.generated.h"

class UCoverComponent;
class UCoverStateComponent;
struct FInputActionValue;

UCLASS()
class SNEAKGEAR_API AStealthPlayerCharacter : public AThirdPersonPlayerCharacter
{
	GENERATED_BODY()

public:
	AStealthPlayerCharacter();

	bool IsInCover() const
	{
		return CoverStateComponent ? CoverStateComponent->IsInCover() : false;
	}

	float GetCoverMoveAxis() const
	{
		return CoverStateComponent ? CoverStateComponent->GetCoverMoveAxis() : 0.f;
	}

protected:
	virtual void Move(const FInputActionValue& Value) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverComponent> CoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	TObjectPtr<UCoverStateComponent> CoverStateComponent;
};
