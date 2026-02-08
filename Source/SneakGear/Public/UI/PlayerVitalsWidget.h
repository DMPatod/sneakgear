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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

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

	void UpdateFromPlayer();
};
