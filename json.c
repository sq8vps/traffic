#include "json.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "sim.h"

struct InCommand
{
    uint8_t type;
    uint8_t startRoad;
    uint8_t endRoad;
    uint32_t length;
} __attribute__ ((packed));

enum
{
    COMMAND_ADD_VEHICLE = 1,
    COMMAND_STEP = 2,
};

static void JsonVehicleExitedCallback(struct Vehicle *vehicle, void *context)
{
    FILE *f = context;
    fprintf(f, "\r\n\"%s\",", vehicle->name);
    free(vehicle);
}

int JsonRunSimFromExternalData(const char *inPath, const char *outPath)
{
    FILE *in = fopen(inPath, "rb");
    if(NULL == in)
    {
        printf("Unable to open %s\r\n", inPath);
        return -1;
    }

    FILE *out = fopen(outPath, "w+");
    if(NULL == in)
    {
        printf("Unable to open %s\r\n", outPath);
        return -1;
    }

    fprintf(out, "{\r\n\"stepStatuses\": [ ");

    SimInit();

    SimRegisterVehicleExitedCallback(JsonVehicleExitedCallback, out);

    printf("Using %s as input and %s as output\r\n", inPath, outPath);
    
    struct InCommand cmd;
    size_t size = 0;

    while(1)
    {
        size = fread(&cmd, 1, sizeof(cmd), in);
        if(0 == size)
            break;
        if(sizeof(cmd) != size)
        {
            fclose(in);
            fclose(out);
            printf("Input file is broken (incomplete command structure)\r\n");
            return -1;
        }

        switch(cmd.type)
        {
            case COMMAND_ADD_VEHICLE:
                struct Vehicle *v = malloc(sizeof(*v) + cmd.length + 1);
                if(NULL == v)
                {
                    fclose(in);
                    fclose(out);
                    printf("Memory allocation failed\r\n");
                    return -1;
                }

                v->direction = cmd.endRoad;

                if(cmd.length != fread(v->name, 1, cmd.length, in))
                {
                    fclose(in);
                    fclose(out);
                    printf("Input file is broken (incomplete command structure)\r\n");
                    return -1;
                }
                v->name[cmd.length] = '\0';

                SimPlaceVehicle(v, SimSelectLane(cmd.startRoad, cmd.endRoad));
                break;
            case COMMAND_STEP:
                fprintf(out, "\r\n{\r\n\"leftVehicles\": [ ");
                SimDoStep();
                fseek(out, ftell(out) - 1, SEEK_SET); //remove comma after the last element
                fprintf(out, "]\r\n},");
                break;
            default:
                fclose(in);
                fclose(out);
                printf("Unknown encoded command: %u\r\n", (unsigned int)cmd.type);
                return -1;
        }

    } 

    fseek(out, ftell(out) - 1, SEEK_SET); //remove comma after the last element
    fprintf(out, "]\r\n}\r\n");
    printf("Simulation finished\r\n");
    fclose(out);
    fclose(in);
    return 0;
}
