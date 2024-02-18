# Marching-Cubes-Reloaded
In this repository I will refactor and upgrade my marching cubes algorthm from [Voxels](https://github.com/gilchristb78/Voxel). <br> this created a highly modifiable generator that can dynamically create different worlds. <br>
![Final Result](https://github.com/gilchristb78/Marching-Cubes-Reloaded/blob/main/marchingcapture/CaptureMarchingFinal..PNG)
> this world was created with the settings (drawDistance=8, ChunkSize=32, ChunkHeight=256, VoxelSize=20, SeaLevel=120, Frequency=0.02, FractalOctaves=3, FractalLacunarity=2.5, FractalGain=0.4, ZoomLevel=5)


## Chunk World
This class will control all of the chunks and be the main gateway for function calls.

### Functions
- BeginPlay: At the start of the game render chunks in a square around the world origin.
- Render Chunk: If the chunk exists (in the map) generate the mesh and apply the mesh. <br> Else spawn the chunk, set the values, and add the chunk to the map.
- Remove Voxels 3x3x3: In a 3x3x3 cube call the get chunk function with each coordinate. <br> save this chunk in a unique list. <br> call its remove function with localized x,y,z. <br> re-render all chunks in our unique list. <br>`note: must ensure chunks are rendered around player or this will break`
- Add Voxels 3x3x3: In the same manar as remove voxels, except this time set the blocks to stone rather than air.
- Set Block To: set a voxel at a specific location (making use of locations from UE) to a specific block. <br> Note we must take in to consider chunk borders. <br> because of how the mesh is created for the marchng cubes algorithm we must remember that the same voxels are stored in two seperate chunks on chunk borders. <br> To accomidate this we floor our values so we only have to worry about overlapping on two sides. <br> Then we check if we are adjacent to a side (or corner) and update the adjacent chunks accordingly.
- Get Chunk Pos: Given an FVector Coordinate, set the z coordinate to 0 (as thats where we spawn all chunks). <br> then divide Y and Z by chunkSize * VoxelSize. <br> This will give us the location that chunks are spawned at and can be used within our map.
- Get Local Coordinates: Take our World Position and subtract the location of the chunk we are in resulting in a localize coordinate relative to its chunk.

## Chunk
this class will control an individual chunk, focusing on an area of size ChunkSize * ChunkSize * ChunkHeight (with reference to voxelSize).

### Functions
- BeginPlay: At the start of our game use all of the values passed through by the chunk generator to set various values. <br> Then generate our voxel map and render the chunk.
- Generate Voxels: for every x and y value we have: set a surface height based on a 2D perlin noise map, Work our way up from z level 0, use a 3d noise map to get caves, if we are within a cave set the block to air, otherwise set the block to stone, then when we get to the surface level add some sand if we are planning on adding water nearbly, finally go from our surfacelevel up to our sealevel adding water.
- Generate Mesh: march our cube around the voxels adding values to our mesh where applicable. For a more indepth explanation see [Voxels](https://github.com/gilchristb78/Voxel).
- Render Chunk: Regenerate our mesh and apply any changes to our mesh.
- Set Voxel To: a basic helper functioj that changes a voxels type
- get voxel index: a helper function that converts 3d coordinates into a 1d index.

## Todo:

- [X] add readme for chunk
- [ ] figure out what surface level does (swapped which side is "up" will generate the mesh uside down)
- [X] Voxel Size
- [X] create Add / Remove Voxels
  - [ ] Different Blocks
  - [X] Add Voxels
  - [X] Remove Voxels
- [ ] Biomes
- [ ] Caves
  - [X] 3d perlin noise
  - [ ] snaking perlin noise
