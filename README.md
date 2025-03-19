# Intersection simulator

This is a configurable road intersection emulator written from scratch in C as a job interview exercise.
## Description
This program simulates an intersection of four roads (north, south, west, east) with highly configurable lanes on each road.

### Policies
The simulator provides four lane scheduling policies:
* right hand rule (RHL) - with no traffic lights, just plain uncontrolled intersection,
* first come, first served (FCFS) - lanes are prioritized by the order of vehicles arrival,
* highest load, first served (HLFS) - lanes are prioritized by the number of waiting vehicles,
* dynamic - lanes are scheduled based on their priority and the number of vehicles.

Additionally, there are three light timing policies:
* fixed - green light time is fixed,
* proportional - green light time is proportional to the number of vehicles,
* prioritized - green light time is proportional to the lane priority and the number of vehicles.

### Lanes

All roads can have multiple independently configured lanes. The following parameters can be configured for each line:
* possible traffic directions,
* permissive/separated,
* priority,
* minimum red and green light time,
* fixed/maximum green light time.

The permissive option allows for creating conditional left turns, where two colliding paths can have the green light at the same time. The simulator resolves such a scenario using the right hand rule. On the other hand, when the lane is separated, no colliding traffic is allowed at any time. Both colliding lines must be permissive to allow colliding traffic.

### Lights

The simulator controls the traffic lights, which can be in the following states:
* red,
* red + yellow (when switching from red to green),
* yellow (when switching from green to red),
* green,
* red + green arrow.
  
The red + green arrow light mode is always used for right turn lanes. This allows for typical conditional right turns when there is no colliding traffic.

### Vehicles

The vehicles can be added at any time during the simulation. The simulator can dynamically select appropriate lane for the vehicle based on the priority and load.

The callback function to be called on each vehicle exit, e.g. for memory deallocation, can be registered in the simulator library.

## Code structure
The code is written mostly in C. The tests are written in C++ using the GTest framework, and the script for translating input JSON files is written in Python. The project is built using CMake.

The simulator sources can be found under *sim* directory. These are built as a static library. The tests can be found under *tests* directory. The examples are stored in their corresponding subdirectories under *examples* directory. The interface allowing the simulator to use a JSON input and JSON output consits of *json.c*, *json.h*, *main.c*, and *traffic.py* files.

## Running

After building the code, it can be run using:
```
traffic.exe <input.dat> <output.json>
```
Since *input.dat* must be a binary file with packed C structures, the wrapper script can be used to parse and convert the input JSON and run the simulation. This can be done using:
```
python traffic.py <input.json> <output.json>
```

The simulation will then use the provided input JSON file and output the results to another JSON file. All simulation events are also printed to the standard output.

The simulation is preconfigured with one non-permissive lane per road with equal priorities. Lane selection policy is set to dynamic and light timing is set to prioritized.

## Algortihm description
The step of the simulation consits of several substeps:
* switching to red lights and calculating dynamic/instantaneous lane priority,
* selecting lanes based on their instantaneous priority and avoiding collisions,
* switching to green lights,
* moving vehicles.

### Calculating instantaneous priority and switching to red lights
This step maintains the green light time counter, wait counter, and initiates switch to a red light whenever necessary. If the light is red (or red + green arrow), then the instantaneous priority is calculated depending on the selection policy:
* FCFS - as a reciprocal of the vehicle sequential index, so that the lower indices have higher priority,
* HLFS - as a number of vehicles on a lane,
* dynamic - as a number of vehicles on a line plus waiting time, times the lane priority.
If there are no vehicles on a given lane, then the priority is set to -1 and the lane is never promoted.

### Selecting lanes
The lanes are sorted by their instantaneous priority. The algorithm scans all lanes with the red lights, starting with the highest priority lane. First it check, whether the minimum red time elapsed. If so, the lane is compared agains other lanes, which have green light. It is checked if a collision may occur. If not, the algorithm continues. Otherwise, it is checked whether the colliding flow is acceptable, based on the *permissive/separated* parameter of both lanes and the direction of the traffic (left turn and straight flow). If acceptable, the algorithm continues. Otherwise, the lane is skipped. If there was no collision detected in any iteration, the lane is marked as *unblocked* and the green light time is calculated.

The green light time is calculated depending on the timing policy:
* fixed - as a fixed value,
* proportional - as a direct proportion to the number of vehicles,
* prioritized - as a proportion to the number of vehicles times the priority.

### Moving vehicles
All lanes with the green light (or red + green arrow) are scanned for the first waiting vehicle. Each lane is compared against all other lanes. It is checked whether there is a collision or not. If there is, then it is decided which vehicle takes precedence. The vehicle with the green light always takes precedence over the vehicle with the green arrow. If both vehicles have green light, then the right hand rule is applied. The winning vehicle is then removed from the simulation and the user callback is executed.