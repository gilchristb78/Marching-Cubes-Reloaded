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
	chunk->ZoomLevel = ZoomLevel;
	UGameplayStatics::FinishSpawningActor(chunk, transform);
	chunks.Add(ChunkPos, chunk);
	
}

void AChunkGenerator::RemoveVoxels(FVector TargetPos)
{
	TArray<AChunk*> ModifiedChunks = TArray<AChunk*>();
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int z = -1; z <= 1; z++)
			{
				for (AChunk* chunk : SetBlockTo(TargetPos + FVector(x * VoxelSize, y * VoxelSize, z * VoxelSize), EBlock::Air))
				{
					ModifiedChunks.AddUnique(chunk);
				}
			}
		}
	}
	for (AChunk* chunk : ModifiedChunks)
		chunk->RenderChunk();
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

TArray<AChunk*> AChunkGenerator::SetBlockTo(FVector WorldPos, EBlock blockType)
{
	TArray<AChunk*> ModifiedChunks = TArray<AChunk*>();
	FVector ChunkPos = GetChunkPos(WorldPos);
	FVector LocalPos = GetLocalPos(WorldPos) / VoxelSize;
	LocalPos.X = floor(LocalPos.X);
	LocalPos.Y = floor(LocalPos.Y);
	LocalPos.Z = floor(LocalPos.Z);

	if (LocalPos.X == 0)
	{
		FVector AdjacentChunk = FVector(ChunkPos.X - (VoxelSize * ChunkSize), ChunkPos.Y, 0);
		if (chunks.Contains(AdjacentChunk))
		{
			ModifiedChunks.Add(chunks[AdjacentChunk]);
			chunks[AdjacentChunk]->SetVoxelTo(FVector(ChunkSize, LocalPos.Y, LocalPos.Z), blockType);
		}
	}
	if (LocalPos.Y == 0)
	{
		FVector AdjacentChunk = FVector(ChunkPos.X , ChunkPos.Y - (VoxelSize * ChunkSize), 0);
		if (chunks.Contains(AdjacentChunk))
		{
			ModifiedChunks.Add(chunks[AdjacentChunk]);
			chunks[AdjacentChunk]->SetVoxelTo(FVector(LocalPos.X, ChunkSize, LocalPos.Z), blockType);
		}
	}

	if (LocalPos.X == 0 && LocalPos.Y == 0)
	{
		FVector AdjacentChunk = FVector(ChunkPos.X - (VoxelSize * ChunkSize), ChunkPos.Y - (VoxelSize * ChunkSize), 0);
		if (chunks.Contains(AdjacentChunk))
		{
			ModifiedChunks.Add(chunks[AdjacentChunk]);
			chunks[AdjacentChunk]->SetVoxelTo(FVector(ChunkSize, ChunkSize, LocalPos.Z), blockType);
		}
	}


	if (chunks.Contains(ChunkPos))
	{
		ModifiedChunks.Add(chunks[ChunkPos]);
		chunks[ChunkPos]->SetVoxelTo(LocalPos, blockType);
	}

	return ModifiedChunks;
}

FVector AChunkGenerator::GetLocalPos(FVector WorldPos)
{
	return WorldPos - GetChunkPos(WorldPos);
}

FVector AChunkGenerator::GetChunkPos(FVector WorldPos)
{
	FVector ChunkPos = FVector(WorldPos.X, WorldPos.Y, WorldPos.Z);
	ChunkPos.X = floor(ChunkPos.X / (VoxelSize * ChunkSize));
	ChunkPos.Y = floor(ChunkPos.Y / (VoxelSize * ChunkSize));
	ChunkPos.Z = 0;
	ChunkPos *= (VoxelSize * ChunkSize);
	return ChunkPos;
}

