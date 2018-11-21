/*
 * pts2xodr.cpp
 *
 */

#include <stdio.h>
#include "odRoad.h"

void usage(char *aName) {
    fprintf(stderr,"Usage: %s fin.pts fout.xodr\n", aName);
    exit(1);
}

int main(int argc, char **argv) {

    if (argc!=3) usage(argv[0]);

    odRoad road;

    road.loadPts(argv[1]);

    road.print();

    road.saveXodr(argv[2]);

    return 0;
}
