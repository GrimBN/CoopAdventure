#pragma once

#include "CoreMinimal.h"
#include "Transporter.h"
#include "ObjectRotator.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COOPADVENTURE_API UObjectRotator : public UTransporter
{
	GENERATED_BODY()

public:

	UObjectRotator();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FRotator StartRotation;
	FRotator EndRotation;

	bool bAreRotationPointsSet;

public:

	UFUNCTION(BlueprintCallable)
	void SetRotationPoints(FRotator Point1, FRotator Point2);
};
