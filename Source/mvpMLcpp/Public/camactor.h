// camactor.h
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "camactor.generated.h"

UCLASS()
class MVPMLCPP_API Acamactor : public ACameraActor
{
    GENERATED_BODY()

public:
    Acamactor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    /** Explicit target (set in Details). If null, we’ll search by tag. */
    UPROPERTY(EditAnywhere, Category = "Follow")
    AActor* TargetActor = nullptr;

    /** If TargetActor not set, we search for this tag (add it to your UAV). */
    UPROPERTY(EditAnywhere, Category = "Follow")
    FName TargetTag = FName(TEXT("UAV"));

    /** How often (seconds) to retry finding the target when missing. */
    UPROPERTY(EditAnywhere, Category = "Follow")
    float ReacquirePeriod = 0.5f;

    /** Base follow distance behind target (cm). */
    UPROPERTY(EditAnywhere, Category = "Follow")
    float FollowDistance = 2500.f;

    /** Height above target (cm). */
    UPROPERTY(EditAnywhere, Category = "Follow")
    float HeightOffset = 600.f;

    /** Position smoothing speed. */
    UPROPERTY(EditAnywhere, Category = "Smoothing")
    float PosInterpSpeed = 3.5f;

    /** Rotation smoothing speed. */
    UPROPERTY(EditAnywhere, Category = "Smoothing")
    float RotInterpSpeed = 5.0f;

    /** Lateral weave amplitude (cm). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float LateralWeaveAmp = 700.f;

    /** Lateral weave frequency (Hz). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float LateralWeaveFreq = 0.09f;

    /** Radial in/out weave amplitude (cm). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float RadialWeaveAmp = 900.f;

    /** Radial in/out weave frequency (Hz). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float RadialWeaveFreq = 0.06f;

    /** Framing drift (degrees). */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float FramingYawDeg = 12.f;

    /** Framing drift (degrees). */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float FramingPitchDeg = 6.f;

    /** Optional phase offsets to desync motions. */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseLateral = 1.3f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseRadial = 2.1f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseYaw = 0.7f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhasePitch = 2.6f;

    /** If true, we’ll automatically switch Player0’s view to this camera at BeginPlay. */
    UPROPERTY(EditAnywhere, Category = "View")
    bool AutoBecomeViewTarget = true;

private:
    float TimeElapsed = 0.f;
    float ReacquireTimer = 0.f;

    void TryAcquireTarget();
    void EnsureViewTarget();
};
