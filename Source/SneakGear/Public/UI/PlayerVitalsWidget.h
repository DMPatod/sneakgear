#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerVitalsWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AThirdPersonPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UPlayerVitalsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> StaminaText;

private:
	TWeakObjectPtr<AThirdPersonPlayerCharacter> CachedPlayer;

	void HandleHealthChanged(const struct FOnAttributeChangeData& Data);
	void HandleStaminaChanged(const struct FOnAttributeChangeData& Data);
	void UpdateFromPlayer();
};
