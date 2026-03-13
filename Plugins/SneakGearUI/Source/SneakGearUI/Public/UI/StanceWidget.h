#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/PlayerMovementTypes.h"
#include "StanceWidget.generated.h"

class APawn;
class UTextBlock;
class UImage;

UCLASS()
class SNEAKGEARUI_API UStanceWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> StanceImage;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> StanceText;

	UPROPERTY(EditDefaultsOnly, Category="Stances")
	TMap<EStance, TObjectPtr<UTexture2D>> StanceTextures;

private:
	TWeakObjectPtr<APawn> CachedPlayer;
	EStance CurrentStance = EStance::Standing;
	bool bHasInitializedUI = false;

	bool TryCachePlayer();
	class IPlayerUIDataSource* GetPlayerUIDataSource() const;
	void HandleStanceChanged(EStance NewStance);
	void UpdateFromPlayer();
};
