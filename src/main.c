#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>
#define PIN 17
// A University library wants a system that helps staff monitor the number of students occupying a study room. Each study room has a maximum capacity. The library needs a way to track students entering and leaving and to identify when the room has reached its occupancy limit.
// The team will develop a Raspberry Pi- based occupancy tracking system that maintains the current occupancy count and provides a visual indication when the room is full.
// Expected Product Functionality.
// Allow the user to record when a person enters or exits a study room.
// Maintain an accurate count of current occupants.
// Prevent occupancy from becoming negative.
// Prevent the count from exceeding the maximum room capacity.
// Display the current occupancy and room status.
// Save occupancy events to a file.
// Use LED connected to the Raspberry Pi to indicate when the room is full.
// Support a change to the maximum room capacity.

int light(int ledOnOff) {
    if (ledOnOff == 1) {
        gpioWrite(PIN,1); // LED ON
    } else if (ledOnOff == 0) {
        gpioWrite(PIN,0); // LED OFF
    } else {
        printf("Invalid input for LED control. Use 1 for ON and 0 for OFF.\n");
        return -1; // Return an error code for invalid input
    }
    return 0;
}

// does the maximum capacity need to be changeable with user input? Or is it a hardcoded value?
// *UNANSWERED QUESTION* Do we need to check for if someone gives a bad input in scanf and re-issue a new scanf prompt?
// save the occupancy events to a file.
int main() {
    int occupancy = 0;
    int maxCapacity = 1;
    int userInput = 0;

    printf("Enter the maximum capacity of the study room: ");
    scanf("%d", &maxCapacity); // Read the maximum capacity from user input

    int gpioStatus = gpioInitialise();
    if (gpioStatus < 0) {
        fprintf(stderr, "Failed to initialise pigpio.\n");
        return 1;
    }
    printf("GPIO initialised successfully.\n");
    
    gpioSetMode(PIN,PI_OUTPUT); // configures the GPIO pin as an output
    FILE* fptr;
    // Opening the file in append mode
    fptr = fopen("occupancy.txt", "a");

    // checking if the file is opened successfully
    if (fptr == NULL) {
        perror("Could not open occupancy.txt");
        return 1;
    }
    // Ensure the LED is off at the start
    light(0);
    // While statement to continuously check for user input
    while (1) {
        printf("Press 1 when a student enters. Press 0 when a student leaves. Press any other integer key to exit.\n");
        scanf("%d", &userInput);

        // Prevents non-valid user input
        if (userInput != 0 && userInput != 1) {
            printf("Exiting the program.\n");
            break; // Exit the loop if the input is not 0 or 1
        } else if (occupancy == maxCapacity && userInput == 1) {
            printf("Room is full. Cannot allow more students to enter.\n");
            fprintf(fptr, "Room is full. Cannot allow more students to enter.\n");
            fflush(fptr);
        } else if (occupancy == 0 && userInput == 0) {
            printf("Room is empty. Cannot allow students to leave.\n");
            fprintf(fptr, "Room is empty. Cannot allow students to leave.\n");
            fflush(fptr);
        } else {
            if (userInput == 1) {
                occupancy++;
                if (occupancy == maxCapacity) {
                    light(1); // Call the light function to turn on the LED
                }
                printf("Student entered. Current occupancy: %d\n", occupancy);
                fprintf(fptr, "Student entered. Current occupancy: %d\n",
                    occupancy);
                fflush(fptr);
            } else if (userInput == 0) {
                if (occupancy == maxCapacity) {
                    light(0); // Call the light function to turn off the LED
                }
                occupancy--;
                printf("Student left. Current occupancy: %d\n", occupancy);
                fprintf(fptr, "Student left. Current occupancy: %d\n",
                    occupancy);
                fflush(fptr);
            }

        }
        // printf("Current occupancy: %d\n", occupancy);
        // fprintf(fptr, "Current occupancy: %d\n", occupancy);
    }
    fclose(fptr);
    gpioTerminate();
    return 0;
}