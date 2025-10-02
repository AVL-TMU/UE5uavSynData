// camactor.cpp

#include "camactor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

Acamactor::Acamactor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void Acamactor::BeginPlay()
{
    Super::BeginPlay();

    TimeElapsed = 0.f;
    ReacquireTimer = 0.f;

    if (!TargetActor)
    {
        TryAcquireTarget();
    }

    if (AutoBecomeViewTarget)
    {
        EnsureViewTarget();
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(9001, 3.0f, FColor::Cyan, TEXT("[camactor] BeginPlay OK"));
    }
}

void Acamactor::EnsureViewTarget()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        // Smoothly become the active camera for Player0
        PC->SetViewTargetWithBlend(this, /*BlendTime=*/0.25f);
    }
}

void Acamactor::TryAcquireTarget()
{
    if (TargetActor) return;

    if (TargetTag != NAME_None)
    {
        TArray<AActor*> Tagged;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetTag, Tagged);
        if (Tagged.Num() > 0)
        {
            TargetActor = Tagged[0];
        }
    }

    if (!TargetActor && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1001, 1.0f, FColor::Red,
            FString::Printf(TEXT("[camactor] No target yet (Tag: %s). Set TargetActor or add tag."),
                *TargetTag.ToString()));
    }
}

void Acamactor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    TimeElapsed += FMath::Max(DeltaSeconds, KINDA_SMALL_NUMBER);

    // Keep reacquiring until we find a target (covers late spawns)
    if (!TargetActor)
    {
        ReacquireTimer += DeltaSeconds;
        if (ReacquireTimer >= ReacquirePeriod)
        {
            ReacquireTimer = 0.f;
            TryAcquireTarget();
        }
        return;
    }

    const FVector TPos = TargetActor->GetActorLocation();
    const FVector TFwd = TargetActor->GetActorForwardVector();
    const FVector TRight = TargetActor->GetActorRightVector();
    const FVector Up = FVector::UpVector;

    const float TwoPi = 2.f * PI;

    // Weaves (deterministic; no randomness)
    const float RadialIn = RadialWeaveAmp * FMath::Sin(TwoPi * RadialWeaveFreq * TimeElapsed + PhaseRadial);
    const float Lateral = LateralWeaveAmp * FMath::Sin(TwoPi * LateralWeaveFreq * TimeElapsed + PhaseLateral);

    const float DesiredDist = FMath::Max(100.f, FollowDistance + RadialIn);
    const FVector DesiredPos = TPos - TFwd * DesiredDist + TRight * Lateral + Up * HeightOffset;

    const FVector CurrentPos = GetActorLocation();
    const FVector NewPos = FMath::VInterpTo(CurrentPos, DesiredPos, DeltaSeconds, PosInterpSpeed);

    // Look-at with framing drift
    FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(NewPos, TPos);
    const float YawDrift = FramingYawDeg * FMath::Sin(TwoPi * (RadialWeaveFreq * 0.8f) * TimeElapsed + PhaseYaw);
    const float PitchDrift = FramingPitchDeg * FMath::Sin(TwoPi * (LateralWeaveFreq * 0.65f) * TimeElapsed + PhasePitch);
    LookAt.Yaw += YawDrift;
    LookAt.Pitch += PitchDrift;

    const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), LookAt, DeltaSeconds, RotInterpSpeed);

    // Apply transform (no TeleportPhysics needed for a camera)
    SetActorLocationAndRotation(NewPos, NewRot, /*bSweep=*/false);

    // Optional debug
    if (GEngine)
    {
        const FString Msg = FString::Printf(
            TEXT("[camactor] Following %s | Dist %.1fm | Lateral %.1fm"),
            *TargetActor->GetName(), (DesiredDist / 100.f), (Lateral / 100.f));
        GEngine->AddOnScreenDebugMessage(42, 0.f, FColor::Yellow, Msg);
    }
}
