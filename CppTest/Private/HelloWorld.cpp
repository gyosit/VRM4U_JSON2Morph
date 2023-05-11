// Fill out your copyright notice in the Description page of Project Settings.


#include "HelloWorld.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AHelloWorld::AHelloWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHelloWorld::BeginPlay()
{
	Super::BeginPlay();

	UKismetSystemLibrary::PrintString(this, "Hello World!");
}

// Called every frame
void AHelloWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

