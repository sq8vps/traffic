#ifndef HELPERS_H
#define HELPERS_H

#include "types.h"

/**
 * @brief Check if given path is a straight path (N-S or W-E)
 * @param start Path start
 * @param end Path end
 * @return True if straight, false otherwise
 */
static inline bool SimIsFlowStraight(enum Direction start, enum Direction end)
{
    if(((NORTH == start) && (SOUTH == end))
        || ((SOUTH == start) && (NORTH == end))
        || ((WEST == start) && (EAST == end))
        || ((EAST == start) && (WEST == end)))
        return true;
    else
        return false;
}

/**
 * @brief Check if given two lanes allow identical flow (e.g. N to W)
 * @param *laneA Lane A
 * @param *laneB Lane B
 * @return True if allow identical flow, false otherwise
 */
static inline bool SimAreFlowsIdentical(const struct Lane *laneA, const struct Lane *laneB)
{
    if(laneA->road != laneB->road)
        return false;
    
    if((laneA->direction.north && laneB->direction.north) || (laneA->direction.south && laneB->direction.south)
    || (laneA->direction.west && laneB->direction.west) || (laneA->direction.east && laneB->direction.east))
        return true;

    return false;
}

/**
 * @brief Check if given paths are perpendicular to each other
 * @param startA Path A start
 * @param endA Path A end
 * @param startB Path B start
 * @param endB Path B end
 * @return True if perpendicular, false otherwise
 */
static inline bool SimIsFlowPerpendicular(enum Direction startA, enum Direction endA, enum Direction startB, enum Direction endB)
{
    if(SimIsFlowStraight(startA, endA) && SimIsFlowStraight(startB, endB))
    {
        if(((NORTH == startA) || (SOUTH == startA)) && ((WEST == startB) || (EAST == startB)))
            return true;
        else if(((NORTH == startB) || (SOUTH == startB)) && ((WEST == startA) || (EAST == startA)))
            return true;
    }
    return false;
}

/**
 * @brief Check if given path is a left turn
 * @param start Path start
 * @param end Path end
 * @return True if is a left turn, false otherwise
 */
static inline bool SimIsLeftTurn(enum Direction start, enum Direction end)
{
    if(((NORTH == start) && (EAST == end))
    || ((SOUTH == start) && (WEST == end))
    || ((WEST == start) && (NORTH == end))
    || ((EAST == start) && (SOUTH == end)))
        return true;
    else
        return false;
}

/**
 * @brief Check if given path is a right turn
 * @param start Path start
 * @param end Path end
 * @return True if is a right turn, false otherwise
 */
static inline bool SimIsRightTurn(enum Direction start, enum Direction end)
{
    if(((NORTH == start) && (WEST == end))
    || ((SOUTH == start) && (EAST == end))
    || ((WEST == start) && (SOUTH == end))
    || ((EAST == start) && (NORTH == end)))
        return true;
    else
        return false;
}

/**
 * @brief Check if a right turn is legal from given lane
 * @param *lane Target lane
 * @return True if right turn is legal, false otherwise
 */
static inline bool SimCanTurnRightFromLane(const struct Lane *lane)
{
    if((NORTH == lane->road->position) && lane->direction.west)
        return true;
    else if((WEST == lane->road->position) && lane->direction.south)
        return true;
    else if((SOUTH == lane->road->position) && lane->direction.east)
        return true;
    else if((EAST == lane->road->position) && lane->direction.north)
        return true;
    else
        return false;
}

/**
 * @brief Check if the first vehicle at given line is ready and can move immediately
 * @param *lane Target lane
 * @return True if vehicle ready, false otherwise
 */
static inline bool SimIsVehicleReady(const struct Lane *lane)
{
    if((0 == lane->vehicleCount) || lane->blocked)
        return false;
    
    if((LIGHT_DISABLED == lane->light) || (LIGHT_GREEN == lane->light))
        return true;
    
    if((LIGHT_ARROW == lane->light) && SimIsRightTurn(lane->road->position, lane->vehicles->direction))
        return true;
    
    return false;
}

/**
 * @brief Check whether two flows are colliding
 * @param *laneA Vehicle A starting lane
 * @param directionA Vehicle A direction
 * @param *laneB Vehicle B starting lane
 * @param direciotnB Vehicle B direction
 * @return True if colliding, false otherwise
 */
static inline bool SimAreFlowsColliding(const struct Lane *laneA, enum Direction directionA, 
    const struct Lane *laneB, enum Direction directionB)
{
    if(directionA == directionB)
        return true;

    if(SimIsFlowPerpendicular(laneA->road->position, directionA, laneB->road->position, directionB))
        return true;
    
    if((SimIsFlowStraight(laneA->road->position, directionA) && SimIsLeftTurn(laneB->road->position, directionB))
    || (SimIsLeftTurn(laneA->road->position, directionA) && SimIsFlowStraight(laneB->road->position, directionB)))
        return true;
    
    return false;
}

/**
 * @brief Check whether one flow is colliding with other flows
 * @param *laneA Lane A
 * @param directionA Direction from lane A
 * @param *laneB Lane B
 * @return True if colliding, false otherwise
 */
static inline bool SimIsOneFlowCollidingWithOthers(const struct Lane *laneA, enum Direction directionA, const struct Lane *laneB)
{
    if(laneB->direction.north)
        if(SimAreFlowsColliding(laneA, directionA, laneB, NORTH))
            return true;
    if(laneB->direction.south)
        if(SimAreFlowsColliding(laneA, directionA, laneB, SOUTH))
            return true;   
    if(laneB->direction.west)
        if(SimAreFlowsColliding(laneA, directionA, laneB, WEST))
            return true;
    if(laneB->direction.east)
        if(SimAreFlowsColliding(laneA, directionA, laneB, EAST))
            return true;
    
    return false;
}

/**
 * @brief Check whether any two flows are colliding
 * @param *laneA Lane A
 * @param *laneB Lane B
 * @return True if colliding, false otherwise
 */
static inline bool SimAreAnyFlowsColliding(const struct Lane *laneA, const struct Lane *laneB)
{
    if(laneA->direction.north)
        if(SimIsOneFlowCollidingWithOthers(laneA, NORTH, laneB))
            return true;
    if(laneA->direction.south)
        if(SimIsOneFlowCollidingWithOthers(laneA, SOUTH, laneB))
            return true;
    if(laneA->direction.west)
        if(SimIsOneFlowCollidingWithOthers(laneA, WEST, laneB))
            return true;
    if(laneA->direction.east)
        if(SimIsOneFlowCollidingWithOthers(laneA, EAST, laneB))
            return true;            
    return false;
}

/**
 * @brief Check whether two vehicles are on a colliding paths
 * @param *vA Vehicle A
 * @param *vB Vehicle B
 * @return True if both vehicles are on a colliding paths, false otherwise
 */
static inline bool SimAreVehiclesColliding(const struct Vehicle *vA, const struct Vehicle *vB)
{
    //corner case when vehicles are on the same road, but different lanes and have the same direction
    //such a case is not a collision
    if((vA->lane->road == vB->lane->road) && (vA->direction == vB->direction))
        return false;
    return SimAreFlowsColliding(vA->lane, vA->direction, vB->lane, vB->direction);
}

/**
 * @brief Check whether Vehicle A is at the right hand side of the Vehicle B
 * @param *vA Vehicle A
 * @param *vB Vehicle B
 * @return True if Vehicle A is at the right hand side of the Vehicle B, false otherwise
 */
static inline bool SimIsAtRightHand(const struct Vehicle *vA, const struct Vehicle *vB)
{
    enum Direction pA, pB;
    pA = vA->lane->road->position;
    pB = vB->lane->road->position;

    if(((NORTH == pA) && (EAST == pB))
    || ((WEST == pA) && (NORTH == pB))
    || ((SOUTH == pA) && (WEST == pB))
    || ((EAST == pA) && (SOUTH == pB)))
        return true;
    else
        return false;
}

/**
 * @brief Check whether vehicle A takes precendce over vehicle B on a colliding path
 * @param *vA Vehicle A
 * @param *vB Vehicle B
 * @return True if Vehicle A has precedence over Vehicle B
 */
static bool SimTakesPrecedence(const struct Vehicle *vA, const struct Vehicle *vB)
{
    /*
    A takes precedence over B when:
    1. A has green light and B has green arrow
    2. Both have green or lights are disabled and A is at the right side of B
    3. Both have green or lights are disabled and B is not at the right side of B and
        B is trying to turn left
    */
    if((LIGHT_GREEN == vA->lane->light) && (LIGHT_ARROW == vB->lane->light))
        return true;
    else if(SimIsAtRightHand(vA, vB))
        return true;
    else if(!SimIsAtRightHand(vB, vA) && SimIsLeftTurn(vB->lane->road->position, vB->direction))
        return true;

    return false;
}

#endif