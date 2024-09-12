
#include "MovableActor.h"
#include "Components/ArrowComponent.h"
#include "Transporter.h"
#include "ObjectRotator.h"

AMovableActor::AMovableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);

	Point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint"));
	Point1->SetupAttachment(RootComp);
	Point1->SetRelativeLocation(FVector::Zero());

	Point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("EndPoint"));
	Point2->SetupAttachment(RootComp);
	Point2->SetRelativeLocation(FVector(0.f, 0.f, 300.0f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));

}

void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	FVector StartPoint = GetActorLocation() + Point1->GetRelativeLocation();

	FVector EndPoint = GetActorLocation() + Point2->GetRelativeLocation();

	Transporter->SetPoints(StartPoint, EndPoint);

	if (ObjectRotator)
	{
		FRotator StartRotation = GetActorRotation() + Point1->GetRelativeRotation();
		
		FRotator EndRotation = GetActorRotation() + Point2->GetRelativeRotation();


		ObjectRotator->SetRotationPoints(StartRotation, EndRotation);
	}
	
}

void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

