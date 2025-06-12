#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define FAVOURITES_FILE "favourites.txt"

typedef struct arrival {
    int routeCode;
    int vehCode;
    float arrivalTime;
} Arrival;

typedef struct arrivals
{
    Arrival* arrivalArray;
    int numOfArrivals;
}Arrivals;

typedef struct favourite
{
    int code;
    char* nameOfFacourite;
}Favourite;

typedef struct favourites
{
    Favourite* favourites;
    int numOfFavourites;
}Favourites;

Arrivals getArrivals(int code);
void showArrivals(Arrivals arrivals);
void showMenu();
void favouritesMenu();
void addFavouriteMenu();
Favourites getFavourites();

int main() {
    int running = 1;
    while (running)
    {
        showMenu();
        int sel;
        scanf("%d", &sel);

        switch (sel)
        {
        case 1:
        {
            system("clear");
            printf("Give me a stop code: ");
            int code;
            scanf("%d", &code);

            Arrivals arrivals = getArrivals(code);
            showArrivals(arrivals);

            if(arrivals.numOfArrivals)
            {
                free(arrivals.arrivalArray);
            }

        }break;
        case 2:
        {
            favouritesMenu();            
        }break;
        case 3:
        {
            addFavouriteMenu();

        }break;
        default:
        {
            running = 0;
        }break;
        }

        system("clear");

    }
    return 0;
}

Arrivals getArrivals(int stop_id)
{
    Arrivals nullArrivals;
    nullArrivals.arrivalArray = NULL;
    nullArrivals.numOfArrivals = 0;

    char command[256];
    snprintf(command, sizeof(command), "./arrivals.sh %d", stop_id);

    if (system(command) != 0) {
        fprintf(stderr, "Error running arrivals.sh\n");
        return nullArrivals;
    }

    FILE *fp = fopen("arrivals.bin", "rb");
    if (!fp) {
        perror("fopen");
        return nullArrivals;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    if (fileSize == 0) {
        fclose(fp);
        return nullArrivals;
    }

    size_t count = fileSize / sizeof(Arrival);

    Arrival *arrivals = malloc(count * sizeof(Arrival));
    if (!arrivals) {
        perror("malloc");
        fclose(fp);
        return nullArrivals;
    }

    size_t read = fread(arrivals, sizeof(Arrival), count, fp);
    if (read != count) {
        fprintf(stderr, "fread mismatch: expected %zu, got %zu\n", count, read);
        free(arrivals);
        fclose(fp);
        return nullArrivals;
    }

    Arrivals returnArrivals;
    returnArrivals.arrivalArray = arrivals;
    returnArrivals.numOfArrivals = count;

    fclose(fp);
    return returnArrivals;
}

void showMenu()
{
    system("clear");
    printf("1. Get arrivals by code\n2.Favourites\n3.Add Favourites\n4.Exit\n>");
}

void showArrivals(Arrivals arrivals)
{
    if(arrivals.numOfArrivals == 0)
    {
        printf("No arrivals for the given stop\nPress enter to continue");
        getchar();
        getchar();
        return;
    }

    for (size_t i = 0; i < arrivals.numOfArrivals; i++) 
    {
        printf("Route: %d | Vehicle: %d | Arrival in: %.1f mins\n",
               arrivals.arrivalArray[i].routeCode,
               arrivals.arrivalArray[i].vehCode,
               arrivals.arrivalArray[i].arrivalTime);
    } 

    printf("Press enter to continue");
    getchar();
    getchar();
}

void favouritesMenu()
{
    if(getFavourites().numOfFavourites == 0)
    {
        system("clear");
        printf("No favourites\nPress enter to continue");
        getchar();
        getchar();
        return;
    }

    printf("Favourites\n");
    Favourites favourites = getFavourites();

    for(int i = 0; i < favourites.numOfFavourites; i++)
    {
        printf("%d. Stop Name: %s | Stop Code %d\n", i, favourites.favourites[i].nameOfFacourite, favourites.favourites[i].code);
    }

    printf("Give number to show arrivals: ");
    int sel;
    scanf("%d", &sel);

    if(sel <= favourites.numOfFavourites)
    {
        system("clear");
        showArrivals(getArrivals(favourites.favourites[sel].code));
    }else
    {
        printf("Invalid selection");
    }

    free(favourites.favourites);
}

void addFavouriteMenu()
{
    int code;
    char name[256];

    printf("Give stop code: ");
    scanf("%d", &code);
    getchar(); // consume newline

    Favourites f = getFavourites();

    // Check for duplicate stop code
    for (int i = 0; i < f.numOfFavourites; i++) {
        if (f.favourites[i].code == code) {
            printf("This stop is already in your favourites!\nPress enter to continue.");
            getchar();

            // Free memory and return
            for (int j = 0; j < f.numOfFavourites; j++)
                free(f.favourites[j].nameOfFacourite);
            free(f.favourites);
            return;
        }
    }

    printf("Give name for favourite: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; // remove newline

    FILE* file = fopen(FAVOURITES_FILE, "a");
    if (!file) {
        perror("fopen");
        return;
    }

    fprintf(file, "%d %s\n", code, name);
    fclose(file);

    printf("Favourite added. Press enter to continue.");
    getchar();

    // Free memory
    for (int i = 0; i < f.numOfFavourites; i++)
        free(f.favourites[i].nameOfFacourite);
    free(f.favourites);
}

Favourites getFavourites()
{
    Favourites f;
    f.favourites = NULL;
    f.numOfFavourites = 0;

    FILE* file = fopen(FAVOURITES_FILE, "r");
    if (!file) {
        return f; // No favourites yet
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        int code;
        char name[256];

        if (sscanf(line, "%d %[^\n]", &code, name) == 2) {
            f.favourites = realloc(f.favourites, (f.numOfFavourites + 1) * sizeof(Favourite));
            f.favourites[f.numOfFavourites].code = code;
            f.favourites[f.numOfFavourites].nameOfFacourite = strdup(name);
            f.numOfFavourites++;
        }
    }

    fclose(file);
    return f;
}



