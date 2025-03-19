#include <gtest/gtest.h>
#include "helpers.h"

TEST(SimHelpers, StraightFlow)
{
    //not so many cases, check them all
    EXPECT_TRUE(SimIsFlowStraight(NORTH, SOUTH));
    EXPECT_TRUE(SimIsFlowStraight(SOUTH, NORTH));
    EXPECT_TRUE(SimIsFlowStraight(WEST, EAST));
    EXPECT_TRUE(SimIsFlowStraight(EAST, WEST));
    EXPECT_FALSE(SimIsFlowStraight(NORTH, EAST));
    EXPECT_FALSE(SimIsFlowStraight(NORTH, WEST));
    EXPECT_FALSE(SimIsFlowStraight(SOUTH, EAST));
    EXPECT_FALSE(SimIsFlowStraight(SOUTH, WEST));
    EXPECT_FALSE(SimIsFlowStraight(WEST, NORTH));
    EXPECT_FALSE(SimIsFlowStraight(WEST, SOUTH));
    EXPECT_FALSE(SimIsFlowStraight(EAST, NORTH));
    EXPECT_FALSE(SimIsFlowStraight(EAST, SOUTH));
    EXPECT_FALSE(SimIsFlowStraight(NORTH, NORTH));
    EXPECT_FALSE(SimIsFlowStraight(SOUTH, SOUTH));
    EXPECT_FALSE(SimIsFlowStraight(WEST, WEST));
    EXPECT_FALSE(SimIsFlowStraight(EAST, EAST));
}

TEST(SimHelpers, PerpendicularFlow)
{
    //check all correct cases
    EXPECT_TRUE(SimIsFlowPerpendicular(NORTH, SOUTH, WEST, EAST));
    EXPECT_TRUE(SimIsFlowPerpendicular(NORTH, SOUTH, EAST, WEST));
    EXPECT_TRUE(SimIsFlowPerpendicular(SOUTH, NORTH, WEST, EAST));
    EXPECT_TRUE(SimIsFlowPerpendicular(SOUTH, NORTH, EAST, WEST));
    //check some incorrect cases: u-turns with and without common road
    EXPECT_FALSE(SimIsFlowPerpendicular(NORTH, NORTH, WEST, EAST));
    EXPECT_FALSE(SimIsFlowPerpendicular(NORTH, NORTH, NORTH, EAST));
    EXPECT_FALSE(SimIsFlowPerpendicular(NORTH, NORTH, WEST, WEST));
}

TEST(SimHelpers, LeftTurn)
{
    //check all correct cases
    EXPECT_TRUE(SimIsLeftTurn(NORTH, EAST));
    EXPECT_TRUE(SimIsLeftTurn(SOUTH, WEST));
    EXPECT_TRUE(SimIsLeftTurn(WEST, NORTH));
    EXPECT_TRUE(SimIsLeftTurn(EAST, SOUTH));
    //check right turn, no turn, u-turn
    EXPECT_FALSE(SimIsLeftTurn(NORTH, WEST));
    EXPECT_FALSE(SimIsLeftTurn(NORTH, NORTH));
    EXPECT_FALSE(SimIsLeftTurn(NORTH, SOUTH));
}

TEST(SimHelpers, RightTurn)
{
    //check all correct cases
    EXPECT_TRUE(SimIsRightTurn(NORTH, WEST));
    EXPECT_TRUE(SimIsRightTurn(SOUTH, EAST));
    EXPECT_TRUE(SimIsRightTurn(WEST, SOUTH));
    EXPECT_TRUE(SimIsRightTurn(EAST, NORTH));
    //check left turn, no turn, u-turn
    EXPECT_FALSE(SimIsRightTurn(NORTH, SOUTH));
    EXPECT_FALSE(SimIsRightTurn(NORTH, EAST));
    EXPECT_FALSE(SimIsRightTurn(NORTH, NORTH));
}

TEST(SimHelpers, RightTurnFromLane)
{
    struct Road road;
    road.position = NORTH;
    road.laneCount = 1;
    road.lane[0].road = &road;
    road.lane[0].direction.west = 1;    
    EXPECT_TRUE(SimCanTurnRightFromLane(&road.lane[0]));
    road.lane[0].direction.west = 0;
    road.lane[0].direction.north = 1;
    road.lane[0].direction.east = 1;
    EXPECT_FALSE(SimCanTurnRightFromLane(&road.lane[0]));
    road.position = SOUTH;
    road.lane[0].direction.east = 1;
    EXPECT_TRUE(SimCanTurnRightFromLane(&road.lane[0]));
    road.lane[0].direction.east = 0;
    EXPECT_FALSE(SimCanTurnRightFromLane(&road.lane[0]));
}

TEST(SimHelper, RightHand)
{
    struct Road road[2];
    struct Vehicle v[2];
    road[0].laneCount = 1;
    road[0].lane[0].road = &road[0];
    road[0].lane[0].vehicleCount = 1;
    road[0].lane[0].vehicles = &v[0];
    v[0].lane = &road[0].lane[0];

    road[1].laneCount = 1;
    road[1].lane[0].road = &road[1];
    road[1].lane[0].vehicleCount = 1;
    road[1].lane[0].vehicles = &v[1];
    v[1].lane = &road[1].lane[0];

    road[0].position = NORTH;
    road[1].position = EAST;
    EXPECT_TRUE(SimIsAtRightHand(&v[0], &v[1]));
    road[0].position = SOUTH;
    road[1].position = WEST;
    EXPECT_TRUE(SimIsAtRightHand(&v[0], &v[1]));
    road[0].position = WEST;
    road[1].position = NORTH;
    EXPECT_TRUE(SimIsAtRightHand(&v[0], &v[1]));
    road[0].position = EAST;
    road[1].position = SOUTH;
    EXPECT_TRUE(SimIsAtRightHand(&v[0], &v[1]));

    road[0].position = NORTH;
    road[1].position = NORTH;
    EXPECT_FALSE(SimIsAtRightHand(&v[0], &v[1]));

    road[1].position = SOUTH;
    EXPECT_FALSE(SimIsAtRightHand(&v[0], &v[1]));

    road[1].position = WEST;
    EXPECT_FALSE(SimIsAtRightHand(&v[0], &v[1]));
}