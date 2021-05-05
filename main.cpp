#include <stdio.h>
#include "tef/aurora/effects/simpleEffect.h"
#include "lib/fadecandy/opc_client.h"
#include "lib/svl/SVL.h"
#include "lib/rapidjson/reader.h"

int main(int argc, char** argv)
{
    rapidjson::ParseFlag pf;
    Vec3 v3;
    TEF::Aurora::Effects::SimpleEffect se;
    OPCClient opc;
    printf("herro!\n");
}