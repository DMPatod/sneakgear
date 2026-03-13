#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDebugWidget.generated.h"

class UTextBlock;
class APawn;

UCLASS()
class SNEAKGEARUI_API UPlayerDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DebugText;

private:
	TWeakObjectPtr<APawn> CachedPlayer;
	FTimerHandle RefreshTimer;

	void UpdateDebugText();
	bool TryCachePlayer();
	const class IPlayerUIDataSource* GetPlayerUIDataSource() const;
};
