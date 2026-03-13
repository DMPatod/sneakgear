#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerVitalsWidget.generated.h"

class UProgressBar;
class UTextBlock;
class APawn;

UCLASS()
class SNEAKGEARUI_API UPlayerVitalsWidget : public UUserWidget
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
	TWeakObjectPtr<APawn> CachedPlayer;

	void HandleVitalsChanged();
	const class IPlayerUIDataSource* GetPlayerUIDataSource() const;
	void UpdateFromPlayer();
};
