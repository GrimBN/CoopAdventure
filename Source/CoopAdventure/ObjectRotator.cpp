


#include "ObjectRotator.h"

//TODO: Find out if this component can be added dynamically to specific movable actors
UObjectRotator::UObjectRotator()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	bAreRotationPointsSet = false;

	StartRotation = FRotator::ZeroRotator;
	EndRotation = FRotator::ZeroRotator;
}

void UObjectRotator::BeginPlay()
{
	Super::BeginPlay();
}

void UObjectRotator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction); //Does this work?

	AActor* Owner = GetOwner();

	if (Owner && Owner->HasAuthority() && bAreRotationPointsSet)
	{
		FQuat CurrentRotation = Owner->GetActorRotation().Quaternion();
		float angularVeloctiy = EndRotation.Quaternion().AngularDistance(StartRotation.Quaternion()) / MoveTime;
		//UE_LOG(LogTemp, Warning, TEXT("%f"), angularVeloctiy);

		FQuat TargetRotation = AllTriggerActorsTriggered ? EndRotation.Quaternion() : StartRotation.Quaternion();

		if (!CurrentRotation.Equals(TargetRotation))
		{
			FQuat NewRotation = FMath::QInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, angularVeloctiy);
			Owner->SetActorRotation(NewRotation);
		}
	}
}


void UObjectRotator::SetRotationPoints(FRotator Point1, FRotator Point2)
{
	StartRotation = Point1;
	EndRotation = Point2;

	bAreRotationPointsSet = true;
}

