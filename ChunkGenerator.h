// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.h"
#include "ChunkGenerator.generated.h"

UCLASS()
class VOXEL_API AChunkGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkGenerator();

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int DrawDistance = 5;



	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int ChunkSize = 32;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int ChunkHeight = 32;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int VoxelSize = 100; //TODO

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int SeaLevel = 30;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	float Frequency = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	float SurfaceLevel = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	int FractalOctaves = 3;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	float FractalLacunarity = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Chunk Gen")
	float FractalGain = 0.5f;

	TMap<FVector, AChunk*> chunks;

	UPROPERTY(EditInstanceOnly, Category = "Chunk Gen")
	TObjectPtr<UMaterialInterface> MaterialGround;

	UPROPERTY(EditInstanceOnly, Category = "Chunk Gen")
	TObjectPtr<UMaterialInterface> MaterialWater;

	void RenderChunk(FVector ChunkPos);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	float Frequency = 0.01f;
	float SurfaceLevel = 0.0f;

	int ChunkSize = 32;
	int ChunkHeight = 32;
	int SeaLevel = 30;

	int FractalOctaves = 3;
	float FractalLacunarity = 2.0f;
	float FractalGain = 0.5f;


	*/

};
