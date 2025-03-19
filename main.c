#include <stdio.h>
#include "json.h"
#include "sim.h"

int main(int argc, char **argv)
{
    if(argc < 3)
        printf("Usage: %s <in-file.dat> <out-file.json>\r\n", argv[0]);
    
    SimConfig.selectionPolicy = SIM_DYNAMIC;
    SimConfig.timePolicy = SIM_TIME_PRIORITIZED;

    SimConfig.road[0].position = NORTH;
    SimConfig.road[0].laneCount = 1;
    SimConfig.road[0].lane[0].road = &SimConfig.road[0];
    SimConfig.road[0].lane[0].direction.south = 1;
    SimConfig.road[0].lane[0].direction.east = 1;
    SimConfig.road[0].lane[0].direction.west = 1;
    SimConfig.road[0].lane[0].minGreenTime = 1;
    SimConfig.road[0].lane[0].maxGreenTime = 10;
    SimConfig.road[0].lane[0].minRedTime = 1;
    SimConfig.road[0].lane[0].priority = 1.f;
    SimConfig.road[0].lane[0].permissive = false;

    SimConfig.road[1].position = SOUTH;
    SimConfig.road[1].laneCount = 1;
    SimConfig.road[1].lane[0].road = &SimConfig.road[1];
    SimConfig.road[1].lane[0].direction.north = 1;
    SimConfig.road[1].lane[0].direction.east = 1;
    SimConfig.road[1].lane[0].direction.west = 1;
    SimConfig.road[1].lane[0].minGreenTime = 1;
    SimConfig.road[1].lane[0].maxGreenTime = 10;
    SimConfig.road[1].lane[0].minRedTime = 1;
    SimConfig.road[1].lane[0].priority = 1.f;
    SimConfig.road[1].lane[0].permissive = false;

    SimConfig.road[2].position = WEST;
    SimConfig.road[2].laneCount = 1;
    SimConfig.road[2].lane[0].road = &SimConfig.road[2];
    SimConfig.road[2].lane[0].direction.south = 1;
    SimConfig.road[2].lane[0].direction.east = 1;
    SimConfig.road[2].lane[0].direction.north = 1;
    SimConfig.road[2].lane[0].minGreenTime = 1;
    SimConfig.road[2].lane[0].maxGreenTime = 10;
    SimConfig.road[2].lane[0].minRedTime = 1;
    SimConfig.road[2].lane[0].priority = 1.f;
    SimConfig.road[2].lane[0].permissive = false;

    SimConfig.road[3].position = EAST;
    SimConfig.road[3].laneCount = 1;
    SimConfig.road[3].lane[0].road = &SimConfig.road[3];
    SimConfig.road[3].lane[0].direction.south = 1;
    SimConfig.road[3].lane[0].direction.west = 1;
    SimConfig.road[3].lane[0].direction.north = 1;
    SimConfig.road[3].lane[0].minGreenTime = 1;
    SimConfig.road[3].lane[0].maxGreenTime = 10;
    SimConfig.road[3].lane[0].minRedTime = 1;
    SimConfig.road[3].lane[0].priority = 1.f;
    SimConfig.road[3].lane[0].permissive = false;

    JsonRunSimFromExternalData(argv[1], argv[2]);
}
