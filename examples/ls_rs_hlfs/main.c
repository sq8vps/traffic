#include <stdio.h>
#include "sim.h"

/*
This example demonstrates a simple, more or less realistic intersection with two lanes on each road.

The left lane allows for going straigth and turning left, while the right lane allows for going straight
and turning right. The lanes are all permissive, meaning that the colliding flows are allowed (which is the
full green light and not a green arrow on Polish roads - this is of course not the conditional right turn arrow!).

The simulation policy is set to highest load first served, with light timing proportional to load on a given lane.

There are 8 vehicles:
* V0 goes from north to west (right)
* V1 goes from north to south
* V2 goes from north to east (left)
* V3 goes from north to south
* V4 goes from north to south
* V5 goes from south to north
* V6 goes from south to north
* V7 goes from south to west (left)
defined in added in this order.

Vehicles are assigned to the best lane (highest priority/load ratio) automatically.
Say lane A is the straight+right turn lane, and lane B is the straight+left turn lane. Then:
* V0 is at north lane A (since it is a right turn)
* V1 is at north lane B (since it is less loaded)
* V2 is at north lane B (since it is a left turn)
* V3 is at north lane A (since it is less loaded)
* V4 is at north lane A (since both lanes are equally loaded and lane A has smaller internal index)
* V5 is at south lane A (since lane A has smaller internal index)
* V6 is at south lane B (less loaded)
* V7 is at lane B (left turn)

The simulation is expected to be as follows:
1. All north and south lanes turn green, because they are permissive
2. V0 lefts (immediate right turn with no collision), V1, V5 and V6 left (straight north/south)
3. V3 lefts (straight) - because it was waiting in line
4. V4 lefts (straight) - again, it was waiting in line
5. V2 and V7 left - because they are left turns and needed to wait. This is simulteneous, because there is no collision.

*/

int main(int argc, char **argv)
{
    SimConfig.selectionPolicy = SIM_HLFS;
    SimConfig.timePolicy = SIM_TIME_PROPORTIONAL;

    SimConfig.road[0].position = NORTH;
    SimConfig.road[0].laneCount = 2;
    //from north to west and south (straight and right)
    SimConfig.road[0].lane[0].road = &SimConfig.road[0];
    SimConfig.road[0].lane[0].direction.south = 1;
    SimConfig.road[0].lane[0].direction.east = 0;
    SimConfig.road[0].lane[0].direction.west = 1;
    SimConfig.road[0].lane[0].minGreenTime = 1;
    SimConfig.road[0].lane[0].maxGreenTime = 10;
    SimConfig.road[0].lane[0].minRedTime = 1;
    SimConfig.road[0].lane[0].stepsPerVehicle = 2;
    SimConfig.road[0].lane[0].permissive = true;
    SimConfig.road[0].lane[0].priority = 1.f;
    //from north to east and south (straight and left)
    SimConfig.road[0].lane[1].road = &SimConfig.road[0];
    SimConfig.road[0].lane[1].direction.south = 1;
    SimConfig.road[0].lane[1].direction.east = 1;
    SimConfig.road[0].lane[1].direction.west = 0;
    SimConfig.road[0].lane[1].minGreenTime = 1;
    SimConfig.road[0].lane[1].maxGreenTime = 10;
    SimConfig.road[0].lane[1].minRedTime = 1;
    SimConfig.road[0].lane[1].stepsPerVehicle = 2;
    SimConfig.road[0].lane[1].permissive = true;
    SimConfig.road[0].lane[1].priority = 1.f;

    SimConfig.road[1].position = SOUTH;
    SimConfig.road[1].laneCount = 2;
    //from south to north and east (straight and right)
    SimConfig.road[1].lane[0].road = &SimConfig.road[1];
    SimConfig.road[1].lane[0].direction.north = 1;
    SimConfig.road[1].lane[0].direction.east = 1;
    SimConfig.road[1].lane[0].direction.west = 0;
    SimConfig.road[1].lane[0].minGreenTime = 1;
    SimConfig.road[1].lane[0].maxGreenTime = 10;
    SimConfig.road[1].lane[0].minRedTime = 1;
    SimConfig.road[1].lane[0].stepsPerVehicle = 2;
    SimConfig.road[1].lane[0].permissive = true;
    SimConfig.road[1].lane[0].priority = 1.f;
    //from south to north and west (straight and left)
    SimConfig.road[1].lane[1].road = &SimConfig.road[1];
    SimConfig.road[1].lane[1].direction.north = 1;
    SimConfig.road[1].lane[1].direction.east = 0;
    SimConfig.road[1].lane[1].direction.west = 1;
    SimConfig.road[1].lane[1].minGreenTime = 1;
    SimConfig.road[1].lane[1].maxGreenTime = 10;
    SimConfig.road[1].lane[1].minRedTime = 1;
    SimConfig.road[1].lane[1].stepsPerVehicle = 2;
    SimConfig.road[1].lane[1].permissive = true;
    SimConfig.road[1].lane[1].priority = 1.f;

    SimConfig.road[2].position = WEST;
    SimConfig.road[2].laneCount = 2;
    //from west to east and south (straight and right)
    SimConfig.road[2].lane[0].road = &SimConfig.road[2];
    SimConfig.road[2].lane[0].direction.south = 1;
    SimConfig.road[2].lane[0].direction.east = 1;
    SimConfig.road[2].lane[0].direction.north = 0;
    SimConfig.road[2].lane[0].minGreenTime = 1;
    SimConfig.road[2].lane[0].maxGreenTime = 10;
    SimConfig.road[2].lane[0].minRedTime = 1;
    SimConfig.road[2].lane[0].stepsPerVehicle = 2;
    SimConfig.road[2].lane[0].permissive = true;
    SimConfig.road[2].lane[0].priority = 1.f;
    //from west to east and north (straight and left)
    SimConfig.road[2].lane[1].road = &SimConfig.road[2];
    SimConfig.road[2].lane[1].direction.south = 0;
    SimConfig.road[2].lane[1].direction.east = 1;
    SimConfig.road[2].lane[1].direction.north = 1;
    SimConfig.road[2].lane[1].minGreenTime = 1;
    SimConfig.road[2].lane[1].maxGreenTime = 10;
    SimConfig.road[2].lane[1].minRedTime = 1;
    SimConfig.road[2].lane[1].stepsPerVehicle = 2;
    SimConfig.road[2].lane[1].permissive = true;
    SimConfig.road[2].lane[1].priority = 1.f;

    SimConfig.road[3].position = EAST;
    SimConfig.road[3].laneCount = 2;
    //from east to west and north (straight and left)
    SimConfig.road[3].lane[0].road = &SimConfig.road[3];
    SimConfig.road[3].lane[0].direction.south = 0;
    SimConfig.road[3].lane[0].direction.west = 1;
    SimConfig.road[3].lane[0].direction.north = 1;
    SimConfig.road[3].lane[0].minGreenTime = 1;
    SimConfig.road[3].lane[0].maxGreenTime = 10;
    SimConfig.road[3].lane[0].minRedTime = 1;
    SimConfig.road[3].lane[0].stepsPerVehicle = 2;
    SimConfig.road[3].lane[0].permissive = true;
    SimConfig.road[3].lane[0].priority = 1.f;
    //from east to west and south (straight and right)
    SimConfig.road[3].lane[1].road = &SimConfig.road[3];
    SimConfig.road[3].lane[1].direction.south = 1;
    SimConfig.road[3].lane[1].direction.west = 1;
    SimConfig.road[3].lane[1].direction.north = 0;
    SimConfig.road[3].lane[1].minGreenTime = 1;
    SimConfig.road[3].lane[1].maxGreenTime = 10;
    SimConfig.road[3].lane[1].minRedTime = 1;
    SimConfig.road[3].lane[1].stepsPerVehicle = 2;
    SimConfig.road[3].lane[1].permissive = true;
    SimConfig.road[3].lane[1].priority = 1.f;

    printf("Left+straight, right+straight, highest load first served example\r\n");

    SimInit();

    struct Vehicle v[8] = {{.name = "v0"}, {.name = "v1"}, {.name = "v2"}, 
        {.name = "v3"}, {.name = "v4"}, {.name = "v5"}, {.name = "v6"}, {.name = "v7"}};

    v[0].direction = WEST; //v0 goes from north to west (right)
    v[1].direction = SOUTH; //v1 goes from north to south
    v[2].direction = EAST; //v2 goes from north to east (left)
    v[3].direction = SOUTH; //v3 goes from north to south
    v[4].direction = SOUTH; //v4 goes from north to south
    v[5].direction = NORTH; //v5 goes from south to north
    v[6].direction = NORTH; //v6 goes from south to north
    v[7].direction = WEST; //v7 goes from south to west (left)

    //place vehicles and let simulator select the best lane
    for(size_t i = 0; i < 8; i++)
        SimPlaceVehicle(&v[i], SimSelectLane((i < 5) ? NORTH : SOUTH, v[i].direction));

    while(SimDoStep())
        ;
}
