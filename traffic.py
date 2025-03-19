import sys
import json
import struct
import subprocess

DATA_FILE = "cinput.dat"

def encodeDirection(dir):
    if dir == "north":
        return 0
    elif dir == "south":
        return 1
    elif dir == "west":
        return 2
    elif dir == "east":
        return 3
    else:
        raise Exception("Unknown direction " + dir)

with open(DATA_FILE, "wb") as cinput:
    with open(sys.argv[1]) as f:
        cmds = json.load(f)["commands"]
        for cmd in cmds:
            if cmd["type"] == "addVehicle":
                nameLen = len(cmd["vehicleId"])
                data = struct.pack("=BBBI{}s".format(nameLen), 1, encodeDirection(cmd["startRoad"]),
                    encodeDirection(cmd["endRoad"]), nameLen, bytes(cmd["vehicleId"], encoding="utf8"))
            elif cmd["type"] == "step":
                data = struct.pack("=BxxI", 2, 0)
            else:
                raise Exception("Unknown command " + cmd["type"]) 

            cinput.write(data)

print(subprocess.run(["build/traffic.exe", DATA_FILE, sys.argv[2]], capture_output = True, text = True).stdout)