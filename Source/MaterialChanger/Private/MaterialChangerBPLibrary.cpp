// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialChangerBPLibrary.h"
#include "MaterialChanger.h"
#include "SceneTypes.h"
#include "RHIDefinitions.h"
#include "TimerManager.h"


UMaterialChangerBPLibrary::UMaterialChangerBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UMaterialChangerBPLibrary::MaterialChangerSampleFunction(const UObject* WorldContextObject, UStaticMeshComponent* Mesh, UMaterialInstance* StartMaterialInstance, UMaterialInstance* EndMaterialInstance,
	FVector FringeColor, float Power, float Glow, float TimePeriod, FVector Direction)
{
	// 1, Create a new Changer Material Instance
	// 2, Set Parameter for this new Changer Material Instance
	// 3, Set new Changer Material Instance to mesh
	// 4, Delay X seconds
	// 5, Set End Material to mesh
	UMaterialInstance* ChangerMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/MaterialChanger/M_MaterialChanger_Inst.M_MaterialChanger_Inst"));
	if (ChangerMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Material Changer Found"));
		UMaterialInstanceDynamic* DynChangerMaterial = UMaterialInstanceDynamic::Create(ChangerMaterial, nullptr);
		DynChangerMaterial->SetScalarParameterValue("Glow", Glow);
		DynChangerMaterial->SetScalarParameterValue("Power", Power);
		DynChangerMaterial->SetScalarParameterValue("TimePeriod", TimePeriod);
		DynChangerMaterial->SetVectorParameterValue("FringeColor", FringeColor);
		DynChangerMaterial->SetVectorParameterValue("Direction", Direction);

		TArray<UTexture*> OutStartTextures;
		StartMaterialInstance->GetUsedTextures(OutStartTextures, 
			EMaterialQualityLevel::High,
			true,
			ERHIFeatureLevel::Num,
			true
			);
		if (OutStartTextures.Num() > 0)
		{
			DynChangerMaterial->SetTextureParameterValue("StartTexture", OutStartTextures[0]);
		}
		
		TArray<UTexture*> OutEndTextures;
		EndMaterialInstance->GetUsedTextures(OutEndTextures,
			EMaterialQualityLevel::High,
			true,
			ERHIFeatureLevel::Num,
			true
		);
		if (OutEndTextures.Num() > 0)
		{
			DynChangerMaterial->SetTextureParameterValue("EndTexture", OutEndTextures[0]);
		}

		Mesh->SetMaterial(0, DynChangerMaterial);
		
		if (GEngine == nullptr) return;
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World != nullptr)
		{
			FTimerHandle TimerHandle;
			/*FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(World, FName("SetEndMaterial"), Mesh, EndMaterialInstance);*/
			World->GetTimerManager().SetTimer(TimerHandle, 
				[Mesh, EndMaterialInstance]() {
					Mesh->SetMaterial(0, EndMaterialInstance);
				},
				TimePeriod,
				false
				);
		}
	}
	return;
}

/*
void UMaterialChangerBPLibrary::SetEndMaterial(UStaticMesh* Mesh, UMaterialInstance* EndMaterialInstance)
{
	Mesh->SetMaterial(0, EndMaterialInstance);
	UE_LOG(LogTemp, Warning, TEXT("Material Changer Done"));
}
*/
