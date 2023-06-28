// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinky.h"

AGridBaseNode* APinky::GetPlayerRelativeTarget()
{
	return Super::GetPlayerRelativeTarget();
}

APinky::APinky()
{
	CurrentGridCoords = FVector2D(20, 18);
}

void APinky::BeginPlay()
{
	Super::BeginPlay();

	this->SetChaseState();
	UscitaGhost = true;
}

void APinky::RespawnGhostStartingPosition()
{
	Super::RespawnGhostStartingPosition();

	const FVector Location(2050, 1850, GetActorLocation().Z);

	LastNode = (*(GridGenTMap.Find(FVector2D(20, 18))));
	SetNextNode(*(GridGenTMap.Find(FVector2D(20, 18))));
	SetTargetNode(NextNode);

	SetActorLocation(Location);
	UscitaGhost = true;
	this->SetChaseState();
}

void APinky::GoHome() {
	this->SetDeadState();

	//target node (17,14)

	const AGridBaseNode* Target = *(GridGenTMap.Find(FVector2D(20, 18)));

	AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));

	if (PossibleNode)
	{
		this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
	}

	if (CurrentGridCoords == FVector2D(20, 18))
	{
		UscitaGhost = true;
		this->SetChaseState();
	}
}