// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"
#include "Transporter.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	//SetReplicates(true);
	SetReplicateMovement(true);

	bActivated = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);


	//Don't like setting these valus through code, probably try commenting these and setting directly through blueprint
	/*auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3f, 3.3f, 0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	}*/

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	/*auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));

	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 0.5f));
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, 7.2f));
	}*/

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));
	Transporter->OwnerIsTriggerActor = true;
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));
	if (HasAuthority())
	{
		TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnPressurePlateBeginOverlap);
		TriggerMesh->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnPressurePlateEndOverlap);
	}
	
	FVector EndPoint = GetActorLocation() + FVector(0.f, 0.f, -10.f);
	Transporter->SetPoints(GetActorLocation(), EndPoint);
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void APressurePlate::OnPressurePlateBeginOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OverlappingActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "TriggerMesh Begin Overlap");
	if (!bActivated && OverlappingActor && OverlappingActor->ActorHasTag("TriggerActor"))
	{
		bActivated = true;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Activated");
		OnActivated.Broadcast();
	}
}

void APressurePlate::OnPressurePlateEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OverlappingActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "TriggerMesh End Overlap");

	TArray<AActor*> OverlappingActors;
	TriggerMesh->GetOverlappingActors(OverlappingActors);

	AActor* TriggerActor = nullptr;

	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		AActor* A = OverlappingActors[i];
		if (OverlappingActors[i]->ActorHasTag("TriggerActor"))
		{
			TriggerActor = A;
			break;
		}
	}

	if (bActivated && !TriggerActor)
	{
		bActivated = false;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, "Deactivated");
		OnDeactivated.Broadcast();
	}
}




