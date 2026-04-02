#include "Game/MissionStartTarget.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/SandboxGameState.h"
#include "GameFramework/Pawn.h"
#include "Misc/DataValidation.h"
#include "UI/InfoPromptNativeWidget.h"

AMissionStartTarget::AMissionStartTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	ObjectiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	SetRootComponent(ObjectiveMesh);
	ObjectiveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ReachTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ReachTrigger"));
	ReachTrigger->SetupAttachment(ObjectiveMesh);
	ReachTrigger->SetSphereRadius(120.f);
	ReachTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ReachTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);

	ObjectivePromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ObjectivePromptComponent"));
	ObjectivePromptComponent->SetupAttachment(ObjectiveMesh);
	ObjectivePromptComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ObjectivePromptComponent->SetDrawAtDesiredSize(true);
	ObjectivePromptComponent->SetRelativeLocation(ObjectivePromptOffset);
	ObjectivePromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ObjectivePromptWidgetClass = UInfoPromptNativeWidget::StaticClass();
}

void AMissionStartTarget::BeginPlay()
{
	Super::BeginPlay();

	if (ObjectivePromptComponent)
	{
		ObjectivePromptComponent->SetRelativeLocation(ObjectivePromptOffset);
		ObjectivePromptComponent->SetWidgetClass(ObjectivePromptWidgetClass
			? ObjectivePromptWidgetClass.Get()
			: UInfoPromptNativeWidget::StaticClass());
		ObjectivePromptComponent->InitWidget();
		UpdatePrompt();
	}

	if (ReachTrigger)
	{
		ReachTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMissionStartTarget::HandleReachTriggerBeginOverlap);
	}
}

void AMissionStartTarget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ObjectivePromptComponent)
	{
		ObjectivePromptComponent->SetRelativeLocation(ObjectivePromptOffset);
	}
}

EDataValidationResult AMissionStartTarget::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ObjectiveMesh)
	{
		Context.AddError(FText::FromString(TEXT("ObjectiveMesh component is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!ReachTrigger)
	{
		Context.AddError(FText::FromString(TEXT("ReachTrigger component is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!ObjectivePromptComponent)
	{
		Context.AddError(FText::FromString(TEXT("ObjectivePromptComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (MissionName.IsEmpty())
	{
		Context.AddWarning(FText::FromString(TEXT("MissionName is empty; the mission can still start but will log without a title.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}

void AMissionStartTarget::StartMissionFromTarget(AActor* OverlappingActor)
{
	if (OverlappingActor && !IsPlayerActor(OverlappingActor))
	{
		return;
	}

	ASandboxGameState* SandboxGameState = GetWorld() ? GetWorld()->GetGameState<ASandboxGameState>() : nullptr;
	if (!SandboxGameState)
	{
		return;
	}

	if (!bRestartMissionIfAlreadyActive && SandboxGameState->IsMissionActive())
	{
		return;
	}

	SandboxGameState->StartMission(MissionName, ObjectiveText);
	UpdatePrompt();

	if (bDestroyAfterMissionStart)
	{
		Destroy();
	}
}

void AMissionStartTarget::UpdatePrompt()
{
	if (!ObjectivePromptComponent)
	{
		return;
	}

	ObjectivePromptComponent->SetVisibility(bShowObjectivePrompt);

	UInfoPromptNativeWidget* PromptWidget = Cast<UInfoPromptNativeWidget>(ObjectivePromptComponent->GetWidget());
	if (!PromptWidget)
	{
		return;
	}

	const FText PromptText = !ObjectivePromptText.IsEmpty() ? ObjectivePromptText : ObjectiveText;
	PromptWidget->SetPromptText(PromptText);
	PromptWidget->SetPromptVisible(bShowObjectivePrompt && !PromptText.IsEmpty());
}

bool AMissionStartTarget::IsPlayerActor(const AActor* OtherActor) const
{
	const APawn* Pawn = Cast<APawn>(OtherActor);
	return Pawn && Pawn->IsPlayerControlled();
}

void AMissionStartTarget::HandleReachTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;

	if (!bStartMissionOnOverlap || !OtherActor || !IsPlayerActor(OtherActor))
	{
		return;
	}

	StartMissionFromTarget(OtherActor);
}
