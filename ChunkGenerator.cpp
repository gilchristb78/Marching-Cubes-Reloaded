// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkGenerator.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChunkGenerator::AChunkGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AChunkGenerator::RenderChunk(FVector ChunkPos)
{
	
	if (chunks.Contains(ChunkPos))
	{
		chunks[ChunkPos]->RenderChunk();
		return;
	}
	
	FTransform transform = FTransform(FRotator::ZeroRotator, ChunkPos, FVector::OneVector);
	AChunk* chunk = GetWorld()->SpawnActorDeferred<AChunk>(AChunk::StaticClass(), transform, this);
	chunk->Frequency = Frequency;
	chunk->SurfaceLevel = SurfaceLevel;
	chunk->ChunkSize = ChunkSize;
	chunk->ChunkHeight = ChunkHeight;
	chunk->SeaLevel = SeaLevel;
	chunk->Material = MaterialGround;
	chunk->MaterialWater = MaterialWater;
	chunk->FractalOctaves = FractalOctaves;
	chunk->FractalGain = FractalGain;
	chunk->FractalLacunarity = FractalLacunarity;
	UGameplayStatics::FinishSpawningActor(chunk, transform);
	chunks.Add(ChunkPos, chunk);
	
}

// Called when the game starts or when spawned
void AChunkGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (int x = -DrawDistance; x <= DrawDistance; x++)
	{
		for (int y = -DrawDistance; y <= DrawDistance; y++)
		{
			RenderChunk(FVector(x * ChunkSize * VoxelSize, y * ChunkSize * VoxelSize, 0));
		}
	}
	
}


