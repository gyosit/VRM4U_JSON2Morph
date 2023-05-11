// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BluePrintTestEditor.generated.h"

/**
 * 
 */
typedef TSharedPtr<FJsonObject> JsonObjectPtr;

UCLASS()
class BAKEMORPHEDITOR_API UBluePrintTestEditor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static bool ReadJson(JsonObjectPtr &JsonRootObject, FString FilePathFull);

public:
	UFUNCTION(BlueprintCallable, Category = "BluePrintTestEditor")
	static void Setup();
	UFUNCTION(BlueprintCallable, Category = "BluePrintTestEditor")
	static void CreateAnimation(FString jsonPath, FString animName);
};
