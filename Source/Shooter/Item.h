// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Ledendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMax")
};
UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "Pickedup"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
  	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStarts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageMultiplier;
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called When Overlapping AreaSphere
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called When End Overlapping AreaSphere
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	/* Set the active stars arrat of bools based rarity */
	void SetActiveStars();

	/* Set properties of ýtem's  components based on state*/
	virtual void SetItemProperties(EItemState State);

	/* Called when ItemInterpTimer is finished*/
	void FinishInterping();

	/* Handles ýtem interpolation when in the equipInterping state*/
	void ItemInterp(float DeltaTime);

	/*Get Interp Location based on the Item type*/
	FVector GetInterpLocation();


	void PlayPickupSound(bool bForcePlaySound = false);

	
	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& FTransform) override;

	void EnableGlowMaterial();

	void UpdatePulse();
	void ResetPulseTimer();
	void StartPulseTimer();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called in a ShooterCharacter::GetPickup
	void PlayEquipSound(bool bForcePlaySound = false);
private:

	/* Skeletak mesh fo the item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ItemMesh;

	/* Line trace collides with box to show hud widgets*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionBox;

	/* Popup widget for when the player look at the item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;

	/* The name which appears on the pickup widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		FString ItemName;

	/* Item count amo Etc.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		int32  ItemCount;

	/* Item rarity - determinesnumber of the stars in pickup widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
		EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		TArray<bool> ActiveStars;

	/* State of the Item*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
		EItemState ItemState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	/* Starting location when interping begins*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/* Target ýnterp location iþn front of the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;

	/* true when interping*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;

	/* Plays when we start interping*/
	FTimerHandle ItemInterpTimer;

	/* Duration of the curve and timer*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	/* Pointer to the charater*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* Character;

	/* X and Y for the ýtem while ýnterping in the EquipInterping state*/
	float ItemInterpX;
	float ItemInterpY;

	/*Initial Yaw offset between the camera and the ýnterping item*/
	float InterpInitialYawOffset;

	/* Curve used to scale the item when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	/* Sound played when ýtem is  picked up*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* PickupSound;

	/* Sound played when the ýtem is equipped*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	/* Enum for the type of the ýtem this item is*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	/*Index of the interp location this item is interping to*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex;

	/*Index for the material*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;

	/* Dynamic Instace that we can change at runtime*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	/*Material Instance using with the dynamic material ýnstance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;


	bool bCanChangeCustomDepth;

	/*Curve to drive the dynamic material parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	class UCurveVector* PulseCurve;

	/*Curve to drive the dynamic material parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;


	FTimerHandle PulseTimer;

	/* Time for the PulseTimer*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	float PulseCurveTime;

	UPROPERTY(VisibleAnywhere, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	float GlowAmount;

	
	UPROPERTY(VisibleAnywhere, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	float FresnelExponent;


	UPROPERTY(VisibleAnywhere, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	float FresnelReflectFraction;

	

	/* Icon for this item in inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconItem;


	/*  Ammo Icon for this item in inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoItem;

	/* Slot in the ýnventory array*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemProperties", meta = (AllowPrivateAccess = "true"))
	bool bCharacterInventoryFull;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	class UDataTable* ItemRarityDataTable;

	/* Light Color of the in pickup widget*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColor;

	/* Glow Color of the in pickup widget*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor;

	/* Dark Color of in the pickup widget*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColor;

	/* Number of star in pickup widget*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfStars;

	/* Background icon for the inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	float DamageMultiplier;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere()   const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox()   const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetItemMesh()  const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const{ return PickupSound; }
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE void SetEquipSound(USoundCue* Sound) { EquipSound = Sound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetCharacter(AShooterCharacter* Char) { Character = Char; }
	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }
	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	//Set item icon for the ýnventory
	FORCEINLINE void SetIconItem(UTexture2D* Icon) { IconItem = Icon; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* Icon) { AmmoItem = Icon; }
	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Instance) { MaterialInstance = Instance; }
	FORCEINLINE UMaterialInstance* GetMaterialInstance() { return MaterialInstance; }
	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* Instance) { DynamicMaterialInstance = Instance; }
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() { return DynamicMaterialInstance; }
	FORCEINLINE FLinearColor GetGlowColor() const { return GlowColor; }
	FORCEINLINE int32 GetMaterialIndex() const { return MaterialIndex; }
	FORCEINLINE void SetMaterialIndex(int32 Index) { MaterialIndex = Index; }
	FORCEINLINE float GetDamageMultiplier() const { return DamageMultiplier; }
	/*Called from the AShooterCharacter class*/
	void StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound = false);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	
	void DisableGlowMaterial();
}; 