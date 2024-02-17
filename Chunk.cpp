// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "FastNoiseLite.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();


	Mesh->SetCastShadow(false);

	SetRootComponent(Mesh);

	Voxels.SetNum((ChunkHeight + 1) * (ChunkSize + 1) * (ChunkSize + 1));
}

void AChunk::GenerateVoxels()
{
	Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);
	Noise->SetFractalOctaves(FractalOctaves);
	Noise->SetFractalLacunarity(FractalLacunarity);
	Noise->SetFractalGain(FractalGain);

	FVector Position = GetActorLocation() / 100;

	for (int x = 0; x <= ChunkSize; x++)
	{
		for (int y = 0; y <= ChunkSize; y++)
		{
			const int Height = FMath::Clamp(
				FMath::RoundToInt(
					(Noise->GetNoise(x + Position.X, y + Position.Y) + 1)// 0 - 2
					* ChunkHeight / 2), //0 - ChunkHeight
				0, ChunkHeight); //ensure (clamp)

			for (int z = 0; z < Height; z++)
			{
				Voxels[GetVoxelIndex(x, y, z)] = EBlock::Stone;
			}

			for (int z = Height; z < SeaLevel; z++)
			{
				Voxels[GetVoxelIndex(x, y, z)] = EBlock::Water;
			}
		}
	}
}

void AChunk::ApplyMesh() const
{
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Normals,
		MeshData.UV0,
		MeshData.Colors,
		TArray<FProcMeshTangent>(),
		true);

	Mesh->SetMaterial(1, MaterialWater);
	Mesh->CreateMeshSection(1, MeshDataWater.Vertices,
		MeshDataWater.Triangles,
		MeshDataWater.Normals,
		MeshDataWater.UV0,
		MeshDataWater.Colors,
		TArray<FProcMeshTangent>(),
		false);
}

void AChunk::RenderChunk()
{
	GenerateMesh();

	ApplyMesh();
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

	GenerateVoxels();

	RenderChunk();
	
}

void AChunk::GenerateMesh()
{
	VertexCountWater = 0;	//reset our variables
	VertexCountGround = 0;
	MeshData = FChunkMeshData();
	MeshDataWater = FChunkMeshData();

	if (SurfaceLevel > 0.0f) //triangles face correct direction based on surface level
	{
		TriangleOrder[0] = 0;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 2;
	}
	else
	{
		TriangleOrder[0] = 2;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 0;
	}

	float GroundCube[8] = {};
	float WaterCube[8] = {};

	for (int x = 0; x < ChunkSize; x++)
	{
		for (int y = 0; y < ChunkSize; y++)
		{
			for (int z = 0; z < ChunkSize; z++)
			{
				for (int i = 0; i < 8; i++)
				{
					EBlock boxVector = Voxels[GetVoxelIndex(x + VertexOffset[i][0], y + VertexOffset[i][1], z + VertexOffset[i][2])];
					boxVector == EBlock::Stone ? GroundCube[i] = 1 : GroundCube[i] = 0;
					boxVector == EBlock::Water ? WaterCube[i] = 1 : WaterCube[i] = 0;
				}

				if (WaterCube[0] == 1 || WaterCube[1] == 1 || WaterCube[2] == 1 || WaterCube[3] == 1) //hack to make water a flat surface
				{
					WaterCube[0] = 1; WaterCube[1] = 1; WaterCube[2] = 1; WaterCube[3] = 1;
				}

				March(x, y, z, GroundCube, MeshData, VertexCountGround);
				March(x, y, z, WaterCube, MeshDataWater, VertexCountWater, true);
			}
		}
	}
}

int AChunk::GetVoxelIndex(int X, int Y, int Z) const
{
	return X * (ChunkSize + 1) * (ChunkHeight + 1) + Y * (ChunkHeight + 1) + Z;
}

void AChunk::March(int X, int Y, int Z, const float Cube[8], FChunkMeshData& data, int& VertexIncrementer, bool Water)
{
	int VertexMask = 0;
	for (int i = 0; i < 8; i++) //set our vertex mask
	{
		if (Cube[i] <= SurfaceLevel)
			VertexMask |= 1 << i;
	}
	
	const int EdgeMask = CubeEdgeFlags[VertexMask]; //where we should draw edges
	if (EdgeMask == 0) return; //no edges to draw, we are done

	FVector EdgeVertex[12];
	for (int i = 0; i < 12; i++)
	{
		if ((EdgeMask & 1 << i) != 0)
		{
			EdgeVertex[i].X = X + (VertexOffset[EdgeConnection[i][0]][0] + .5 * EdgeDirection[i][0]); //.5 is our "interpolation" value (not interpolating)
			EdgeVertex[i].Y = Y + (VertexOffset[EdgeConnection[i][0]][1] + .5 * EdgeDirection[i][1]);
			EdgeVertex[i].Z = Z + (VertexOffset[EdgeConnection[i][0]][2] + .5 * EdgeDirection[i][2]);
			if (Water)
				EdgeVertex[i].Z -= 0.1;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		if (TriangleConnectionTable[VertexMask][3 * i] < 0) break;

		FVector V1 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i]] * 100; //get a vertex of the triangle we should draw
		FVector V2 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 1]] * 100;
		FVector V3 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 2]] * 100;

		FVector Normal = FVector::CrossProduct(V2 - V1, V3 - V1);
		Normal.Normalize();

		data.Vertices.Add(V1);
		data.Vertices.Add(V2);
		data.Vertices.Add(V3);

		data.Triangles.Add(VertexIncrementer + TriangleOrder[0]);
		data.Triangles.Add(VertexIncrementer + TriangleOrder[1]);
		data.Triangles.Add(VertexIncrementer + TriangleOrder[2]);

		data.Normals.Add(Normal);
		data.Normals.Add(Normal);
		data.Normals.Add(Normal);

		if (Water)
		{
			data.Colors.Add(FColor(50, 100, 200));
			data.Colors.Add(FColor(50, 100, 200));
			data.Colors.Add(FColor(50, 100, 200));
		}
		else
		{
			data.Colors.Add(FColor(100, 100, 100));
			data.Colors.Add(FColor(100, 100, 100));
			data.Colors.Add(FColor(100, 100, 100));
		}

		VertexIncrementer += 3;
	}

}



