// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_Max UMETA(DisplayName = "DefaultMAX")
};
/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:


	/* Handle turn in place variables*/
	void TurnInPlace();

	/* Handle calculations for leaning while running*/
	void Lean(float DeltaTime);
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta =(AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	// Speed of the Chracter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	// Whether or not chacter is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
    
	//Whether or not chracter is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* Offset yaw used for strafing*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/*Offset yaw  the frame before we  stopped moving*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/* Yaw of the Character this frame; Only updated when standing still and not in air*/
	float TIPCharacterYaw;

	/* Yaw of the character previous frame; Only updated when standing still and not in air*/
	float TIPCharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;


	/* Rotation curve value this frame*/
	float RotationCurve;

	/*Rotation Curve value last frame*/
	float RotationCurveValueLastFrame;

	/* The pitch of the aim rotation, used for Anim offset*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	/* True when reloading, used to prevent aim offset when reloading*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	/* Offset State; used to determine which Aim Offset to use*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	/* Character yaw this frame*/
	FRotator CharacterRotation;

	/* Character Yaw last frame*/
	FRotator CharacterRotationLastFrame;

	/* Yaw delta used for leaning in the running blendspace*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	/* True when crouching*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;


	/* True when equipping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	/* Change the recoil weight based on turning in place and aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	/*True when turning in place*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

	/* Weapon type for the currently equpped weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;


	/* True when not reloading and equipping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};
