# DropletErosion

## Description

Lab project

## Goal

Create erosion simulation system with real time update of images.

## Implementation notes

TerrainGenerator - to create heightmap based on perlin noise
NormalMapGenerator - creates normal map from heightmap (for faster drops calculation)
RngService - has API to provide pseudo random streams of random numbers based provided parameters (such as coordinates, iteration) may be thread unsafe !!!
DropletGenerationService - TBD - generates droplets based on given parameters
ErosionService - TBD - performs iterative erosion operations

## Roadmap

* erosion system
* water pool system
* water streams system
* Make much more parallelizeable solution:
  * Test image layered solution instead of particles
  * threads:
     * Try to combine actions in chains to prevent multiple thread creation/destruction
     * Create thread pool
  * Move calculations to gpu (using shaders, vulkan, cuda etc)
