// Fill out your copyright notice in the Description page of Project Settings.


#include "Inky.h"

AGridBaseNode* AInky::GetPlayerRelativeTarget()
{
	return Super::GetPlayerRelativeTarget();
}

AInky::AInky()
{
	CurrentGridCoords = FVector2D(11, 9);
}

void AInky::BeginPlay()
{
	Super::BeginPlay();

	this->SetChaseState();

	UscitaGhost = true;

	ExitGhostArea();
}

void AInky::RespawnGhostStartingPosition()
{
	Super::RespawnGhostStartingPosition();

	const FVector Location(1150, 950, GetActorLocation().Z);

	LastNode = (*(GridGenTMap.Find(FVector2D(11, 9))));
	SetNextNode(*(GridGenTMap.Find(FVector2D(11, 9))));
	SetTargetNode(NextNode);

	SetActorLocation(Location);

	this->SetChaseState();

	UscitaGhost = true;
}

void AInky::GoHome() {
	//this->SetDeadState();

	const AGridBaseNode* Target = *(GridGenTMap.Find(FVector2D(11, 9)));

	AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));

	//const FVector Dimensions(60, 60, 20);
	//DrawDebugBox(GetWorld(), PossibleNode->GetTileCoordinates(), Dimensions, FColor::Red);

	if (PossibleNode)
	{
		this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
	}

	if (CurrentGridCoords == FVector2D(11, 9))
	{
		UscitaGhost = true;
		this->SetChaseState();
	}
}