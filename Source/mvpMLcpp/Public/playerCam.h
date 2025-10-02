// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "playerCam.generated.h"

UCLASS()
class MVPMLCPP_API AplayerCam : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AplayerCam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Target to follow (set in editor). If null, we’ll auto-find Auav at BeginPlay. */
    UPROPERTY(EditAnywhere, Category = "Follow")
    AActor* TargetActor = nullptr;

    /** How far behind the UAV (cm). */
    UPROPERTY(EditAnywhere, Category = "Follow")
    float FollowDistance = 2500.f;   // 25 m

    /** Height offset above the UAV (cm). */
    UPROPERTY(EditAnywhere, Category = "Follow")
    float HeightOffset = 600.f;      // 6 m

    /** How quickly camera position catches up. */
    UPROPERTY(EditAnywhere, Category = "Smoothing")
    float PosInterpSpeed = 3.5f;

    /** How quickly camera rotation catches up. */
    UPROPERTY(EditAnywhere, Category = "Smoothing")
    float RotInterpSpeed = 5.0f;

    /** Side-to-side weave amplitude (cm). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float LateralWeaveAmp = 700.f;

    /** Side-to-side weave frequency (Hz). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float LateralWeaveFreq = 0.09f;

    /** In/Out (distance) weave amplitude (cm). Positive pulls in; negative pushes out. */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float RadialWeaveAmp = 900.f;

    /** In/Out weave frequency (Hz). */
    UPROPERTY(EditAnywhere, Category = "Weave")
    float RadialWeaveFreq = 0.06f;

    /** Yaw framing offset amplitude (degrees). */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float FramingYawDeg = 12.f;

    /** Pitch framing offset amplitude (degrees). */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float FramingPitchDeg = 6.f;

    /** Optional phase shifts so the motion isn’t symmetric. */
    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseLateral = 1.3f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseRadial = 2.1f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhaseYaw = 0.7f;

    UPROPERTY(EditAnywhere, Category = "Framing")
    float PhasePitch = 2.6f;

private:
	float TimeElapsed = 0.f;

};
