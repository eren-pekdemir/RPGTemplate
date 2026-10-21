// RPGReputationHolder.cpp

#include "Interfaces/RPGReputationHolder.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "RPGCoreStatics.h"

IRPGReputationHolder* IRPGReputationHolder::Find(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	if (IRPGReputationHolder* Direct = URPGCoreStatics::FindInterface<IRPGReputationHolder, URPGReputationHolder>(Actor))
	{
		return Direct;
	}

	APlayerState* PlayerState = nullptr;
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		PlayerState = Pawn->GetPlayerState();
	}
	else if (const AController* Controller = Cast<AController>(Actor))
	{
		PlayerState = Controller->PlayerState;
	}
	else if (const APawn* InstigatorPawn = Actor->GetInstigator())
	{
		PlayerState = InstigatorPawn->GetPlayerState();
	}

	return URPGCoreStatics::FindInterface<IRPGReputationHolder, URPGReputationHolder>(PlayerState);
}
