// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomPawn.h"
#include "PacmanPawn.h"
#include "TestGridGameMode.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

APhantomPawn::APhantomPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// get the game instance reference
	GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	DeadSkin = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Engine/MapTemplates/Sky/M_BlackBackground.M_BlackBackground'"));

	VulnerableSkin = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/M_Blue.M_Blue'"));
}

void APhantomPawn::BeginPlay()
{
	Super::BeginPlay();
	FVector2D StartNode = TheGridGen->GetXYPositionByRelativeLocation(GetActorLocation());
	LastNode = TheGridGen->TileMap[StartNode];

	Player = Cast<APacmanPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), APacmanPawn::StaticClass()));

	GameMode = (ATestGridGameMode*)(GetWorld()->GetAuthGameMode());
}

void APhantomPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	const auto Pacman = Cast<APacmanPawn>(OtherActor);

	//chase state
	if (this->IsChaseState())
	{
		if (Pacman && IsValid(GameInstance)) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("I Reached you")));

			//decrementa vita di 1
			int newvalue = (GameInstance->GetLives()) - 1;
			GameInstance->SetLives(newvalue);


			//play pacman dead sound
			UGameplayStatics::PlaySound2D(this, PacmanDeadSound);

			//respawn starting postion of pawns
			GameMode->RespawnPositions();

			//se player non ha più vite->destroy
			if ((GameInstance->GetLives()) <= 0)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("GAME OVER! YOU ARE DEAD!!!")));
				GameMode->GameOver();
			}
		}
	}

	else if (this->IsFrightenedState()) {
		if (Pacman) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("you killed a ghost")));

			UGameplayStatics::PlaySound2D(this, GhostDeadSound);

			int killings = Pacman->GetNumberOfGhostsKilled() + 1;
			Pacman->SetNumberOfGhostsKilled(killings);

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("ghosts killed: %d"), killings));

			this->SetDeadState();

			//score based on number of ghosts killed
			if (IsValid(GameInstance)) {
				int new_score = (GameInstance->getScore());

				switch (killings)
				{
				case 0:
					GameInstance->setScore(new_score);
					break;
				case 1:
					new_score += 400;
					GameInstance->setScore(new_score);
					break;
				case 2:
					new_score += 800;
					GameInstance->setScore(new_score);
					break;
				case 3:
					new_score += 1600;
					GameInstance->setScore(new_score);
					break;
				default:
					break;
				}
			}
		}
	}
}

void APhantomPawn::OnNodeReached()
{
	Super::OnNodeReached();
	//vuole entrare nella ghost area
	if (CurrentGridCoords == (FVector2D(17, 13)))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("trying to enter")));
		// Permetti il transito
		if (IsDeadState() || UscitaGhost)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("enter granted")));
		}

		//Non può entrare, prosegue per la sua direzione
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("enter denied")));
			LastValidInputDirection = PreviousDirection;
			SetNextNodeByDir(LastValidInputDirection, true);
			SetTargetNode(NextNode);
		}
	}

	//vuole uscire dalla ghost area
	else if ((CurrentGridCoords == (FVector2D(15, 13)) || CurrentGridCoords == (FVector2D(15, 12)) || CurrentGridCoords == (FVector2D(15, 14))) && (LastInputDirection.X > 0 || LastValidInputDirection.X > 0))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("trying to exit")));

		// Permetti il transito
		if ((IsChaseState() || IsFrightenedState()) && UscitaGhost)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("exit granted")));
			this->UscitaGhost = false;
		}
		// Blocca il transito
		else
		{
			if (CurrentGridCoords == FVector2D(15, 12)) {
				LastNode = (*(TheGridGen->TileMap.Find(FVector2D(15, 12))));
				//torna indietro
				FVector OppositeDirection = -GetLastValidDirection();
				SetNextNodeByDir(OppositeDirection, true);
				SetTargetNode(NextNode);
			}
			else if (CurrentGridCoords == FVector2D(15, 13)) {
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("exit denied")));
				LastNode = (*(TheGridGen->TileMap.Find(FVector2D(15, 13))));
				//torna indietro
				FVector OppositeDirection = -GetLastValidDirection();
				SetNextNodeByDir(OppositeDirection, true);
				SetTargetNode(NextNode);
			}

			else { // (CurrentGridCoords == FVector2D(15, 14))
				LastNode = (*(TheGridGen->TileMap.Find(FVector2D(15, 14))));
				//torna indietro
				FVector OppositeDirection = -GetLastValidDirection();
				SetNextNodeByDir(OppositeDirection, true);
				SetTargetNode(NextNode);
			}
		}
	}

	else if ((CurrentGridCoords == (FVector2D(17, 12)) || CurrentGridCoords == (FVector2D(17, 14))) && (UscitaGhost == true)) {
		this->UscitaGhost = false;
	}
}

void APhantomPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->GetTargetNode() == nullptr)
	{
		SetGhostTarget();
	}
}

APacmanPawn* APhantomPawn::GetPlayer() const
{
	return Player;
}

void APhantomPawn::SetSpeed(float Speed)
{
	CurrentMovementSpeed = Speed;
}

AGridBaseNode* APhantomPawn::GetPlayerRelativeTarget()
{
	return Player->GetLastNode();
}

void APhantomPawn::SetGhostTarget()
{
	//chase state allora insegue player, da togliere frightenedstate
	if ((this->IsChaseState() || this->IsFrightenedState()) && !UscitaGhost)
	{
		const AGridBaseNode* Target = GetPlayerRelativeTarget();
		if (!Target)
		{
			Target = GetPlayer()->GetLastNode();
		}

		AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));

		//const FVector Dimensions(60, 60, 20);
		//DrawDebugBox(GetWorld(), PossibleNode->GetTileCoordinates(), Dimensions, FColor::Red);

		if (PossibleNode)
		{
			this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
		}
	}
	else if (UscitaGhost)
	{
		const AGridBaseNode* Target1 = *(TheGridGen->TileMap.Find(FVector2D(17.0, 14.0)));

		AGridBaseNode* PossibleNode1 = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target1->GetGridPosition(), -(this->GetLastValidDirection()));

		if (PossibleNode1)
		{
			this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode1->GetGridPosition() - this->GetLastNodeCoords()), true);
		}

		if (CurrentGridCoords == FVector2D(17.0, 14.0))
		{
			UscitaGhost = false;
		}
	}
	else if (this->IsDeadState())
	{
		//override della casella home per ciascun ghost
		this->GoHome();
	}

	else if (UscitaGhost && (IsChaseState() || IsFrightenedState()))
	{
		this->ExitGhostArea();
	}
}

//in override in ciascun ghost
void APhantomPawn::RespawnGhostStartingPosition()
{
}

//in override in ciascun ghost
void APhantomPawn::GoHome()
{
}

void APhantomPawn::SetChaseState()
{
	//todo: change direction
	StaticMesh->SetMaterial(2, DefaultSkin);

	this->EEnemyState = Chase;
}

bool APhantomPawn::IsChaseState()
{
	if (this->EEnemyState == Chase) return true;

	else return false;
}

void APhantomPawn::SetFrightenedState()
{
	//todo: change direction
	StaticMesh->SetMaterial(2, VulnerableSkin);

	this->EEnemyState = Frightened;
}

bool APhantomPawn::IsFrightenedState()
{
	if (this->EEnemyState == Frightened) return true;

	else return false;
}

void APhantomPawn::SetDeadState()
{
	StaticMesh->SetMaterial(2, DeadSkin);

	this->EEnemyState = Dead;
}

bool APhantomPawn::IsDeadState()
{

	if (this->EEnemyState == Dead) return true;

	else return false;
}

void APhantomPawn::ExitGhostArea()
{
	const AGridBaseNode* Target1 = *(TheGridGen->TileMap.Find(FVector2D(15, 13)));

	AGridBaseNode* PossibleNode1 = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target1->GetGridPosition(), -(this->GetLastValidDirection()));

	if (PossibleNode1)
	{
		this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode1->GetGridPosition() - this->GetLastNodeCoords()), true);
	}

	if (CurrentGridCoords == FVector2D(15, 13))
	{
		//esci
		const AGridBaseNode* Target2 = *(TheGridGen->TileMap.Find(FVector2D(17, 13)));

		AGridBaseNode* PossibleNode2 = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target2->GetGridPosition(), -(this->GetLastValidDirection()));

		if (PossibleNode2)
		{
			this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode2->GetGridPosition() - this->GetLastNodeCoords()), true);
			this->UscitaGhost = false;
		}
	}
}

