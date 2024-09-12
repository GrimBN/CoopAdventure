


#include "ObjectRotator.h"

//TODO: Find out if this component can be added dynamically to specific movable actors
UObjectRotator::UObjectRotator()
{
	bAreRotationPointsSet = false;

	StartRotation = FRotator::ZeroRotator;
	EndRotation = FRotator::ZeroRotator;
}

void UObjectRotator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction); //Does this work?

	AActor* Owner = GetOwner();

	if (Owner && Owner->HasAuthority() && bAreRotationPointsSet)
	{
		FRotator CurrentRotation = Owner->GetActorRotation();
		float angularVeloctiy = EndRotation.GetManhattanDistance(StartRotation) / MoveTime;

		FRotator TargetRotation = AllTriggerActorsTriggered ? EndRotation : StartRotation;

		if (!CurrentRotation.Equals(TargetRotation))
		{
			FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, angularVeloctiy);
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

