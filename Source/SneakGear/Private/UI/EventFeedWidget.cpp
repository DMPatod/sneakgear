#include "UI/EventFeedWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

void UEventFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// SetCanTick(true);

	if (EventText || !WidgetTree)
	{
		return;
	}

	EventText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EventFeedText"));
	WidgetTree->RootWidget = EventText;
}

void UEventFeedWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	auto* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr;
	if (!EventLog)
	{
		return;
	}

	TArray<FGameEventEntry> NewEvents;
	EventLog->GetEventsAfter(LastSeenEventId, NewEvents);

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	for (const FGameEventEntry& Event : NewEvents)
	{
		FEventFeedLine& Line = VisibleLines.AddDefaulted_GetRef();
		Line.EventId = Event.EventId;
		Line.Message = Event.Message;
		Line.ExpireTime = CurrentTime + EventLifetimeSeconds;
		LastSeenEventId = FMath::Max(LastSeenEventId, Event.EventId);
	}

	VisibleLines.RemoveAll([CurrentTime](const FEventFeedLine& Line)
	{
		return Line.ExpireTime <= CurrentTime;
	});

	if (MaxVisibleEvents > 0 && VisibleLines.Num() > MaxVisibleEvents)
	{
		VisibleLines.RemoveAt(0, VisibleLines.Num() - MaxVisibleEvents);
	}

	RefreshText();
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
