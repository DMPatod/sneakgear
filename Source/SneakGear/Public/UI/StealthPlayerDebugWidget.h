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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DebugText;

private:
	TWeakObjectPtr<AStealthPlayerCharacter> CachedPlayer;

	void UpdateDebugText();
	bool TryCachePlayer();
};
