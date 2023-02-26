// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Camera/CameraComponent.h"
#include "Math/Vector.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "ShooterCharacter.h"	
// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	bCanHitReact(true),
	HitNumberDetroyTime(1.5f),
	bStunned(false),
	StunChance(0.5f),
	AttackLFast(TEXT("AttackLFast")),
	AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")),
	AttackR(TEXT("AttackR")),
	BaseDamage(20.f),
	LeftWeaponSocket(TEXT("FX_Trail_L_01")),
	RightWeaponSocket(TEXT("FX_Trail_R_01")),
	bCanAttack(true),
	AttackWaitTime(1.f),
	bDying(false),
	DeathTime(4.f),
	HealthBarScaleMultiply(40.f),
	HealthBarScaleMultiplyAim(100.f)

{
	/* Set this character to call Tick() every frame.
	You can turn this off to improve performance if you don't need it. */
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetMesh());

	// Create the combat Range Sphere
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetMesh());

	// Weapon Collisions
	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));


	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);



}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeBeginOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	// Bind functions to overlaps event for weapon boxes
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	//Set collision presents for weapon boxes
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// get Aý controller
	EnemyController = Cast<AEnemyController>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);



	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);

		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
}

void AEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString EnemyDataTablePath(TEXT("DataTable'/Game/Game/DataTables/EnemyDatatable.EnemyDatatable'"));

	UDataTable* EnemyDataTableObject = Cast<UDataTable>(StaticLoadObject
	(UDataTable::StaticClass(), nullptr, *EnemyDataTablePath));



	if (EnemyDataTableObject == nullptr) return;


	FEnemyDataTable* EnemyDataRow = nullptr;

	switch (EnemyType)
	{
	case EEnemyType::EET_Grux:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("Grux"), TEXT(""));
		break;
	case EEnemyType::EET_GruxBeetleRed:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxBeetleRed"), TEXT(""));
		break;
	case EEnemyType::EET_GruxChestPlate:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxChestPlate"), TEXT(""));
		break;
	case EEnemyType::EET_GruxHalloween:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxHalloween"), TEXT(""));
		break;
	case EEnemyType::EET_GruxMolten:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxMolten"), TEXT(""));
		break;
	case EEnemyType::EET_GruxQilin:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxQilin"), TEXT(""));
		break;
	case EEnemyType::EET_GruxWarChief:

		EnemyDataRow = EnemyDataTableObject->FindRow<FEnemyDataTable>(FName("GruxWarChief"), TEXT(""));
		break;
	default:
		EnemyDataRow = nullptr;
		break;
	}

	if (EnemyDataRow == nullptr) return;


	if (MovementComponentPtr == nullptr)
	{
		MovementComponentPtr = Cast<UCharacterMovementComponent>(GetCharacterMovement());
		MovementComponentPtr->MaxWalkSpeed = EnemyDataRow->Speed;

	}

	if (CapsuleComponent == nullptr)
	{

		CapsuleComponent = Cast<UCapsuleComponent>(GetCapsuleComponent());
		CapsuleComponent->SetCapsuleHalfHeight(EnemyDataRow->CapsuleHalfHeight);
		CapsuleComponent->SetCapsuleRadius(EnemyDataRow->CapsuleRadius);
	}

	GetMesh()->SetSkeletalMesh(EnemyDataRow->SkinMesh);
	GetMesh()->SetWorldScale3D(EnemyDataRow->MeshScale);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, EnemyDataRow->CapsuleHalfHeight * (-1)));

	BaseDamage = EnemyDataRow->BaseDamage;
	Health = EnemyDataRow->Health;
	MaxHealth = EnemyDataRow->Health;
	HealthBar->SetDrawSize(EnemyDataRow->HealthBarScale);
	HealthBar->SetRelativeLocation(EnemyDataRow->HealthbarRelativeLocation);

	/*Combat ve Agro sphere oto ölçekleme için meshe attachlandý ancak orta
	noktasý CapsuleHalfHeight kadar kaydýðý için Z lokasyonlarýna ekleme yapýldý*/
	CombatRangeSphere->SetRelativeLocation(FVector(0.f, 0.f, EnemyDataRow->CapsuleHalfHeight));
	AgroSphere->SetRelativeLocation(FVector(0.f, 0.f, EnemyDataRow->CapsuleHalfHeight));
}


void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}


void AEnemy::Die()
{
	if (bDying) return;
	bDying = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    
	HideHealthBar();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), true);
		EnemyController->StopMovement();
	}
}


void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}
		bCanHitReact = false;
		const float HitReactTime = FMath::RandRange(HitReactTimeMin, HitReactTimeMax);
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
	}
}


void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}


void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDetroyTime, false);
}


void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}


void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber = HitPair.Key;
		FVector Location = HitPair.Value;
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}


void AEnemy::SetStunned(bool Stunned)
{
	StunCount++;
	bStunned = Stunned;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}


void AEnemy::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	bInAttackRange = true;

	auto ShooterChracter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterChracter)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
}


 void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
	
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemy::ResetCanAttack, AttackWaitTime);

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
}

FName AEnemy::GetAttackSectionName()
{
	const int32 Section = FMath::RandRange(1, 4);
	FName SectionName;
	switch (Section)
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}

	return SectionName;
}


void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character, LeftWeaponSocket);
		StunCharacter(Character);
	}
}


void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character, RightWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (Victim == nullptr) return;

	if (Victim)
	{
		UGameplayStatics::ApplyDamage(Victim, BaseDamage, EnemyController, this, UDamageType::StaticClass());

		if (Victim->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Victim->GetMeleeImpactSound(),
				Victim->GetActorLocation());
		}
	}
}

void AEnemy::SpawnBlood(AShooterCharacter* Victim, FName SocketName)
{
	if (Victim == nullptr) return;

	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (TipSocket)
	{
		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetBloodParticles())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticles(), SocketTransform);
		}
	}

}

void AEnemy::StunCharacter(AShooterCharacter* Victim)
{
	if (Victim)
	{
		const float Stun = FMath::FRandRange(0.f, 1.f);

		//Stun the victim if Stun chance is bigger than stun
		if (Victim->GetStunChange() >= Stun)
		{
			Victim->Stun();
		}
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::DestroyEnemy, DeathTime);

}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

float AEnemy::HealthBarScaler()
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player == nullptr) return 1;

	FVector PlayerCameraLocation = Player->FindComponentByClass<UCameraComponent>()->GetComponentLocation();


	float DistanceDifference = FVector::Distance(GetActorLocation(), PlayerCameraLocation);
	RenderScaleMultiply = HealthBarScaleMultiply / FMath::Sqrt(DistanceDifference);

	if (Player->GetAiming())
	{
		RenderScaleMultiply = HealthBarScaleMultiplyAim / FMath::Sqrt(DistanceDifference);
	}
	else
	{
		RenderScaleMultiply = HealthBarScaleMultiply / FMath::Sqrt(DistanceDifference);
	}

	return RenderScaleMultiply;
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto ShooterChracter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterChracter)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
}


void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || EnemyController == nullptr) return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		// Set the value of the target blackboard key
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
	}
}


// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();
}


// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* ShooterActor, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
}


float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bDying) return DamageAmount;

	// Set the target blackboard key to agro the character
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
	}
	 
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	if (bDying) return DamageAmount;

	ShowHealthBar();

	//Determine whether bullet hit stun
	const float Stunned = FMath::FRandRange(0.f, 1.f);

	if (Stunned <= StunChance || StunCount <= 0)
	{
		//Stun the enemy
		PlayHitMontage(FName("HitReactFront"));
		
		SetStunned(true);
	}

	return DamageAmount;
}

