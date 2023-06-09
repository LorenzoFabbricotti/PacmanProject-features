// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridPawn.h"
#include "MyGameInstance.h"
#include "PhantomPawn.generated.h"

/**
 * 
 */
class SoundCue;

UENUM()
enum EEnemyState{
	Chase, //ghost chases pacman
	Frightened, //ghost can be eat by pacman
	Dead //ghost eaten by player when in frightened state
};


UCLASS()
class PACMANGRID_API APhantomPawn : public AGridPawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	APhantomPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnNodeReached() override;

private:
	UPROPERTY(VisibleAnywhere)
		class APacmanPawn* Player;


public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		APacmanPawn* GetPlayer() const;
	UFUNCTION()
		void SetSpeed(float Speed);
	UFUNCTION()
		virtual AGridBaseNode* GetPlayerRelativeTarget();
	void SetGhostTarget();

	FTimerHandle DeadStateTimer;

	// game instance reference
	UMyGameInstance* GameInstance;

	UPROPERTY(EditAnywhere)
		USoundCue* PacmanDeadSound;
	UPROPERTY(EditAnywhere)
		USoundCue* GhostDeadSound;

	//skins
	UPROPERTY(EditAnywhere)
		UMaterialInterface* DefaultSkin;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* VulnerableSkin;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* DeadSkin;

	//respawn starting position
	virtual void RespawnGhostStartingPosition();

	//go back in assigned position (when eaten by player)
	virtual void GoHome();

	//gestione stati
	UPROPERTY(EditAnywhere, Category = "Ghost State", meta = (DisplayName = "Ghost State"))
		TEnumAsByte<EEnemyState> EEnemyState = Chase;

	void SetChaseState();
	bool IsChaseState();

	void SetFrightenedState();
	bool IsFrightenedState();

	void SetDeadState();
	bool IsDeadState();

	UPROPERTY(EditAnywhere, Category = "Ghost State", meta = (DisplayName = "UscitaGhost"))
	bool UscitaGhost;

	void ExitGhostArea();
};
