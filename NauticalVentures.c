//Program to store boats and track repairs
//Include headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Define charge rate and max amounts for name, storage for array and length
#define SLIP_RATE 12.50
#define LAND_RATE 14.00
#define TRAILOR_RATE 25.00
#define STORAGE_RATE 11.20
#define MAX_STORAGE 120

#define MAX_STRING 250
#define MAX_LENGTH 100

//Typedef string
typedef char string[MAX_STRING];

//Typedef enum for places the boat can be stored
typedef enum {
    slip,
    land,
    trailor,
    storage
} location;

//Typedef union for extra info the boat will store depending on its location
typedef union {
    int slipNumber;
    char bayLetter;
    string licenseTag;
    int storageNumber;
} info;

//Typedef boat struct
typedef struct {
    string name;
    int length;
    location place;
    info relevantInfo;
    double moneyOwed;
} boatDetails;

//Typedef to create a boat array of pointers to boat details
typedef boatDetails *boats[MAX_STORAGE];

//Malloc function
void * Malloc(size_t Size) {

    void * Memory;

    if ((Memory = malloc(Size)) == NULL) {
        perror("Cannot malloc");
        exit(EXIT_FAILURE);
    } else {
        return(Memory);
    }
}

//Function to convert a location string to a location enum
location locationStringToEnum(char *place) {
//Use srtcasecmp to ignore case, used with a ! because if they match
//str
    if (!strcasecmp(place, "slip")) {
        return slip;
    } else if (!strcasecmp(place, "land")) {
        return land;
    } else if (!strcasecmp(place, "trailor")) {
        return trailor;
    } else if (!strcasecmp(place, "storage")) {
        return storage;
    } else {
        printf("Error: %s is not a valid location for storage.\n", place);
        exit(EXIT_FAILURE);
    }
}

char *locationEnumToString(location place) {
//switch statement to turn location enum into a string
    switch(place) {
        case slip:
            return "slip";
            break;
        case land:
            return "land";
            break;
        case trailor:
            return "trailor";
            break;
        case storage:
            return "storage";
            break;
        default:
            printf("Error: unable to convert place enum back into string (should be impossible to get here)");
            exit(EXIT_FAILURE);
    }
}

//Load in boat data from a csv
void loadBoats(string file, boats *boatTracker, int *boatCounter) {
    
//Declare temporary string that will read in each line
    string csvline;

//Declare token variable for strtok to read in the values
    char *strtokToken;

//Declare an int length so the string that comes out of strtok 
    int length;

//Check to make sure a file was entered
    if (file == NULL) {
        printf("Error: a filename is needed as a command line argument");
        exit(EXIT_FAILURE);
    }

//If a filename is provided, try to open it
    FILE *csvFile = fopen(file, "r");

//Make sure the file was able to be opened
    if (csvFile == NULL) {
        printf("Error: failed to open the file: %s", file);
        exit(EXIT_FAILURE);
    }

//If file was provided and was successfully opened, run a while loop to
//get each line of the csv
    while(fgets(csvline, MAX_STRING, csvFile) != NULL) {
//For each boat read, create a new boatDetails variable and run the Malloc 
//function to allocate memory and error check the allocation
        boatDetails *boatInfo = (boatDetails*)Malloc(sizeof(boatDetails));

//If memory is successfully allocated for the boat, get the name
        strtokToken = strtok(csvline,",");
//Check to make sure name is not more than 127 characters
        if (strlen(strtokToken) > 127) {
            printf("Error: the name of the boat cannot be more than 127 characters.\n");
            return;
        }
//Copy the name into the new boatInfo struct
        strcpy(boatInfo -> name, strtokToken);

//Do the same thing for length, use NULL so strtok picks up where it left off
//in the csv file
        strtokToken = strtok(NULL, ",");

//convert strtokToken (which is currently holding length), to an int
        length = atoi(strtokToken);

//Check to make sure length is less than or equal to 100
        if (length > MAX_LENGTH) {
            printf("Error: boat length %d is above the maximum length allowed.\n", length);
            return;
        }

//Save the length into the new boatInfo struct
        boatInfo -> length = length;

//Do the same thing for place, use NULL so strtok picks up where it left off
//in the csv file
        strtokToken = strtok(NULL, ",");

//Run locationConverter to turn strtokToken (currently holding place) into
//a location enum type to be stored in the new boatInfo struct
        boatInfo -> place = locationStringToEnum(strtokToken);

//Do the same thing for relevantInfo, use NULL so strtok picks up where it left off
//in the csv file
        strtokToken = strtok(NULL, ",");

//Switch statement to use place enum and get info from union typedef
        switch(boatInfo -> place) {
            case slip:
                boatInfo -> relevantInfo.slipNumber = atoi(strtokToken);
                break;
            case land:
                boatInfo -> relevantInfo.bayLetter = toupper(strtokToken[0]);
                break;
            case trailor:
                strcpy(boatInfo -> relevantInfo.licenseTag, strtokToken);
                break;
            case storage:
                boatInfo -> relevantInfo.storageNumber = atoi(strtokToken);
                break;
            default:
                printf("Error: %s is not a valid place for boat storage.",strtokToken);
                exit(EXIT_FAILURE);
        }

//Do the same thing for moneyOwed, use NULL so strtok picks up where it left off
//in the csv file
        strtokToken = strtok(NULL,",");
//Save the moneyOwed into the new boatInfo struct
        boatInfo -> moneyOwed = strtod(strtokToken, NULL);

//Put the new boatInfo struct into the boatTracker
        (*boatTracker)[*boatCounter] = boatInfo;   

//Add one to the boatCounter
        (*boatCounter)++;
    }
//Close the csvFile and run an error check to make sure it was closed successfully
    if (fclose(csvFile) != 0) {
        printf("Error: unable to close the csv file.");
        exit(EXIT_FAILURE);
    }
    
}

//Function to restore boatTracker array of boatDetails structs back into csv file
void storeBoats(string file, boatDetails **boatTracker, int boatCounter) {

//int i for for loop
    int i;

//Attempt to open file in write mode
    FILE *csvFile = fopen(file, "w");

//Make sure the file was able to be opened
    if (csvFile == NULL) {
        printf("Error: failed to open the file: %s", file);
        exit(EXIT_FAILURE);
    }

//For loop to go through boatTracker to write data to csv file
    for(i = 0; i < boatCounter; ++i) {
//Write beginning of boatDetails struct to file
        fprintf(csvFile, "%s,%d,%s,", boatTracker[i] -> name, boatTracker[i] -> length, locationEnumToString(boatTracker[i] -> place));
//Switch statement to use location enum to get the relevant info for the boat and also write in moneyOwed
        switch(boatTracker[i] -> place) {
            case slip:
                fprintf(csvFile,"%d,%.2lf\n", boatTracker[i] -> relevantInfo.slipNumber, boatTracker[i] -> moneyOwed);
                break;
            case land:
                fprintf(csvFile,"%c,%.2lf\n", boatTracker[i] -> relevantInfo.bayLetter, boatTracker[i] -> moneyOwed);
                break;
            case trailor:
                fprintf(csvFile,"%s,%.2lf\n", boatTracker[i] -> relevantInfo.licenseTag, boatTracker[i] -> moneyOwed);
                break;
            case storage:
                fprintf(csvFile,"%d,%.2lf\n", boatTracker[i] -> relevantInfo.storageNumber, boatTracker[i] -> moneyOwed);
                break;
            default:
                printf("Error: %s is not a valid place, saving failed.", locationEnumToString(boatTracker[i] -> place));
//Exit failure here because csv will not save right which would mess up the marina's data
                exit(EXIT_FAILURE);
        }
    }
//Close the csvFile and run an error check to make sure it was closed successfully
    if (fclose(csvFile) != 0) {
        printf("Error: unable to close the csv file.");
        exit(EXIT_FAILURE);
    }
    return;

    }
//Function to be used in qsort to compare two boat names to eachother
int compareBoatNames(void const *A, void const *B) {

//Turn void pointers into a pointer pointer for a boatDetails struct    
    const boatDetails **boatA = (const boatDetails**)A;
    const boatDetails **boatB = (const boatDetails**)B;

//Return the comparison of the names of the two boats using strcasecmp to make boat names case insensitive
    return strcasecmp((*boatA)-> name, (*boatB) -> name);
}

//Function to print out all of the boatDetails structs stored in the boatTracker array
void printInventory(boatDetails **boatTracker, int boatCounter) {
//Int declaration for i for for loop
    int i;

//For loop to print each boatDetails struct with its data
    for (i = 0; i < boatCounter; ++i) {

//Print statement to print name, length and place (done by calling enum to string method for location enum)
        printf("%-21s %d' %7s ", boatTracker[i] -> name, boatTracker[i] -> length, locationEnumToString(boatTracker[i] -> place));

//Switch statement to find out what relevant info needs to be printed out for a give boatDetails struct
        switch(boatTracker[i] -> place) {
            case slip:
                printf("  #%3d ", boatTracker[i] -> relevantInfo.slipNumber);
                break;
            case land:
                printf("%6c ", boatTracker[i] -> relevantInfo.bayLetter);
                break;
            case trailor:
                printf("%6s ", boatTracker[i] -> relevantInfo.licenseTag);
                break;
            case storage:
                printf("  #%3d ", boatTracker[i] -> relevantInfo.storageNumber);
                break;
            default:
                printf("\nError: unable to print out relevant information due to enum mismatch");
        }

//Print money owed
        printf("  Owes $%7.2lf\n", boatTracker[i] -> moneyOwed);

    }
}

void addNewBoat(boatDetails **boatTracker, int *boatCounter) {
//String to read in info as csv line
    string readInInfo;

//Declare token variable for strtok to read in the from the user input
    char *strtokToken;

//Declare an int length so the string that comes out of strtok 
//can be converted to a double and checked to be at or below 
//100 before being entered into the new struct for boatDetails
    int length;

//Malloc boatTracker to have space dynmically allocated for a new boatDetails struct
    boatDetails *boatInfo = (boatDetails*)Malloc(sizeof(boatDetails));

//Make sure boatTracker has enough space to add a new boatDetails struct
    if(*boatCounter == MAX_STORAGE) {
        printf("Error: cannot add boat as max capacity has been reached.\n");
        return;
    }

//Print out message to let user know to enter in csv format
    printf("Please enter the boat data in CSV format                 : ");
    getchar();

//Scan in the csv format for a new boat
    fgets(readInInfo, sizeof(readInInfo), stdin);

//Get the name from the readInInfo input from the user
    strtokToken = strtok(readInInfo, ",");

//Check to make sure name is not more than 127 characters
    if (strlen(strtokToken) > 127) {
        printf("Error: the name of the boat cannot be more than 127 characters.\n");
        return;
    }

//Copy strtokToken(currently name) into boatInfo struct that will be added to boatTracker
    strcpy(boatInfo -> name, strtokToken);

//Get the length from the readInInfo input from the user, use NULL so 
//strtok picks up where it left off in the user input formatted as a csv
    strtokToken = strtok(NULL, ",");

//convert strtokToken (which is currently holding length), to an int
    length = atoi(strtokToken);

//Check to make sure length is less than or equal to 100
    if (length > MAX_LENGTH) {
        printf("Error: boat length %d is above the maximum length allowed.\n", length);
        return;
    }

//Save the length into the new boatInfo struct
    boatInfo -> length = length;

//Do the same thing for place, use NULL so strtok picks up where it left off
//in the csv file
    strtokToken = strtok(NULL, ",");
        
//Run locationConverter to turn strtokToken (currently holding place) into
//a location enum type to be stored in the new boatInfo struct
    boatInfo -> place = locationStringToEnum(strtokToken);

//Do the same thing for relevantInfo, use NULL so strtok picks up where it left off
//in the csv file
    strtokToken = strtok(NULL, ",");

//Switch statement to use place enum and get info from union typedef
    switch(boatInfo -> place) {
        case slip:
            boatInfo -> relevantInfo.slipNumber = atoi(strtokToken);
            break;
        case land:
            boatInfo -> relevantInfo.bayLetter = toupper(strtokToken[0]);
            break;
        case trailor:
            strcpy(boatInfo -> relevantInfo.licenseTag, strtokToken);
            break;
        case storage:
            boatInfo -> relevantInfo.storageNumber = atoi(strtokToken);
            break;
        default:
            printf("Error: %s is not a valid place for boat storage.",strtokToken);
            exit(EXIT_FAILURE);
    }

//Do the same thing for moneyOwed, use NULL so strtok picks up where it left off
//in the csv file
        strtokToken = strtok(NULL,",");
//Save the moneyOwed into the new boatInfo struct
        boatInfo -> moneyOwed = strtod(strtokToken, NULL);

//Put the new boatInfo struct into the boatTracker
        boatTracker[*boatCounter] = boatInfo; 

//Add one to the boatCounter
        (*boatCounter)++;

}

void removeBoat(boatDetails **boatTracker, int *boatCounter) {
//String to hold the name of the boat we're going to remove
    string boatRemoveName;

//int i and j for for loops
    int i;
    int j;

//Get user input for boat name, use %[^\n] to get boat name with spaces
//where scanf stops reading once a newline character is read
    printf("Please enter the boat name                               : ");
    scanf(" %[^\n]", boatRemoveName);

//Cycle through all the items in boatTracker array until boat to remove is found or we reach the end
    for(i = 0; i < *boatCounter; ++i) {
//Check if one of the boatTracker structs name variables is the same as the name to remove
        if(strcasecmp(boatTracker[i] -> name, boatRemoveName) == 0) {
//If a match is found, free the boatTracker struct at the index
            free(boatTracker[i]);
//Once memory for the struct is freed, move down all of the boat structs after the freed boat
            for(j = i; j < *boatCounter; ++j) {
                boatTracker[j] = boatTracker[j+1];
            }
//Decrease the size of the boatDetails array by one
            (*boatCounter)--;
//Return so we don't brint warning message (only happens if boatRemoveName is found)
            return;
        }
    }
//If the boatRemoveName is not found, print out error message
    printf("No boat with that name\n");
    return;
}

//Method to accept a payment on one of the boatDetails structs
void acceptPayment(boatDetails **boatTracker, int boatCounter) {
//String to hold the name of the boat to have its payment updated
    string boatPaymentName;

//double to hold amount to be paid
    double paymentAmount;

//int i for for loop
    int i;

//Get user input for boat name, use %[^\n] to get boat name with spaces
//where scanf stops reading once a newline character is read
    printf("Please enter the boat name                               : ");
    scanf(" %[^\n]", boatPaymentName);

//Cycle through boatTracker until boatPayment name is found or the end is reached 
    for(i = 0; i < boatCounter; ++i) {
//Check if one of the boatTracker structs name variables is the same as the name to remove
        if(strcasecmp(boatTracker[i] -> name, boatPaymentName) == 0) {
//Get user input for payment amount, use %[^\n] to get boat name with spaces
//where scanf stops reading once a newline character is read
    printf("Please enter the amount to be paid                       : ");
    scanf("%lf", &paymentAmount);
//If found, check to make sure paymentAmount is <= moneyOwed, if it is not print an error message
            if (paymentAmount > boatTracker[i] -> moneyOwed) {
                printf("That is more than the amount owed, $%.2lf\n", boatTracker[i] -> moneyOwed);
                return;
            }
 //Update the moneyOwed on the boat in the boatDetails struct
            boatTracker[i] -> moneyOwed -= paymentAmount;
            return;           
        }
    }
//If the boatPaymentName is not found, print out error message
    printf("No boat with that name\n");
    return;
}

//Function to update the moneyOwed on each boat when a new month starts  
void updateAmountOwed(boatDetails **boatTracker, int boatCounter) {
//Initialize an int length to be used in the for loop to be multiplied by its location rate
    int length;

//int i for for loop
    int i;

//For loop to update each boats amount owed since it is a new month
    for (i = 0; i < boatCounter; ++i) {
        length = boatTracker[i] -> length;
//Switch statement to get the boats location so the rate can be updated
        switch(boatTracker[i] -> place) {
            case slip:
                boatTracker[i] -> moneyOwed += (length * SLIP_RATE);
                break;
            case land:
                boatTracker[i] -> moneyOwed += (length * LAND_RATE);
                break;
            case trailor:
                boatTracker[i] -> moneyOwed += (length * TRAILOR_RATE);
                break;
            case storage:
                boatTracker[i] -> moneyOwed += length * STORAGE_RATE;
                break;
            default:
                printf("Error: could not retrieve location of boat (should be impossible to get here)");
                exit(EXIT_FAILURE);
        }
    } 
}

//Main function with argc (number of arguments passed in including program) and argv
//to take in arguments which will be the filename of the csv
int main(int argc, char *argv[]) {

//Initialize array of pointers to boatDetail structs
    boats boatTracker;
//Initialize counter to keep track of the number of boats
    int boatCounter = 0;
//char for user choice for menu, initialize so it can be used as a while condition
//Once while loop starts this initialization will be overwritten
    char userChoice = 'T';
//int i for for loop
    int i;

//Check to make sure we have the csv file, if argc is not 2 then we either don't
//have enough or have too many files
    if (argc != 2) {
        printf("Error: please provide one file as a command line arguement.\n");
        exit(EXIT_FAILURE);
    }

//Load in csv file 
    loadBoats(argv[1], &boatTracker, &boatCounter);

//If file successfully loaded, qsort it to get the array in alphabetical order
    qsort(boatTracker, boatCounter, sizeof(boatDetails*), compareBoatNames);

//Print welcome message
    printf("\nWelcome to the Boat Management System\n-------------------------------------\n");
    
//Loop for menu
    while(userChoice != 'X') {
//Print out user options
        printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
//Get user choice for menu
        scanf(" %c", &userChoice);
//Set userChoice to upper case so the input can be case insensitive
        userChoice = toupper(userChoice);

        switch(userChoice) {
            case 'I':
//Run printInventory function to print all boatDetails struct variables
                printInventory(boatTracker, boatCounter);
                break;
            case 'A':
//Run addNewBoat to add a new boatDetails struct to the boatTracker
                addNewBoat(boatTracker, &boatCounter);
//Sort the new boatTracker array now that a new boatDetails struct has been added
                qsort(boatTracker, boatCounter, sizeof(boatDetails*), compareBoatNames);
                break;
            case 'R':
//Run removeBoat function to remove a boat from the array of boatDetail structs
                removeBoat(boatTracker, &boatCounter);
                break;
            case 'P':
//Run acceptPayment function to accept payment on a boat
                acceptPayment(boatTracker, boatCounter);
                break;
            case 'M':
//Run updateAmountOwed function to update the amount owed on all boats when a new month starts
                updateAmountOwed(boatTracker, boatCounter);
                break;
            case 'X':
//Leads to switch statement and while loop being exited
                printf("\nExiting the Boat Management System\n\n");
                break;
            default:
                printf("Invalid option %c\n", userChoice);
                break;
        }
    }

//Run storeBoats to save the boatDetails updated structs back into the csv file
    storeBoats(argv[1], boatTracker, boatCounter);

//For loop to free the allocated memory for the boatTracker array
    for(i = 0; i < boatCounter; ++i) {
        free(boatTracker[i]);
    }

//Exit the program 
    return(EXIT_SUCCESS);

}




