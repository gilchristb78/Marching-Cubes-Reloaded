# Marching-Cubes-Reloaded
In this repository I will refactor and upgrade my marching cubes algorthm from [Voxels](https://github.com/gilchristb78/Voxel)


## Chunk World
This class will control all of the chunks and be the main gateway for function calls.

### Variables
- Draw Distance: An integer representing render distance.
- Chunk Size: An integer that represents the width and depth of chunks (height will be fixed).
- Voxel Size: an integer representing the size of our voxels in cm's. `using 100 will result in 1 meter between voxels`
- Chunks: A map that will pair FVector coordinates with chunks.
- Materials: two variables will represent solid materials and translucent materials (liquids). <br> More may be added in the future for seperate mesh sections.

### Functions
- BeginPlay: At the start of the game render chunks in a square around the world origin.
- Render Chunks: If the chunks exist (in the map) generate the mesh and apply the mesh. <br> Else spawn the chunks, set the values, and add the chunk to the map.
- Remove Voxels: In a 3x3x3 cube call the get chunk function with each coordinate. <br> save this chunk in a unique list. <br> call its remove function with localized x,y,z. <br> re-render all chunks in our unique list. <br>`note: must ensure chunks are rendered around player or this will break`
- Get Chunk: Given an FVector Coordinate, set the z coordinate to 0 (as thats where we spawn all chunks). <br> then divide Y and Z by chunkSize * VoxelSize <br> plug this value into our map giving you our chunk. `specifically will break here`
- Get Local Coordinates: Modulus by ChunkSize * VoxelSize giving you coordinates of voxels within a specific chunk.




## Todo:

- [ ] create Add / Remove Voxels
  - [ ] Different Blocks
- [ ] Biomes
- [ ] Caves
  - [ ] 3d perlin noise
  - [ ] snaking perlin noise
