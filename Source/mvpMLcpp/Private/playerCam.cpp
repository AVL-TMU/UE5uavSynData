// Fill out your copyright...

#include "playerCam.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AplayerCam::AplayerCam()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AplayerCam::BeginPlay()
{
    Super::BeginPlay();
    TimeElapsed = 0.f;

    // Auto-find a UAV actor if none set in editor
    if (!TargetActor)
    {
        // If you have Auav class header, you can search by class.
        // Otherwise, tag your UAV "UAV" and use GetAllActorsWithTag.
        // Here: try class search first, then tag as fallback.

        // Try by class name (soft): find any actor named "uav" class.
        // If you have the class: AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), Auav::StaticClass());

        // Generic fallback by tag:
        TArray<AActor*> Tagged;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("UAV")), Tagged);
        if (Tagged.Num() > 0)
        {
            TargetActor = Tagged[0];
        }
        else
        {
            // Last resort: just pick any other actor in the world (do nothing if none)
            // Leaving TargetActor null keeps camera stationary.
        }
    }
}

// Called every frame
void AplayerCam::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeElapsed += FMath::Max(DeltaTime, KINDA_SMALL_NUMBER);

    if (!TargetActor)
    {
        // No target—do nothing (or keep previous circle logic if you want).
        return;
    }

    // === Target frame ===
    const FVector TPos = TargetActor->GetActorLocation();
    const FRotator TRot = TargetActor->GetActorRotation();
    const FVector TFwd = TargetActor->GetActorForwardVector();
    const FVector TRight = TargetActor->GetActorRightVector();
    const FVector Up = FVector::UpVector;

    // === Deterministic weave (no randomness) ===
    const float TwoPi = 2.f * PI;

    // In/Out distance modulation → causes frequent enter/exit of frame
    const float RadialIn = RadialWeaveAmp * FMath::Sin(TwoPi * RadialWeaveFreq * TimeElapsed + PhaseRadial);

    // Lateral side-step → keeps UAV off-center at times
    const float Lateral = LateralWeaveAmp * FMath::Sin(TwoPi * LateralWeaveFreq * TimeElapsed + PhaseLateral);

    // Desired camera anchor (behind + to the side + above)
    const float DesiredDist = FMath::Max(100.f, FollowDistance + RadialIn);
    const FVector DesiredPos =
        TPos
        - TFwd * DesiredDist
        + TRight * Lateral
        + Up * HeightOffset;

    // Smooth position
    const FVector NewPos = FMath::VInterpTo(GetActorLocation(), DesiredPos, DeltaTime, PosInterpSpeed);

    // Look-at with framing offsets (yaw/pitch drift so UAV not always centered)
    FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(NewPos, TPos);

    const float YawDrift = FramingYawDeg * FMath::Sin(TwoPi * (RadialWeaveFreq * 0.8f) * TimeElapsed + PhaseYaw);
    const float PitchDrift = FramingPitchDeg * FMath::Sin(TwoPi * (LateralWeaveFreq * 0.65f) * TimeElapsed + PhasePitch);

    LookAt.Yaw += YawDrift;
    LookAt.Pitch += PitchDrift;

    const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), LookAt, DeltaTime, RotInterpSpeed);

    // Apply
    SetActorLocationAndRotation(NewPos, NewRot, false, nullptr, ETeleportType::TeleportPhysics);

    // Debug (optional)
    if (GEngine)
    {
        const FString Msg = FString::Printf(
            TEXT("CamDist %.1fm  Lateral %.1fm  YawOff %.1f°  PitchOff %.1f°"),
            DesiredDist / 100.f, Lateral / 100.f, YawDrift, PitchDrift);
        GEngine->AddOnScreenDebugMessage(42, 0.f, FColor::Yellow, Msg);
    }
}

// Called to bind functionality to input
void AplayerCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
