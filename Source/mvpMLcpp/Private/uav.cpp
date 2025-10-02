// uav.cpp
#include "uav.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RotationMatrix.h"

// Sets default values
Auav::Auav()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Mesh as ROOT so it inherits actor rotation ---
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    RootComponent = Body;

    Body->SetMobility(EComponentMobility::Movable);
    Body->SetSimulatePhysics(false);
    Body->SetUsingAbsoluteRotation(false);
    Body->SetRelativeRotation(FRotator::ZeroRotator);
    Body->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void Auav::BeginPlay()
{
    Super::BeginPlay();

    FVector L = GetActorLocation();
    if (L.Z < BaseAltitude)
    {
        L.Z = BaseAltitude;
        SetActorLocation(L);
    }

    PrevPos = GetActorLocation();
    PrevYawDeg = GetActorRotation().Yaw;
}

// Called every frame
void Auav::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeElapsed += FMath::Max(DeltaTime, KINDA_SMALL_NUMBER);

    // --- Path: constant-speed loiter + vertical profile ---
    const float Omega = (LoiterRadius > 1.f) ? (CruiseSpeed / LoiterRadius) : 0.f; // rad/s
    const float Theta = Omega * TimeElapsed;

    const float X = LoiterRadius * FMath::Cos(Theta);
    const float Y = LoiterRadius * FMath::Sin(Theta);

    const float ClimbOsc = ClimbAmplitude * FMath::Sin(2.f * PI * ClimbFreq * TimeElapsed);
    const float InitialClimb = 100.f * InitialClimbMeters * (1.f - FMath::Exp(-TimeElapsed / 6.f));
    const float Z = BaseAltitude + ClimbOsc + InitialClimb;

    const FVector NewPos(X, Y, Z);

    // --- Smoothed velocity ---
    const FVector RawVel = (NewPos - PrevPos) / FMath::Max(DeltaTime, SMALL_NUMBER);
    const FVector SmoothedVel = (PrevVel.IsNearlyZero())
        ? RawVel
        : FMath::VInterpTo(PrevVel, RawVel, DeltaTime, VelInterpSpeed);

    FVector Fwd = SmoothedVel.GetSafeNormal();
    if (Fwd.IsNearlyZero())
    {
        Fwd = GetActorForwardVector(); // fallback
    }

    // Nose exactly along velocity (includes vertical pitch)
    const FQuat BaseQuat = FRotationMatrix::MakeFromX(Fwd).ToQuat();

    // Coordinated-turn bank around forward axis
    const float v = CruiseSpeed;
    const float TurnRadius = FMath::Max(LoiterRadius, 1.f);

    float BankRad = FMath::Atan((v * v) / (GravityCm * TurnRadius));
    float BankDeg = FMath::RadiansToDegrees(BankRad);

    const float TurnSign = (Omega >= 0.f) ? -1.f : 1.f;
    BankDeg = FMath::Clamp(BankDeg * TurnSign, -MaxBankDeg, MaxBankDeg);

    const FQuat BankQuat(Fwd, FMath::DegreesToRadians(BankDeg));

    // Combine + mesh-axis correction
    FQuat DesiredQuat = BankQuat * BaseQuat;
    DesiredQuat = DesiredQuat * MeshAxisOffset.Quaternion();

    // Smooth via SLERP
    const FQuat CurrentQuat = GetActorQuat();
    const float Alpha = FMath::Clamp(RotInterpSpeed * DeltaTime, 0.f, 1.f);
    const FQuat SmoothedQuat = FQuat::Slerp(CurrentQuat, DesiredQuat, Alpha).GetNormalized();

    // Apply
    SetActorLocationAndRotation(NewPos, SmoothedQuat, /*bSweep=*/false, nullptr, ETeleportType::None);

    // Debug info
    if (GEngine)
    {
        const FRotator DbgRot = SmoothedQuat.Rotator();

        FString PosMsg = FString::Printf(TEXT("Pos X %.1f  Y %.1f  Z %.1f"),
            NewPos.X, NewPos.Y, NewPos.Z);

        FString FwdMsg = FString::Printf(TEXT("Forward X %.2f  Y %.2f  Z %.2f"),
            Fwd.X, Fwd.Y, Fwd.Z);

        FString RotMsg = FString::Printf(TEXT("Speed %.1fm/s  Yaw %.1f  Pitch %.1f  Roll %.1f"),
            v / 100.f, DbgRot.Yaw, DbgRot.Pitch, DbgRot.Roll);

        GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, PosMsg);
        GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green, FwdMsg);
        GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Cyan, RotMsg);
    }

    PrevPos = NewPos;
    PrevVel = SmoothedVel;
}
