#include <stdio.h>
#include <stdlib.h>

typedef struct arrival {
    int routeCode;
    int vehCode;
    float arrivalTime;
} Arrival;

int main() {
    Arrival a;
    while (scanf("%d %d %f", &a.routeCode, &a.vehCode, &a.arrivalTime) == 3) {
        fwrite(&a, sizeof(Arrival), 1, stdout);
    }
    return 0;
}