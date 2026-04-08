#include "Guards/BehaviorTree/BTService_UpdateGuardFireState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Guards/GuardCharacter.h"

UBTService_UpdateGuardFireState::UBTService_UpdateGuardFireState()
{
	NodeName = TEXT("Update Guard Fire State");
	Interval = 0.1f;
	RandomDeviation = 0.0f;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateGuardFireState, TargetActorKey),
	                               AActor::StaticClass());
	HasLineOfSightKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateGuardFireState, HasLineOfSightKey));
}

void UBTService_UpdateGuardFireState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UE_LOG(LogTemp, Warning, TEXT("HELPPP!2"));

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AGuardCharacter* GuardCharacter = OwnerComp.GetAIOwner()
		                                  ? Cast<AGuardCharacter>(OwnerComp.GetAIOwner()->GetPawn())
		                                  : nullptr;
	if (!Blackboard || !GuardCharacter)
	{
		return;
	}

	const AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	const bool bHasLineOfSight = Blackboard->GetValueAsBool(HasLineOfSightKey.SelectedKeyName);
	GuardCharacter->SetCombatFiringEnabled(TargetActor != nullptr && bHasLineOfSight);
}

void UBTService_UpdateGuardFireState::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	
	UE_LOG(LogTemp, Warning, TEXT("HELPPP!"));

	if (AGuardCharacter* GuardCharacter = OwnerComp.GetAIOwner()
		                                      ? Cast<AGuardCharacter>(OwnerComp.GetAIOwner()->GetPawn())
		                                      : nullptr)
	{
		GuardCharacter->SetCombatFiringEnabled(false);
	}
}
