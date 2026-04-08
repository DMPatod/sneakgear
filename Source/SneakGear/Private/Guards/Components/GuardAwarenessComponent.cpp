#include "Guards/Components/GuardAwarenessComponent.h"

#include "Guards/Data/GuardArchetypeData.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UI/EventLogSubsystem.h"

UGuardAwarenessComponent::UGuardAwarenessComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGuardAwarenessComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetActor)
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	AwarenessState = ResolveAwarenessState(Awareness);
}

void UGuardAwarenessComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePerception(DeltaTime);
}

void UGuardAwarenessComponent::UpdatePerception(float DeltaTime)
{
	if (!TargetActor)
	{
		bHasLineOfSight = false;
		return;
	}

	float VisionScore = 0.f;
	const bool bCanSee = CanSeeTarget(TargetActor, VisionScore);
	bHasLineOfSight = bCanSee;

	if (bCanSee)
	{
		Awareness = FMath::Clamp(Awareness + VisionScore * AwarenessGainPerSecond * DeltaTime, 0.f, 1.f);
	}
	else
	{
		Awareness = FMath::Clamp(Awareness - AwarenessDecayPerSecond * DeltaTime, 0.f, 1.f);
	}

	UpdateAwarenessStateAndEmitEvent();
	DrawDebugVision(TargetActor, bCanSee);
}

void UGuardAwarenessComponent::InitializeFromArchetype(const UGuardArchetypeData* ArchetypeData)
{
	if (!ArchetypeData)
	{
		return;
	}

	Awareness = FMath::Clamp(ArchetypeData->InitialAwareness, 0.f, 1.f);
	VisionRange = ArchetypeData->VisionRange;
	VisionHalfAngleDeg = ArchetypeData->VisionHalfAngleDeg;
	HearingRange = ArchetypeData->HearingRange;
	AwarenessGainPerSecond = ArchetypeData->AwarenessGainPerSecond;
	AwarenessDecayPerSecond = ArchetypeData->AwarenessDecayPerSecond;
	AwarenessState = ResolveAwarenessState(Awareness);
}

void UGuardAwarenessComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void UGuardAwarenessComponent::AddAwareness(float DeltaAwareness)
{
	Awareness = FMath::Clamp(Awareness + DeltaAwareness, 0.f, 1.f);
	UpdateAwarenessStateAndEmitEvent();
}

EGuardAwarenessState UGuardAwarenessComponent::ResolveAwarenessState(float InAwareness) const
{
	if (InAwareness >= 0.75f)
	{
		return EGuardAwarenessState::Alerted;
	}

	if (InAwareness >= 0.33f)
	{
		return EGuardAwarenessState::Suspicious;
	}

	return EGuardAwarenessState::Calm;
}

void UGuardAwarenessComponent::UpdateAwarenessStateAndEmitEvent()
{
	const EGuardAwarenessState NewState = ResolveAwarenessState(Awareness);
	if (NewState == AwarenessState)
	{
		return;
	}

	AwarenessState = NewState;

	FText StateLabel = NSLOCTEXT("SneakGear", "AwarenessCalm", "Calm");
	if (AwarenessState == EGuardAwarenessState::Suspicious)
	{
		StateLabel = NSLOCTEXT("SneakGear", "AwarenessSuspicious", "Suspicious");
	}
	else if (AwarenessState == EGuardAwarenessState::Alerted)
	{
		StateLabel = NSLOCTEXT("SneakGear", "AwarenessAlerted", "Alerted");
	}

	if (UWorld* World = GetWorld())
	{
		if (UEventLogSubsystem* EventLog = World->GetSubsystem<UEventLogSubsystem>())
		{
			EventLog->ReportGuardAwarenessChanged(GetOwner(), StateLabel);
		}
	}
}

bool UGuardAwarenessComponent::CanSeeTarget(const AActor* Target, float& OutVisionScore) const
{
	OutVisionScore = 0.f;
	const AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return false;
	}

	const FVector Eyes = Owner->GetActorLocation() + FVector(0.f, 0.f, 70.f);
	const FVector TargetPoint = Target->GetActorLocation() + FVector(0.f, 0.f, 60.f);

	const FVector ToTarget = TargetPoint - Eyes;
	const float Dist = ToTarget.Length();
	if (Dist > VisionRange)
	{
		return false;
	}

	const FVector Forward = Owner->GetActorForwardVector();
	const FVector Dir = ToTarget / FMath::Max(Dist, 1.f);

	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(VisionHalfAngleDeg));
	const float Dot = FVector::DotProduct(Forward, Dir);
	if (Dot < CosHalfAngle)
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(StealthVision), true);
	Params.AddIgnoredActor(Owner);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Eyes, TargetPoint, ECC_Visibility, Params);

	if (bHit && Hit.GetActor() != Target)
	{
		return false;
	}

	const float DistFactor = 1.f - FMath::Clamp(Dist / VisionRange, 0.f, 1.f);
	const float Dot01 = FMath::Clamp((Dot - CosHalfAngle) / (1.f - CosHalfAngle), 0.f, 1.f);
	OutVisionScore = DistFactor * Dot01;
	return true;
}

void UGuardAwarenessComponent::DrawDebugVision(const AActor* Target, bool bCanSee) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return;
	}

	const FVector Eyes = Owner->GetActorLocation() + FVector(0.f, 0.f, 70.f);
	const FVector Forward = Owner->GetActorForwardVector();

	const FRotator BaseRot = Forward.Rotation();
	const FVector LeftDir = (BaseRot + FRotator(0.f, -VisionHalfAngleDeg, 0.f)).Vector();
	const FVector RightDir = (BaseRot + FRotator(0.f, VisionHalfAngleDeg, 0.f)).Vector();
	const FColor ConeColor = bCanSee ? FColor::Green : FColor::Red;

	DrawDebugLine(GetWorld(), Eyes, Eyes + LeftDir * VisionRange, ConeColor, false, 0.f, 0, 1.5f);
	DrawDebugLine(GetWorld(), Eyes, Eyes + RightDir * VisionRange, ConeColor, false, 0.f, 0, 1.5f);
	DrawDebugLine(GetWorld(), Eyes, Eyes + Forward * VisionRange, ConeColor, false, 0.f, 0, 1.5f);
}
