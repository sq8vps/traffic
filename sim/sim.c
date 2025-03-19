#include "sim.h"
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

struct SimConfig SimConfig = {.road[0].position = NORTH, .road[0].laneCount = 0,
    .road[1].position = SOUTH, .road[1].laneCount = 0,
    .road[2].position = EAST, .road[2].laneCount = 0,
    .road[3].position = WEST, .road[3].laneCount = 0};

static struct 
{
    SimVehicleExitedCallback vehicleExitCallback; /**< Vehicle exit callback */
    void *context; /**< Vehicle exit callback context */
    size_t nextVehicle; /**< Next vehicle sequential index */
    uint32_t step; /**< Current simulation step */
    struct Lane *lanes[MAX_LANES * 4]; /**< List of lanes */
    size_t numLanes; /**< Number of lanes */
    size_t numVehicles; /**< Number of vehicles */
} SimState = {.vehicleExitCallback = NULL, .nextVehicle = 0, .step = 0, .numLanes = 0, .numVehicles = 0};

static const char SimDirectionToChar[] = {[NORTH] = 'N', [SOUTH] = 'S', [WEST] = 'W', [EAST] = 'E'};
static const char *SimDirectionToString[] = {[NORTH] = "north", [SOUTH] = "south", [WEST] = "west", [EAST] = "east"};
static const char *SimLightToString[] = {
    [LIGHT_RED] = "red",
    [LIGHT_RED_YELLOW] = "red+yellow",
    [LIGHT_YELLOW] = "yellow",
    [LIGHT_GREEN] = "green",
    [LIGHT_ARROW] = "red+arrow",
};

/**
 * @brief Get lane attractiveness for placing new vehicles
 * @param *lane Target lane
 * @return Attractiveness
 */
static inline float SimGetLaneAtractiveness(const struct Lane *lane)
{
    if(0 != lane->vehicleCount)
        return lane->priority;
    else
        return lane->priority / (float)lane->vehicleCount;
}

/**
 * @brief Exit vehicle/remove from simulation
 * @param *vehicle Vehicle pointer
 */
static void SimExitVehicle(struct Vehicle *vehicle)
{
    vehicle->lane->vehicles = vehicle->next;
    --vehicle->lane->vehicleCount;
    --SimState.numVehicles;
    printf("Vehicle %s from %s exited at %s\r\n", vehicle->name,
        SimDirectionToString[vehicle->lane->road->position], SimDirectionToString[vehicle->direction]);
    if(NULL != SimState.vehicleExitCallback)
        SimState.vehicleExitCallback(vehicle, SimState.context);
}


void SimRegisterVehicleExitedCallback(SimVehicleExitedCallback callback, void *context)
{
    SimState.vehicleExitCallback = callback;
    SimState.context = context;
}

int SimPlaceVehicle(struct Vehicle *vehicle, struct Lane *lane)
{
    if(NULL == vehicle)
    {
        printf("Vehicle is NULL!\r\n");
        return -1;
    }
    if(NULL == lane)
    {
        printf("Lane is NULL!\r\n");
        return -1;
    }

    vehicle->index = SimState.nextVehicle++;
    vehicle->next = NULL;
    vehicle->lane = lane;

    if(NULL == lane->vehicles)
        lane->vehicles = vehicle;
    else
    {
        struct Vehicle *v = lane->vehicles;
        while(NULL != v->next)
            v = v->next;
        
        v->next = vehicle;
    }
    ++lane->vehicleCount;
    ++SimState.numVehicles;

    return 0;
}

struct Lane* SimSelectLane(enum Direction start, enum Direction end)
{
    if((start > DIRECTION_LIMIT) || (end > DIRECTION_LIMIT))
        return NULL;
    
    struct Road *road = &SimConfig.road[start];
    struct Lane *best = NULL;
    float bestAttractiveness = -1.f;

    for(size_t i = 0; i < road->laneCount; i++)
    {
        if(((NORTH == end) && road->lane[i].direction.north)
        || ((SOUTH == end) && road->lane[i].direction.south)
        || ((WEST == end) && road->lane[i].direction.west)
        || ((EAST == end) && road->lane[i].direction.east))
        {
            float attractiveness = SimGetLaneAtractiveness(&road->lane[i]);
            if(attractiveness > bestAttractiveness)
            {
                bestAttractiveness = attractiveness;
                best = &road->lane[i];
            }
        }
    }
    return best;
}

/**
 * @brief Clear "blocked" states for all lanes
 */
static void SimClearBlockedStates(void)
{
    struct Lane **lane = SimState.lanes;
    size_t i = SimState.numLanes;
    while(i--)
    {
        (*lane)->blocked = false;
        (*lane)->unblocked = false;
        ++lane;
    }
}

/**
 * @brief Handle vehicles in simulation step
 */
static void SimHandleVehicles(void)
{

    //scan though all lanes for ready vehicles
    struct Lane **lane = SimState.lanes;
    size_t i = SimState.numLanes;
    while(i)
    {
        if(SimIsVehicleReady(*lane))
        {
            //scan other lanes for collisions
            struct Lane **other = SimState.lanes;
            size_t k = SimState.numLanes;
            while(k)
            {
                //collision can only happen when the vehicles on the other lane are ready
                //of course skip comparing the same lanes...
                if((*lane != *other) && SimIsVehicleReady(*other))
                {
                    if(SimAreVehiclesColliding((*lane)->vehicles, (*other)->vehicles))
                    {
                        if(SimTakesPrecedence((*lane)->vehicles, (*other)->vehicles))
                            (*other)->blocked = true;
                        else
                            break;
                    }
                }
                --k;
                ++other;
            }

            if(0 == k)
            {
                struct Vehicle *v = (*lane)->vehicles;
                SimExitVehicle(v);
            }

        }
        --i;
        ++lane;
    }
}

static void SimPrintLightState(const struct Lane *lane)
{
    char dest[5];
    char *d = dest;
    if(lane->direction.north)
        *d++ = 'N';
    if(lane->direction.south)
        *d++ = 'S';
    if(lane->direction.west)
        *d++ = 'W';
    if(lane->direction.east)
        *d++ = 'E';
    *d = '\0';

    printf("Light at lane %c->%s switched to %s\r\n", 
        SimDirectionToChar[lane->road->position], dest, SimLightToString[lane->light]);
}

static int SimComparePriorities(const void *a, const void *b)
{
    const struct Lane *const *laneA = a, *const *laneB = b;
    if((*laneA)->dynamicPriority < (*laneB)->dynamicPriority)
        return 1;
    else if((*laneA)->dynamicPriority > (*laneB)->dynamicPriority)
        return -1;
    else
        return 0;
}

static void SimHandleRedLights(void)
{
    struct Lane **lane = SimState.lanes;
    size_t i = SimState.numLanes;
    while(i)
    {
        if((LIGHT_GREEN == (*lane)->light))
        {
            if((0 == (*lane)->stepsBeforeChange--))
            {
                (*lane)->light = LIGHT_YELLOW;
                SimPrintLightState(*lane);
            }
        }
        else if(LIGHT_YELLOW == (*lane)->light)
        {
            if(SimCanTurnRightFromLane(*lane))
                (*lane)->light = LIGHT_ARROW;
            else
                (*lane)->light = LIGHT_RED;

            SimPrintLightState(*lane);
        }
        else if((LIGHT_RED == (*lane)->light) || (LIGHT_ARROW == (*lane)->light))
        {
            if(0 != (*lane)->vehicleCount)
            {
                //always use some kind of a "dynamic priority"
                //which can be based on different things depending on the policy
                if(SIM_FCFS == SimConfig.selectionPolicy)
                    (*lane)->dynamicPriority = 1.f / (float)(*lane)->vehicles->index;
                else if(SIM_HLFS == SimConfig.selectionPolicy)
                    (*lane)->dynamicPriority = (float)(*lane)->vehicleCount;
                else if(SIM_DYNAMIC == SimConfig.selectionPolicy)
                {
                    (*lane)->dynamicPriority = (float)(*lane)->vehicleCount + (float)(*lane)->waitTime;
                    (*lane)->dynamicPriority *= (*lane)->priority;
                }
                ++(*lane)->waitTime;
            }
            else
                (*lane)->dynamicPriority = -1.f; //never promote lanes with no vehicles
        }
        --i;
        ++lane;
    }
}

static void SimHandleSelection(void)
{
    //sort lanes by highest dynamic priority first
    qsort(SimState.lanes, SimState.numLanes, sizeof(*SimState.lanes), SimComparePriorities);

    //starting from the highest priority waiting lane, check if it's safe to switch to green
    struct Lane **lane = SimState.lanes;
    size_t i = SimState.numLanes;
    while(i)
    {
        if((LIGHT_RED == (*lane)->light) || ((LIGHT_ARROW == (*lane)->light)))
        {
            if(0 == (*lane)->vehicleCount) //always skip lanes with no vehicles
            {
                ++lane;
                --i;
                continue;
            }

            if((*lane)->waitTime < (*lane)->minRedTime)
                break;
            
            struct Lane **other = SimState.lanes;
            size_t k = SimState.numLanes;
            while(k)
            {
                if(*lane == *other)
                {
                    ++other;
                    --k;
                    continue;
                }
                //check for colliding flows, which may disqualify the lane
                //however, there are some cases when it is acceptable
                //checking makes sense only when the other lane has green or red-yellow light, or has been just unblocked
                if(((LIGHT_GREEN == (*other)->light) || (LIGHT_RED_YELLOW == (*other)->light) || (*other)->unblocked) 
                    && SimAreAnyFlowsColliding(*lane, *other))
                {
                    //handle permissive intersections:
                    //when two lanes are permissive, then check if they are at the opposing sides
                    //this way we can simulate a real scenario of permissive left turns
                    if(((*lane)->permissive && (*other)->permissive)
                        && (SimIsFlowStraight((*lane)->road->position, (*other)->road->position) || SimAreFlowsIdentical(*lane, *other)))
                    {

                    }
                    else
                    {
                        break;
                    }
                }
                --k;
                ++other;
            }
            if(0 == k)
            {
                //k=0, that is, there is no possible collision or the colision is "legal"
                (*lane)->unblocked = true;
                if(SIM_TIME_FIXED == SimConfig.timePolicy)
                {
                    (*lane)->stepsBeforeChange = (*lane)->greenTime;
                }
                else if((SIM_TIME_PROPORTIONAL == SimConfig.timePolicy)
                    || (SIM_TIME_PRIORITIZED == SimConfig.timePolicy))
                {
                    (*lane)->stepsBeforeChange = (*lane)->vehicleCount * (*lane)->stepsPerVehicle;
                    if(SIM_TIME_PRIORITIZED == SimConfig.timePolicy)
                        (*lane)->stepsBeforeChange = (float)((*lane)->stepsBeforeChange) * (*lane)->priority;
                    if((*lane)->stepsBeforeChange < (*lane)->minGreenTime)
                        (*lane)->stepsBeforeChange = (*lane)->minGreenTime;
                    else if((*lane)->stepsBeforeChange > (*lane)->maxGreenTime)
                        (*lane)->stepsBeforeChange = (*lane)->maxGreenTime;
                }
            }
            else
                break;
        }
        --i;
        ++lane;
    }
}

static void SimHandleSwitchToGreen(void)
{
    struct Lane **lane = SimState.lanes;
    size_t i = SimState.numLanes;
    while(i)
    {
        if(((LIGHT_RED == (*lane)->light) || (LIGHT_ARROW == (*lane)->light)) && (*lane)->unblocked)
        {
            (*lane)->light = LIGHT_RED_YELLOW;
            SimPrintLightState(*lane);
        }
        else if(LIGHT_RED_YELLOW == (*lane)->light)
        {
            (*lane)->light = LIGHT_GREEN;
            SimPrintLightState(*lane);
        }
        --i;
        ++lane;
    }
}

bool SimDoStep(void)
{
    SimClearBlockedStates();
    if(SIM_RIGHT_HAND_RULE != SimConfig.selectionPolicy)
    {
        SimHandleRedLights();
        SimHandleSelection();
        SimHandleSwitchToGreen();
    }
    SimHandleVehicles();
    printf("Step done, %lu vehicles remaining\r\n", SimState.numVehicles);
    return (0 != SimState.numVehicles);
}

void SimInit(void)
{
    printf("Initializing simulation...\r\n");
    size_t index = 0;
    for(uint8_t i = 0; i < (DIRECTION_LIMIT + 1); i++)
    {
        for(size_t k = 0; k < SimConfig.road[i].laneCount; k++)
        {
            if(SIM_RIGHT_HAND_RULE != SimConfig.selectionPolicy)
            {
                if(SimCanTurnRightFromLane(&SimConfig.road[i].lane[k]))
                    SimConfig.road[i].lane[k].light = LIGHT_ARROW;
                else
                    SimConfig.road[i].lane[k].light = LIGHT_RED;
            }
            else
                SimConfig.road[i].lane[k].light = LIGHT_DISABLED;
                
            SimPrintLightState(&SimConfig.road[i].lane[k]);
            SimConfig.road[i].lane[k].dynamicPriority = -1.f;

            SimState.lanes[index] = &SimConfig.road[i].lane[k];
            ++index;
        }
    }
    SimState.numLanes = index;
    SimState.nextVehicle = 1;
    SimState.numVehicles = 0;
    SimState.step = 0;
    printf("Initialization finished\r\n\r\n");
}