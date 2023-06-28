// Fill out your copyright notice in the Description page of Project Settings.


#include "Clyde.h"

AGridBaseNode* AClyde::GetPlayerRelativeTarget()
{
	return Super::GetPlayerRelativeTarget();
}

AClyde::AClyde()
{
	CurrentGridCoords = FVector2D(16, 9);
}

void AClyde::BeginPlay()
{
	Super::BeginPlay();

	this->SetChaseState();

	UscitaGhost = true;
}

void AClyde::RespawnGhostStartingPosition()
{
	Super::RespawnGhostStartingPosition();

	const FVector Location(1650, 950, GetActorLocation().Z);

	LastNode = (*(GridGenTMap.Find(FVector2D(16, 9))));
	SetNextNode(*(GridGenTMap.Find(FVector2D(16, 9))));
	SetTargetNode(NextNode);

	SetActorLocation(Location);

	this->SetChaseState();

	UscitaGhost = true;
}

void AClyde::GoHome() {
	this->SetDeadState();

	const AGridBaseNode* Target = *(GridGenTMap.Find(FVector2D(16, 9)));

	AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));

	if (PossibleNode)
	{
		this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
	}

	if (CurrentGridCoords == FVector2D(16, 9))
	{
		UscitaGhost = true;
		this->SetChaseState();
	}
}