#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/EventLogSubsystem.h"
#include "EventFeedWidget.generated.h"

class UTextBlock;

USTRUCT()
struct FEventFeedLine
{
	GENERATED_BODY()

	int32 EventId = 0;
	float ExpireTime = 0.f;
	FText Message;
};

UCLASS()
class SNEAKGEARUI_API UEventFeedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> EventText;

	UPROPERTY(EditAnywhere, Category="Event Feed")
	int32 MaxVisibleEvents = 6;

	UPROPERTY(EditAnywhere, Category="Event Feed")
	float EventLifetimeSeconds = 7.f;

private:
	int32 LastSeenEventId = 0;
	TArray<FEventFeedLine> VisibleLines;
	FTimerHandle ExpirationTimer;

	void HandleEventAdded(const FGameEventEntry& Event);
	void HandleExpirationTick();
	void UpdateExpirationTimer();
	void RefreshText();
};
