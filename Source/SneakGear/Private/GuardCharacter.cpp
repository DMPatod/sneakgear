#include "GuardCharacter.h"

#include "GuardAIController.h"
#include "Kismet/GameplayStatics.h"

AGuardCharacter::AGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AGuardAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGuardCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && Controller == nullptr)
	{
		SpawnDefaultController();
	}

	if (!TargetActor)
	{
		auto PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		TargetActor = PlayerPawn;
	}

	if (auto Aic = Cast<AGuardAIController>(GetController()))
	{
		Aic->SetPatrolPath(PatrolPath);
		Aic->MovetoNextPoint();
	}
}

bool AGuardCharacter::CanSeeTarget(const AActor* Target, float& OutVisionScore) const
{
	OutVisionScore = 0.f;
	if (!Target)
	{
		return false;
	}

	auto Eyes = GetActorLocation() + FVector(0.f, 0.f, 70.f);
	auto TargetPoint = Target->GetActorLocation() + FVector(0.f, 0.f, 60.f);

	auto ToTarget = TargetPoint - Eyes;
	auto Dist = ToTarget.Length();

	if (Dist > VisionRange)
	{
		return false;
	}

	auto Forward = GetActorForwardVector();
	auto Dir = ToTarget / FMath::Max(Dist, 1.f);

	// Cone check
	auto CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(VisionHalfAngleDeg));
	auto Dot = FVector::DotProduct(Forward, Dir);
	if (Dot < CosHalfAngle)
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(StealthVision), true);
	Params.AddIgnoredActor(this);

	auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Eyes, TargetPoint, ECC_Visibility, Params);

	if (bHit && Hit.GetActor() != Target)
	{
		return false;
	}

	auto DistFactor = 1.f - FMath::Clamp(Dist / VisionRange, 0.f, 1.f);
	auto Dot01 = FMath::Clamp((Dot - CosHalfAngle) / (1.f - CosHalfAngle), 0.f, 1.f);

	OutVisionScore = DistFactor * Dot01;
	return true;
}

void AGuardCharacter::DrawDebugVision(const AActor* Target, bool bCanSee, float VisionScore) const
{
	if (!Target)
	{
		return;
	}

	auto Eyes = GetActorLocation() + FVector(0.f, 0.f, 70.f);
	auto Forward = GetActorForwardVector();

	auto BaseRot = Forward.Rotation();
	auto LeftRot = BaseRot + FRotator(0.f, -VisionHalfAngleDeg, 0.f);
	auto RightRot = BaseRot + FRotator(0.f, VisionHalfAngleDeg, 0.f);

	auto LeftDir = LeftRot.Vector();
	auto RightDir = RightRot.Vector();

	auto ConeColor = bCanSee ? FColor::Green : FColor::Red;

	DrawDebugLine(GetWorld(), Eyes, Eyes + LeftDir * VisionRange, ConeColor, false, 0.f, 0, 1.5f);
	DrawDebugLine(GetWorld(), Eyes, Eyes + RightDir * VisionRange, ConeColor, false, 0.f, 0, 1.5f);
	DrawDebugLine(GetWorld(), Eyes, Eyes + Forward * VisionRange, ConeColor, false, 0.f, 0, 1.5f);

	auto TargetPoint = Target->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	DrawDebugLine(GetWorld(), Eyes, TargetPoint, ConeColor, false, 0.f, 0, 2.f);

	auto TextLoc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
	auto Txt = FString::Printf(TEXT("A: %.2f, V: %.2f"), Awareness, VisionScore);
	DrawDebugString(GetWorld(), TextLoc, Txt, nullptr, FColor::White, 0.f, false);
}

void AGuardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetActor)
	{
		return;
	}

	auto VisionScore = 0.f;
	auto bCanSee = CanSeeTarget(TargetActor, VisionScore);
	bHasLineOfSight = bCanSee;

	if (bCanSee)
	{
		Awareness = FMath::Clamp(Awareness + VisionScore * AwarenessGainPerSecond * DeltaTime, 0.f, 1.f);
	}
	else
	{
		Awareness = FMath::Clamp(Awareness - AwarenessDecayPerSecond * DeltaTime, 0.f, 1.f);
	}

	DrawDebugVision(TargetActor, bCanSee, VisionScore);

	if (GEngine)
	{
		auto Msg = FString::Printf(TEXT("Awareness: %.2f    LOS:"), Awareness, bCanSee ? TEXT("YES") : TEXT("NO"));
		GEngine->AddOnScreenDebugMessage((uint64)(PTRINT)this, 0.f, FColor::Cyan, Msg);
	}
}
