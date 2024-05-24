

#include "Transporter.h"
#include "PressurePlate.h"

// Sets default values for this component's properties
UTransporter::UTransporter()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MoveTime = 3.0f;
	ActivatedTriggerCount = 0;
	bArePointsSet = false;

	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();

}

// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* A : TriggerActors)
	{
		APressurePlate* Plate = Cast<APressurePlate>(A);
		if (Plate)
		{
			Plate->OnActivated.AddDynamic(this, &UTransporter::OnPressurePlateActivated);
			Plate->OnDeactivated.AddDynamic(this, &UTransporter::OnPressurePlateDeactivated);
		}
	}
	
}

// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();

	if (Owner && Owner->HasAuthority() && bArePointsSet)
	{
		FVector CurrentLocation = Owner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;

		FVector TargerLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;

		if (!CurrentLocation.Equals(TargerLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargerLocation, DeltaTime, Speed);
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

void UTransporter::OnPressurePlateActivated()
{
	ActivatedTriggerCount++;
	FString Msg = FString::Printf(TEXT("Transport Trigger Count: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, Msg);

	if (ActivatedTriggerCount >= TriggerActors.Num() && !AllTriggerActorsTriggered)
	{
		AllTriggerActorsTriggered = true;
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "AllTriggerActorsTriggered!");
	}
}

void UTransporter::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;
	FString Msg = FString::Printf(TEXT("Transport Trigger Count: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, Msg);

	if (ActivatedTriggerCount < TriggerActors.Num() && AllTriggerActorsTriggered)
	{
		AllTriggerActorsTriggered = false;
	}
}

