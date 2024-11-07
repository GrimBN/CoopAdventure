

#include "Transporter.h"
#include "PressurePlate.h"
#include "CollectableKey.h"

UTransporter::UTransporter()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MoveTime = 3.0f;
	ActivatedTriggerCount = 0;
	bArePointsSet = false;
	bMovementAllowed = true;

	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();

}

void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	//TriggerActors here are different to the ones for the pressure plate, its just a general term to refer to Actors that can/will trigger the respective component/custom actor class
	for (AActor* A : TriggerActors)
	{
		APressurePlate* Plate = Cast<APressurePlate>(A);
		AActor* Owner = GetOwner();

		//Added a HasAuthority check because I thought I missed it but I didn't and nothing seems to have changed so leaving it as is
		//Update: The reason nothing changed is because the pressure plates only check for overlap, and as a result broadcast on overlap, on the server and not on the client
		//So even though the client transporter had also subscribed to the delegates they never receive the broadcast from the client pressure plate because they never make one
		if (Owner && Owner->HasAuthority() && Plate)
		{
			Plate->OnActivated.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			Plate->OnDeactivated.AddDynamic(this, &UTransporter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* Key = Cast<ACollectableKey>(A);

		if (Owner && Owner->HasAuthority() && Key)
		{
			Key->OnCollected.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
		}
	}
	
}

// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();

	if (Owner && Owner->HasAuthority() && bArePointsSet && bMovementAllowed)
	{
		FVector CurrentLocation = Owner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;

		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;

		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			Owner->SetActorLocation(NewLocation);
		}
	}
}

void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if(Point1.Equals(Point2)) return;

	StartPoint = Point1;
	EndPoint = Point2;

	bArePointsSet = true;
}

void UTransporter::OnTriggerActorActivated()
{
	ActivatedTriggerCount++;

	if (ActivatedTriggerCount >= TriggerActors.Num() && !AllTriggerActorsTriggered)
	{
		AllTriggerActorsTriggered = true;
	}
}

void UTransporter::OnTriggerActorDeactivated()
{
	ActivatedTriggerCount--;

	if (ActivatedTriggerCount < TriggerActors.Num() && AllTriggerActorsTriggered)
	{
		AllTriggerActorsTriggered = false;
	}
}

