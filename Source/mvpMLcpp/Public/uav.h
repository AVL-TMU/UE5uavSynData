// uav.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "uav.generated.h"

UCLASS()
class MVPMLCPP_API Auav : public AActor
{
    GENERATED_BODY()

public:
    Auav();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    /** ====== Components ====== */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UAV|Components")
    class UStaticMeshComponent* Body;   // root mesh

    /** ====== Tunables (editable) ====== */
    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float LoiterRadius = 8000.f;          // cm (80 m)

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float CruiseSpeed = 2500.f;           // cm/s (~25 m/s)

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float BaseAltitude = 12000.f;         // cm (120 m)

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float ClimbAmplitude = 2500.f;        // cm

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float ClimbFreq = 0.08f;              // Hz

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float InitialClimbMeters = 60.f;      // m

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float MaxBankDeg = 45.f;              // deg

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float RotInterpSpeed = 4.f;           // rot smoothing

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float VelInterpSpeed = 6.f;           // vel smoothing

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float YawForwardOffset = 0.f;         // mesh forward-axis fix

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    float GravityCm = 980.f;              // cm/s^2 (bank calc)

    UPROPERTY(EditAnywhere, Category = "UAV|Flight")
    FRotator MeshAxisOffset = FRotator(0.f, 0.f, 0.f); // set in editor, e.g. Pitch=-90 if nose is +Z

private:
    float TimeElapsed = 0.f;
    FVector PrevPos = FVector::ZeroVector;
    FVector PrevVel = FVector::ZeroVector;
    float PrevYawDeg = 0.f;
};
