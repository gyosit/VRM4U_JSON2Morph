// Fill out your copyright notice in the Description page of Project Settings.


#include "BluePrintTestEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSequenceHelpers.h"
#include "AssetRegistryModule.h"
#include "AssetData.h"
#include "AssetRegistryHelpers.h"
#include "Animation/AnimCurveTypes.h" 
#include "AnimationBlueprintLibrary.h"
#include "AnimDataController.h"
#include "Misc/FileHelper.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"

static const FString RootName("InventoryObject");
static const FString WritePath(FPaths::ProjectSavedDir() / "Json");
static const FString FileName("anim.json");
// static const FString FilePathFull("D:\\EpicGames_projects\\UE_5.0\\Projects\\CppTest\\Content\\Json\\anim.json");

bool UBluePrintTestEditor::ReadJson(JsonObjectPtr &JsonRootObject, FString FilePathFull)
{
    // Get Json Data
    // Json Format
    // {"curves":[{"property_name":string,"keys":[{"time":float,"value":float}, ... ]}, ... ]}
    FString RawData;

    bool bLoadedFile = FFileHelper::LoadFileToString(RawData, *FilePathFull);

    if (bLoadedFile)
    {
        // FJsonObject(Jsonデータの入れ物)を作成
        JsonRootObject = MakeShareable(new FJsonObject);

        // FStringからJsonを読み込むためのReaderを作成
        TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RawData);

        // Json文字列からJsonオブジェクトに読み込み
        if (FJsonSerializer::Deserialize(JsonReader, JsonRootObject)) return true;
    }
    return false;   
}

void UBluePrintTestEditor::Setup()
{
    UE_LOG(LogTemp, Warning, TEXT("Hello World!") );
}

void UBluePrintTestEditor::CreateAnimation(FString jsonPath, FString animName)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Creation") );

    // Save the animation sequence to the content folder
	FString PackagePath = TEXT("/Game/Animations/");
	FString AbsolutePackagePath = FPaths::ProjectContentDir() + TEXT("/Animations/");
	// Get the path to the mesh asset
	FString AssetPath = TEXT("SkeletalMesh'/Game/Models/MeikaHimeVroidbase5_uni_2.SK_MeikaHimeVroidbase5_uni_2'");
    USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *AssetPath);
    // Get the skeleton associated with the mesh
    USkeleton* Skeleton = Mesh->GetSkeleton();

	FPackageName::RegisterMountPoint(PackagePath, AbsolutePackagePath);
	PackagePath += TEXT("SampleAnimSeq");
    UPackage* Package = CreatePackage(*PackagePath);
    Package->FullyLoad();
	Package->SetDirtyFlag(true);
	
	// Create a new animation sequence object
	UAnimSequence* AnimSeq = NewObject<UAnimSequence>(Package, UAnimSequence::StaticClass(), *animName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

    FAssetRegistryModule::AssetCreated(AnimSeq);

	// float AnimLength = 100.0f;
	AnimSeq->SetSkeleton(Skeleton);
    // Add the curve to the animation sequence
    // AnimSeq->AddKeyToSequence(0, TEXT("Fcl_MTH_A"), FclMthCurve->transform);
    // UAnimationBlueprintLibrary::AddCurve(AnimSeq, TEXT("Fcl_MTH_A"));
    // for(int i = 0; i < 10; i++)
    // {
    //     UAnimationBlueprintLibrary::AddFloatCurveKey(AnimSeq, TEXT("Fcl_MTH_A"), i*5, i);
    // }

    JsonObjectPtr JsonRootObject;
    if (!ReadJson(JsonRootObject, jsonPath)) return;
    // Get curves
    float end_time = 0.0f;
    int freq = 10;
    TArray<TSharedPtr<FJsonValue>> JsonCurves = JsonRootObject->GetArrayField("curves");
    for (TSharedPtr<FJsonValue> JsonCurve : JsonCurves)
    {
        int count = 0;
        // Get curve name
        FString propertyName = JsonCurve->AsObject()->GetStringField("property_name");
        UAnimationBlueprintLibrary::AddCurve(AnimSeq, FName(*propertyName));
        UE_LOG(LogTemp, Warning, TEXT("propertyName: %s"), *propertyName );

        // Get keys
        TArray<TSharedPtr<FJsonValue>> JsonKeys = JsonCurve->AsObject()->GetArrayField("keys");
        for (TSharedPtr<FJsonValue> JsonKey : JsonKeys)
        {
            count++;
            if(count % freq != 0) continue;
            // Get key time
            float KeyTime = JsonKey->AsObject()->GetNumberField("time");

            // Get key value
            float KeyValue = JsonKey->AsObject()->GetNumberField("value") / 100.0f;

            // Add key to animation sequence
            UAnimationBlueprintLibrary::AddFloatCurveKey(AnimSeq, FName(*propertyName), KeyTime, KeyValue);

            if(end_time < KeyTime) end_time = KeyTime;
        }
    }

    // Set animation length
    IAnimationDataController& controller = AnimSeq->GetController();
    UE_LOG(LogTemp, Warning, TEXT("end_time: %f"), end_time );
    controller.SetPlayLength(end_time, true);

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, AnimSeq, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	//FGlobalComponentReregisterContext ReregisterContext;

	UE_LOG(LogTemp, Warning, TEXT("Creation success!") );
}