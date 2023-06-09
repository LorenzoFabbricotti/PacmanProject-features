// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGridGameMode.h"
#include "GridPlayerController.h"
#include "GridPawn.h"
#include "PacmanPawn.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.h"
#include "GameWinWidget.h"

ATestGridGameMode::ATestGridGameMode()
{
	PlayerControllerClass = AGridPlayerController::StaticClass();
	DefaultPawnClass = APacmanPawn::StaticClass();
}

ATestGridGameMode::~ATestGridGameMode() {
}


void ATestGridGameMode::BeginPlay()
{
	Super::BeginPlay();

	IsGameOver = false;

	IsLevelWin = false;
	
	// con questa posizione la tile di coordinate griglia (0,0) ha come
	// coordinate nello spazio dello spigolo inferiore sinistro (0,0) 
	// 
	// es. coordinate nello spazio della tile di coordinate di griglia (0, 0) 
	// 
	// (100,0) -------------- (100,100)
    //         |            |
	//		   |            |
	//         |  (50, 50)  |
	//         |            |
	//         |            |
	//  (0,0)  -------------- (0, 100)
	//

	FVector GridPos(50.0f, 50.0f, 0.0f);
	// N.B. tutte le funzioni andrebbero normalizzate se la posizione
	// della griglia non fosse pi� questa ma una qualsiasi nello spazio tridimensionale
	if (GridGeneratorClass != nullptr)
	{
		// spawn della griglia
		GField = GetWorld()->SpawnActor<AGridGenerator>(GridGeneratorClass, GridPos, FRotationMatrix::MakeFromX(FVector(0, 0, 0)).Rotator());
	};

	//PacmanPawn =  GetWorld()->SpawnActor<APacmanPawn>(PacmanClass, FVector((500) + 50, (900) + 50, 5.0f), FRotator(0, 0, 0));

	BlinkyPawn = GetWorld()->SpawnActor<ABlinky>(BlinkyClass, FVector((100 * 17) + 50, (100 * 10) + 50, 5.0f), FRotator(0, 0, 0));
	InkyPawn = GetWorld()->SpawnActor<AInky>(InkyClass, FVector((100 * 11) + 50, (100 * 9) + 50, 5.0f), FRotator(0, 0, 0));
	PinkyPawn = GetWorld()->SpawnActor<APinky>(PinkyClass, FVector((100 * 20) + 50, (100 * 18) + 50, 5.0f), FRotator(0, 0, 0));
	ClydePawn = GetWorld()->SpawnActor<AClyde>(ClydeClass, FVector((100 * 16) + 50, (100 * 9) + 50, 5.0f), FRotator(0, 0, 0));
}


void ATestGridGameMode::RespawnPositions()
{
	//BlinkyPawn = Cast<ABlinky>(UGameplayStatics::GetActorOfClass(GetWorld(), ABlinky::StaticClass()));
	if (IsValid(BlinkyPawn))
	{
		BlinkyPawn->RespawnGhostStartingPosition();
	}

	//InkyPawn = Cast<AInky>(UGameplayStatics::GetActorOfClass(GetWorld(), AInky::StaticClass()));
	InkyPawn->RespawnGhostStartingPosition();

	PinkyPawn->RespawnGhostStartingPosition();

	ClydePawn->RespawnGhostStartingPosition();

	PacmanPawn = Cast<APacmanPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), APacmanPawn::StaticClass()));
	PacmanPawn->RespawnStartingPosition();

	//PacmanPawn =  GetWorld()->SpawnActor<APacmanPawn>(PacmanClass, FVector((500) + 50, (900) + 50, 5.0f), FRotator(0, 0, 0));	
}

void ATestGridGameMode::GameOver()
{
	IsGameOver = true;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("GAME OVER! YOU ARE DEAD!!!")));

	if (IsValid(BlinkyPawn))
	{

		BlinkyPawn->Destroy();
	}
	if (IsValid(InkyPawn)) {

		InkyPawn->Destroy();
	}
	if (IsValid(PinkyPawn)) {

		PinkyPawn->Destroy();
	}

	if (IsValid(ClydePawn)) {

		ClydePawn->Destroy();
	}

	PacmanPawn = Cast<APacmanPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), APacmanPawn::StaticClass()));
	if (IsValid(PacmanPawn)) {

		PacmanPawn->Destroy();
	}


	if (IsValid(GameOverWidgetsClass))
	{
		GameOverWidgets = Cast<UGameOverWidget>(CreateWidget(GetWorld(), GameOverWidgetsClass));

		if (GameOverWidgets != nullptr)
		{
			// Mostra il widget sulla viewport
			GameOverWidgets->AddToViewport();
		}
	}
}

void ATestGridGameMode::GameWin()
{
	if (IsValid(BlinkyPawn))
	{

		BlinkyPawn->Destroy();
	}
	if (IsValid(InkyPawn)) {

		InkyPawn->Destroy();
	}
	if (IsValid(PinkyPawn)) {

		PinkyPawn->Destroy();
	}

	if (IsValid(ClydePawn)) {

		ClydePawn->Destroy();
	}

	PacmanPawn = Cast<APacmanPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), APacmanPawn::StaticClass()));
	if (IsValid(PacmanPawn)) {

		PacmanPawn->Destroy();
	}


	if (IsValid(GameWinWidgetsClass))
	{
		GameWinWidgets = Cast<UGameWinWidget>(CreateWidget(GetWorld(), GameWinWidgetsClass));

		if (GameWinWidgets != nullptr)
		{
			// Mostra il widget sulla viewport
			GameWinWidgets->AddToViewport();
		}
	}
}

void ATestGridGameMode::SetLevelWin()
{
	this->RespawnPositions();

	TMap<FVector2D, AEatableEntity*> EatableMap = GField->GetEatableEntityMap();

	for (auto It = EatableMap.CreateIterator(); It; ++It)
	{	
		It->Value->setNotEaten();
		It->Value->SetActorHiddenInGame(false);
	}
}