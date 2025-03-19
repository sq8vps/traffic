#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define MAX_LANES 3 /**< Maximum incoming lanes per road */

#define MAX_VEHICLE_NAME_LENGTH 32 /**< Maximum vehicle name length, might be 0 if vehicle structures are allocated dynamically */

/**
 * @brief Light state
 */
enum Light
{
    LIGHT_DISABLED = 0,
    LIGHT_RED = 1,
    LIGHT_RED_YELLOW = 2,
    LIGHT_YELLOW = 3,
    LIGHT_GREEN = 4,
    LIGHT_ARROW = 5,
};

/**
 * @brief Geographical direction
 */
enum Direction
{
    NORTH = 0,
    SOUTH = 1,
    WEST = 2,
    EAST = 3,
    DIRECTION_LIMIT = 3,
};

struct Road;
struct Vehicle;

/**
 * @brief Lane on a road
 */
struct Lane
{
    /* Lane configuration */
    struct
    {
        uint8_t north : 1;
        uint8_t south : 1;
        uint8_t west : 1;
        uint8_t east : 1;
    } direction; /**< Allowed target directions */
    bool permissive; /**< Lane is not protected - a colliding flow may occur */
    float priority; /**< Lane priority */
    uint32_t minGreenTime; /**< Minimum green light time */
    uint32_t minRedTime; /**< Minimum red light time */
    union
    {
        uint32_t maxGreenTime; /**< Maximum green light time */
        uint32_t greenTime; /**< Fixed green time */
    };
    uint32_t stepsPerVehicle; /**< Time steps per vehicle for proportional timing */

    /* Lane state */
    enum Light light; /**< Current light state */
    struct Road *road; /**< Parent road */
    struct Vehicle *vehicles; /**< Line of vehicles */
    size_t vehicleCount; /**< Number of vehicles */
    float dynamicPriority; /**< Dynamic lane priority */
    uint32_t stepsBeforeChange; /**< Steps left to change the light */
    uint32_t waitTime; /**< Steps elapsed waiting for the green light */
    bool blocked; /**< Lane is blocked in given step, because there was a vehicle on another lane
        that had precedence over this lane */
    bool unblocked; /**< Lane has been unblocked and it's light will change to green */
};

/**
 * @brief Road in a junction
 */
struct Road
{
    /* Road configuration */
    enum Direction position; /**< Geographical road position */
    struct Lane lane[MAX_LANES]; /**< Incoming lanes */
    size_t laneCount; /**< Number of incoming lanes */
};

/**
 * @brief Vehicle on a lane
 */
struct Vehicle
{
    size_t index; /**< Vehicle sequential index */
    enum Direction direction; /**< Vehicle target direction */
    struct Lane *lane; /**< Current lane */
    struct Vehicle *next; /**< Next vehicle in line */
    char name[MAX_VEHICLE_NAME_LENGTH]; /**< Vehicle name */
};

#endif