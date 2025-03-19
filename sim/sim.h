#ifndef SIM_H
#define SIM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "types.h"

enum SimSelectionPolicy
{
    SIM_RIGHT_HAND_RULE = 0, /**< Lights disabled, use right hand rule */
    SIM_FCFS = 1, /**< First come, first served */
    SIM_HLFS = 2, /**< Highest load, first served */
    SIM_DYNAMIC = 3, /**< Dynamic priority-based aligorithm */
};

enum SimTimePolicy
{
    SIM_TIME_FIXED = 0, /**< Fixed light timing */
    SIM_TIME_PROPORTIONAL = 1, /**< Light timing proportional to traffic with max and min values */
    SIM_TIME_PRIORITIZED = 2, /**< Light timing proportional to traffic times lane priority */
};

struct SimConfig
{
    struct Road road[4]; /** Roads - always 4 */
    enum SimSelectionPolicy selectionPolicy; /**< Lane selection policy */
    enum SimTimePolicy timePolicy; /**< Light timing policy */
};

extern struct SimConfig SimConfig; /**< Simulation configuration */

typedef void (*SimVehicleExitedCallback)(struct Vehicle *vehicle, void *context);

/**
 * @brief Register callback for vehicles that exited the intersection
 */
void SimRegisterVehicleExitedCallback(SimVehicleExitedCallback callback, void *context);

/**
 * @brief Place vehicle on given lane
 * @param *vehicle Vehicle to be placed
 * @param *lane Target lane
 * @return 0 on success, <0 on failure
 */
int SimPlaceVehicle(struct Vehicle *vehicle, struct Lane *lane);

/**
 * @brief Select best lane based on starting and ending road
 * @param start Starting road direction
 * @param end Ending road direction
 * @return Best lane pointer, NULL if no lane is available
 */
struct Lane* SimSelectLane(enum Direction start, enum Direction end);

/**
 * @brief Perform simulation step
 * @return True if simulation not ended (vehicles still waiting), false otherwise
 */
bool SimDoStep(void);

/**
 * @brief Initialize simulation
 * @attention Call this function *before* placing vehicles
 */
void SimInit(void);

#endif