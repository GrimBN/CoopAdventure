
#include "WinArea.h"
#include "Components/BoxComponent.h"
#include "CoopAdventureCharacter.h"

AWinArea::AWinArea()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	WinAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinAreaBox"));
	SetRootComponent(WinAreaBox);

}

void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bWinCondition)
	{
		TArray<AActor*> OverlappingActors;
		WinAreaBox->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

		bWinCondition = OverlappingActors.Num() == 2;
		if (bWinCondition)
		{
			//UE_LOG(LogTemp, Display, TEXT("Win!"));

			MulticastRPCWin();
		}
	}

}

void AWinArea::MulticastRPCWin_Implementation()
{
	OnWinCondition.Broadcast();
}

