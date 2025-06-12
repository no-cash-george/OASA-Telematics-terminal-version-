#include <stdlib.h>
#include <stdio.h>

typedef struct arrival {
    int routeCode;
    int vehCode;
    float arrivalTime;
} Arrival;

int main() {
    int stop_id;
    fprintf(stderr, "Give me the stop code: ");
    fflush(stdout);
    scanf("%d", &stop_id);

    char command[256];
    snprintf(command, sizeof(command), "./arrivals.sh %d", stop_id);

    if (system(command) != 0) {
        fprintf(stderr, "Error running arrivals.sh\n");
        return 1;
    }

    FILE *fp = fopen("arrivals.bin", "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    if (fileSize == 0) {
        printf("No arrivals found for stop %d\n", stop_id);
        fclose(fp);
        return 0;
    }

    size_t count = fileSize / sizeof(Arrival);
    printf("Found %zu arrivals\n", count);

    Arrival *arrivals = malloc(count * sizeof(Arrival));
    if (!arrivals) {
        perror("malloc");
        fclose(fp);
        return 1;
    }

    size_t read = fread(arrivals, sizeof(Arrival), count, fp);
    if (read != count) {
        fprintf(stderr, "fread mismatch: expected %zu, got %zu\n", count, read);
        free(arrivals);
        fclose(fp);
        return 1;
    }

    for (size_t i = 0; i < count; i++) {
        printf("Route: %d | Vehicle: %d | Arrival in: %.1f mins\n",
               arrivals[i].routeCode,
               arrivals[i].vehCode,
               arrivals[i].arrivalTime);
    }

    free(arrivals);
    fclose(fp);
    return 0;
}