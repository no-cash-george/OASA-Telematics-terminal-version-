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
char* getRouteName(int routeCode);


int main() {
    int running = 1;
    while (running)
    {
        showMenu();
        int sel;
        fflush(stdout);
        scanf("%d", &sel);
        getchar(); // consume newline after scanf

        switch (sel)
        {
        case 1:
        {
            system("clear");
            printf("Give me a stop code: ");
            fflush(stdout);
            int code;
            scanf("%d", &code);
            getchar(); // consume newline

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
    fflush(stdout);
}

void showArrivals(Arrivals arrivals)
{
    int getNext = 1;
    while(getNext){ 
        system("clear");
        if(arrivals.numOfArrivals == 0)
        {
            printf("No arrivals for the given stop\nPress press 0 to continue");
            fflush(stdout);
            int sel;
            scanf("%d", &sel);
            if(sel == 0)
            {
                break;
            }else
            {
                continue;
            }
        }

        for (size_t i = 0; i < arrivals.numOfArrivals; i++) 
        {
            printf("Route: %s %.1f'\n",
                    getRouteName(arrivals.arrivalArray[i].routeCode),
                    arrivals.arrivalArray[i].arrivalTime);
        } 

        printf("0.Exit | 1.Refresh\n>");
        fflush(stdout);
        int sel;
        scanf("%d", &sel);
        if(sel == 0)
        {
            break;
        }
    }
}

void favouritesMenu()
{
    Favourites favourites = getFavourites();

    if(favourites.numOfFavourites == 0)
    {
        system("clear");
        printf("No favourites\nPress enter to continue");
        fflush(stdout);
        getchar();
        return;
    }

    printf("Favourites\n");
    for(int i = 0; i < favourites.numOfFavourites; i++)
    {
        printf("%d. Stop Name: %s | Stop Code %d\n", i, favourites.favourites[i].nameOfFacourite, favourites.favourites[i].code);
    }

    printf("Give number to show arrivals: ");
    fflush(stdout);
    int sel;
    scanf("%d", &sel);
    getchar(); // consume newline

    if(sel >= 0 && sel < favourites.numOfFavourites)
    {
        system("clear");
        showArrivals(getArrivals(favourites.favourites[sel].code));
    } else
    {
        printf("Invalid selection\nPress enter to continue");
        fflush(stdout);
        getchar();
    }

    for (int i = 0; i < favourites.numOfFavourites; i++)
        free(favourites.favourites[i].nameOfFacourite);
    free(favourites.favourites);
}

void addFavouriteMenu()
{
    int code;
    char name[256];

    printf("Give stop code: ");
    fflush(stdout);
    scanf("%d", &code);
    getchar(); // consume newline

    printf("Give name for favourite: ");
    fflush(stdout);
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; // remove newline

    FILE* f = fopen(FAVOURITES_FILE, "a");
    if (!f) 
    {
        perror("fopen");
        return;
    }

    fprintf(f, "%d %s\n", code, name);
    fclose(f);

    printf("Favourite added. Press enter to continue.");
    fflush(stdout);
    getchar();
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

char* getRouteName(int routeCode)
{
    static char routeName[256];
    char command[512];

    // Δημιουργία εντολής curl + jq
    snprintf(command, sizeof(command), "curl -s -A 'Mozilla/5.0' 'http://telematics.oasa.gr/api/?act=getRouteName&p1=%d' | jq -r '.[0].route_departure_eng'", routeCode);

    FILE* fp = popen(command, "r");
    if (fp == NULL) 
    {
        perror("popen failed");
        return NULL;
    }

    if (fgets(routeName, sizeof(routeName), fp) == NULL) 
    {
        pclose(fp);
        return NULL;
    }

    // Αφαίρεση new line αν υπάρχει
    size_t len = strlen(routeName);
    if (len > 0 && routeName[len - 1] == '\n')
    {
        routeName[len - 1] = '\0';
    }
    
    pclose(fp);
    return routeName;
}

