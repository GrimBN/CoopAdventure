// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKey.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"
#include "CoopAdventureCharacter.h"
#include "CollectableKeyHolder.h"

// Sets default values
ACollectableKey::ACollectableKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Capsule->SetCapsuleHalfHeight(150.f);
	Capsule->SetCapsuleRadius(100.f);
	Capsule->SetGenerateOverlapEvents(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

	RotationSpeed = 100.f;

}

// Called when the game starts or when spawned
void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACollectableKey::OnKeyBeginOverlap);
	}
}

// Called every frame
void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		Mesh->AddRelativeRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));
	}

}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey, bIsCollected);
}

void ACollectableKey::OnRep_IsCollected()
{
	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Display, TEXT("OnRep_IsCollected called from the server"));
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT("OnRep_IsCollected called from the client"));
	}

	Mesh->SetVisibility(!bIsCollected);

	if (bIsCollected && KeyHolderRef)
	{
		KeyHolderRef->ActivateKeyMesh();
		if (HasAuthority())
		{
			OnCollected.Broadcast();
		}
	}

	CollectAudio->Play();
}

void ACollectableKey::OnKeyBeginOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OverlappingActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlappingActor->GetName());

	if (Cast<ACoopAdventureCharacter>(OverlappingActor) && !bIsCollected)
	{
		bIsCollected = true;

		//This is required specifically to call the server's OnRep function since client ones are called automatically when bIsCollected changes
		//No need for HasAuthority check here as this Overlap check function is only called on the server because of the HasAuthority check in BeginPlay
		OnRep_IsCollected();
	}
}

