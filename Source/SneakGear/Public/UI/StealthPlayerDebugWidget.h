#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StealthPlayerDebugWidget.generated.h"

class UTextBlock;
class AStealthPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UStealthPlayerDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DebugText;

private:
	TWeakObjectPtr<AStealthPlayerCharacter> CachedPlayer;
	FTimerHandle RefreshTimer;

	void UpdateDebugText();
	bool TryCachePlayer();
};
