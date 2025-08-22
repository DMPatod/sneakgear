#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DefaultHudBase.generated.h"

class APlayerCharacterBase;
class UDefaultHudLayoutBase;
class UDebugHudLayoutBase;

UENUM(BlueprintType)
enum class EHudViewMode : uint8
{
	Clean UMETA(ToolTip = "Nothing to show on UI."),
	Default UMETA(ToolTip = "Default HUD."),
	Debug UMETA(ToolTip = "Show every information.")
};

UCLASS(ABSTRACT)
class SNEAKGEAR_API ADefaultHudBase : public AHUD
{
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDefaultHudLayoutBase> DefaultLayoutClass = nullptr;

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<UDebugHudLayoutBase> DebugLayoutClass = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetCurrentViewMode(EHudViewMode NewViewMode);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere)
	EHudViewMode CurrentViewMode = EHudViewMode::Default;

	UPROPERTY()
	TObjectPtr<UWorld> World = nullptr;

	UPROPERTY()
	TObjectPtr<UDefaultHudLayoutBase>  DefaultLayoutWidget = nullptr;

	// UPROPERTY()
	// TObjectPtr<UDebugHudLayoutBase> DebugLayoutWidget = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> PlayerCharacter;

	void UpdateWidgets();
	void ClearAllHandlers();
	
	GENERATED_BODY()
};
