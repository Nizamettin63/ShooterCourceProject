// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Engine/DataTable.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	EET_Grux UMETA(DisplayName = "Grux"),
	EET_GruxBeetleRed UMETA(DisplayName = "GruxBeetleRed"),
	EET_GruxChestPlate UMETA(DisplayName = "GruxChestPlate"),
	EET_GruxHalloween UMETA(DisplayName = "GruxHalloween"),
	EET_GruxMolten UMETA(DisplayName = "GruxMolten"),
	EET_GruxQilin UMETA(DisplayName = "GruxQilin"),
	EET_GruxWarChief UMETA(DisplayName = "GruxWarChief"),

	EET_Default UMETA(DisplayName = "Default")
};

USTRUCT(BlueprintType)
struct FEnemyDataTable : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMesh* SkinMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector MeshScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D HealthBarScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector HealthbarRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CapsuleRadius;
};


UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintNativeEvent)
		void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
		void HideHealthBar();

	void Die();

	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	void ResetHitReactTimer();


	UFUNCTION(BlueprintCallable)
		void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
		void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	/* Called when somethings overlap with the agro sphere*/
	UFUNCTION()
		void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SetStunned(bool Stunned);

	UFUNCTION()
		void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
		void CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void PlayAttackMontage(FName Section, float PlayRate);

	UFUNCTION(BlueprintPure)
		FName GetAttackSectionName();

	UFUNCTION()
		void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Activate / Deactivate for Collsion Boxes
	UFUNCTION(BlueprintCallable)
		void ActivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
		void DeactivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
		void ActivateRightWeapon();
	UFUNCTION(BlueprintCallable)
		void DeactivateRightWeapon();

	void DoDamage(class AShooterCharacter* Victim);
	void SpawnBlood(AShooterCharacter* Victim, FName SocketName);

	// Attempt to stun character
	void StunCharacter(AShooterCharacter* Victim);

	void ResetCanAttack();

	UFUNCTION(BlueprintCallable)
		void FinishDeath();

	UFUNCTION()
		void DestroyEnemy();

	UFUNCTION(BlueprintPure)
		float HealthBarScaler();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		EEnemyType EnemyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* HealthBar;


	class UCharacterMovementComponent* MovementComponentPtr;


	class UCapsuleComponent* CapsuleComponent;


	/* Particles to spawn when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* ImpactParticles;

	/* Sound to play when hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USoundCue* ImpactSound;

	/* Current health of enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float Health;

	/* Maximum health of enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaxHealth;

	/* Name of the head bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		FString HeadBone;

	/** Time to display health bar once shot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float HealthBarDisplayTime;


	FTimerHandle HealthBarTimer;

	/** Montage Comtaining  Hit and Death Animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float HitReactTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float HitReactTimeMax;

	bool bCanHitReact;


	/* Map to store hit number widget and their hit Locations */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TMap<UUserWidget*, FVector> HitNumbers;

	/* Time before a hitnumber is removed from the screen*/
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float HitNumberDetroyTime;

	/* Behavior Tree for the Aý Charachter*/
	UPROPERTY(EditAnywhere, Category = BehaviorTree, meta = (AllowPrivateAccess = "true"))
		class UBehaviorTree* BehaviorTree;

	/* Point for the Enemy to move to */
	UPROPERTY(EditAnywhere, Category = BehaviorTree, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPoint;

	/* Point for the Enemy to move to */
	UPROPERTY(EditAnywhere, Category = BehaviorTree, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPoint2;

	class AEnemyController* EnemyController;

	/* Overlap Sphere when the enemy become hostile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AgroSphere;


	/* True when playing the get hit animation*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bStunned;

	/* Chance of being stunned. 0: no stun chance, 1: 100% stun chance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float StunChance;

	/* True when in attack range time to attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bInAttackRange;

	/* Sphere for attack range*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		USphereComponent* CombatRangeSphere;


	/** Montage Comtaining different Attacks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AttackMontage;

	/* Four attack montage section names */
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;

	/* Collision Weapon for left weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* LeftWeaponCollision;

	/* Collision Weapon for right weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* RightWeaponCollision;

	/* Base damage for Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float BaseDamage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		FName LeftWeaponSocket;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		FName RightWeaponSocket;


	/* True when enemy can attack */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	FTimerHandle AttackWaitTimer;

	/* Minimum wait time between attacks*/
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float AttackWaitTime;

	/* Death Anim montage for the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDying;


	FTimerHandle DeathTimer;

	/* Time after death until destroy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float DeathTime;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float HealthBarScaleMultiply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float HealthBarScaleMultiplyAim;

	float RenderScaleMultiply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StunCount;

public:
	// Called every frame	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* ShooterActor, AController* ShooterController) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; } 
	FORCEINLINE void SetStunChange(float Chance) { StunChance = Chance; }
};
