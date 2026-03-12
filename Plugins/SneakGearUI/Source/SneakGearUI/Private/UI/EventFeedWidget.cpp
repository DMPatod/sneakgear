#include "UI/EventFeedWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UEventFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!EventText && WidgetTree)
	{
		EventText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EventFeedText"));
		WidgetTree->RootWidget = EventText;
	}

	if (UEventLogSubsystem* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		EventLog->OnEventAddedEvent().AddUObject(this, &UEventFeedWidget::HandleEventAdded);

		TArray<FGameEventEntry> ExistingEvents;
		EventLog->GetEventsAfter(LastSeenEventId, ExistingEvents);
		for (const FGameEventEntry& Event : ExistingEvents)
		{
			HandleEventAdded(Event);
		}
	}
}

void UEventFeedWidget::NativeDestruct()
{
	if (UEventLogSubsystem* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		EventLog->OnEventAddedEvent().RemoveAll(this);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ExpirationTimer);
	}

	Super::NativeDestruct();
}

void UEventFeedWidget::HandleEventAdded(const FGameEventEntry& Event)
{
	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	FEventFeedLine& Line = VisibleLines.AddDefaulted_GetRef();
	Line.EventId = Event.EventId;
	Line.Message = Event.Message;
	Line.ExpireTime = CurrentTime + EventLifetimeSeconds;
	LastSeenEventId = FMath::Max(LastSeenEventId, Event.EventId);

	if (MaxVisibleEvents > 0 && VisibleLines.Num() > MaxVisibleEvents)
	{
		VisibleLines.RemoveAt(0, VisibleLines.Num() - MaxVisibleEvents);
	}

	RefreshText();
	UpdateExpirationTimer();
}

void UEventFeedWidget::HandleExpirationTick()
{
	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	VisibleLines.RemoveAll([CurrentTime](const FEventFeedLine& Line)
	{
		return Line.ExpireTime <= CurrentTime;
	});

	RefreshText();
	UpdateExpirationTimer();
}

void UEventFeedWidget::UpdateExpirationTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (VisibleLines.IsEmpty())
	{
		World->GetTimerManager().ClearTimer(ExpirationTimer);
		return;
	}

	World->GetTimerManager().SetTimer(ExpirationTimer, this, &UEventFeedWidget::HandleExpirationTick, 0.25f, true);
}

void UEventFeedWidget::RefreshText()
{
	if (!EventText)
	{
		return;
	}

	if (VisibleLines.IsEmpty())
	{
		EventText->SetText(FText::GetEmpty());
		return;
	}

	FString Combined;
	for (int32 Index = 0; Index < VisibleLines.Num(); ++Index)
	{
		if (Index > 0)
		{
			Combined += TEXT("\n");
		}

		Combined += VisibleLines[Index].Message.ToString();
	}

	EventText->SetText(FText::FromString(Combined));
}
