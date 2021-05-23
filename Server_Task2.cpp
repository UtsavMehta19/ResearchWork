#include <stdio.h>
#include <iostream>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define PORT1 9090
#define WEIGHT_OF_CARS 0.5
#define WEIGHT_OF_STATES 0.5

using namespace std;

void optimizationFunction(int* presentState, int* history);
int outputFunction(int* presentState);
void trafficLightStatePrinter(int stateOfOneLight);
bool checkValidState(float input, int* presentS, int* history);

/***
An array to store valid cases
These are the cases in which we allow two cars to pass
For the single car-pass-case, if that case is reached,
it is reached in a condition such that it will be the most optimal pass,
and such a case is treated easily.
***/

const int validCases[12][8] = {
    {1,1}, {0,0,1,1}, {0,0,0,0,1,1}, {0,0,0,0,0,0,1,1}, {1,0,1}, {0,0,0,0,1,0,1}, {0,1,0,1}, {0,0,0,0,0,1,0,1}, {1,0,0,0,0,0,0,1}, {0,0,1,0,0,1}, {0,1,0,0,1}, {0,0,0,1,0,0,1} };

int main()
{
    struct sockaddr_in structure;
    struct fd_set readVar;  //we just read the input from server->client or oppostite way
    int structureLen = sizeof(structure);
    char storage[50] = { 0 };

    //first of all, we initialize WSA variables that provide an environment to create socket programs on windows
    WSADATA wsa_var;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_var) < 0)
    {
        cout << "Error in wsa variable." << endl;
        exit(EXIT_FAILURE);
    }


    /***FIRST, WE INITIALIZE THE SOCKET AND STORE THE INTEGER(SOCKET DESCRIPTOR) RETURNED***/
    int mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket < 0)
    {
        cout << "Error in socket descriptor" << endl;
        exit(EXIT_FAILURE);
    }


    /***NOW WE SET UP THE SOCKADDR STRUCTURE***/
    structure.sin_addr.s_addr = inet_addr("192.168.1.2"); //my ip address for home wifi
    structure.sin_port = htons(PORT1);
    structure.sin_family = AF_INET;
    memset(&(structure.sin_zero), 0, 8);

    /***NOW, WE BIND OUR SOCKET TO A LOCAL PORT***/
    int output = bind(mainSocket, (sockaddr*)&structure, sizeof(sockaddr));
    //bind is a function that combines our socket descriptor output and sockaddr structure element
    if (output < 0)
    {
        cout << "Failed to bind" << endl;
        exit(EXIT_FAILURE);
    }

    /***START LISTENING THE CLIENT REQUESTS***/
    output = listen(mainSocket, 3); 
    if (output < 0)
    {
        cout << "Failed to listen incoming calls from port" << endl;
        exit(EXIT_FAILURE);
    }


    FD_ZERO(&readVar);
    FD_SET(mainSocket, &readVar);

    int iOptVal = 0;
    int iOptLen = sizeof(int);
    bool bOptVal = TRUE;
    int bOptLen = sizeof(BOOL);

    int iResult = getsockopt(mainSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&iOptVal, &iOptLen);

    /***FINALLY, CREATING A LOOP FOR CONTINUOUS CLIENT-SERVER INFO EXCHANGE***/
    int recievedMessage;

    output = accept(mainSocket, (struct sockaddr*)&structure, &structureLen);
    if (output < 0)
    {
        perror("Error in accept call");
        exit(EXIT_FAILURE);
    }

    cout << "THE SERVER IS RUNNING SUCCESSFULLY" << endl;

    int timesteps;

    //this chunk is to exchange time-steps
    send(output, "Number of time-steps:", strlen("Number of time-steps:"), 0);
    recievedMessage = recv(output, storage, 50, 0);
    timesteps = storage[0] - '0';
    printf("\nClient entered number of timesteps as: %d\n\n", timesteps);

    //the "queue"(s)
    int* presentState = (int*)malloc(8 * sizeof(int));
    int* history = (int*)malloc(8 * sizeof(int));

    for (int i = 0; i < 8; ++i)
    {
        *(presentState + i) = 0;
    }
    int exitCondition = 1;

    //this chunk is to exchange starting instruction
    send(output, "Start entering the values(without spaces):", strlen("Start entering the values(without spaces):"), 0);
    printf("\nAsked the client to start entering the values\n\n");
 
    /*****************A loop to go t time-steps********************/


    for (int i = 0; i < timesteps; ++i)
    {
        /******this chunk accepts the number of incoming cars and modifies the queue******/
        char* incoming = (char*)malloc(8 * sizeof(char));
        for (int i = 0; i < 8; ++i)
        {
            *(incoming + i) = 0;
        }
        recv(output, incoming, 8, 0);

        for (int i = 0; i < 8; i ++)
        {
            if (*(incoming + i) == '0') 
                *(presentState + i) += 0;
            if (*(incoming + i) == '1') 
                *(presentState + i) += 1;
         }
        
        printf("\nInitial, updated queue:\n");
        for (int i = 0; i < 8; ++i)
        {
            printf("%d", *(presentState + i));
        }
        printf("\n");
        /******the previous chunk ends here*****/

        optimizationFunction(presentState, history);
        if (i < timesteps - 1)
        {
            outputFunction(presentState);
        }
        else
        {
            exitCondition = outputFunction(presentState);
        }

        free(incoming);
        incoming = NULL;
    }

    if (exitCondition == 0)
    {
        return 0;
    }
    printf("\n\n=======================================================================\n");
    printf("Now we start clearing the accumulated traffic\n");
    printf("=======================================================================\n");

    /***A loop to empty everything after t time-steps***/

    while (1)
    {
        optimizationFunction(presentState, history);
        exitCondition = outputFunction(presentState);
        if (exitCondition == 0)
        {
            break;
        }
    }

    printf("\n\nTASK-2 COMPLETED SUCCESSFULLY\n\n");

    free(presentState);

    return 0;
}

/************************************A fxn to print the traffic light conditions for a case**************************/

void trafficLightStatePrinter(int stateOfOneLight)
{
    switch (stateOfOneLight)
    {
    case 0:
        printf("A - Green for Straight");
        break;
    case 1:
        printf("A - Green for Right");
        break;
    case 2:
        printf("B - Green for Straight");
        break;
    case 3:
        printf("B - Green for Right");
        break;
    case 4:
        printf("C - Green for Straight");
        break;
    case 5:
        printf("C - Green for Right");
        break;
    case 6:
        printf("D - Green for Straight");
        break;
    case 7:
        printf("D - Green for Right");
        break;
    }
    return;
}

/*******************************************A fxn to give the optimized soln*****************************************/

void optimizationFunction(int* presentState, int* history)
{
    float maxChance = -9999.9999;
    float maxButOne = 0.0;
    int found = 0;
    int maxIndex = 0, maxButOneIndex = 0;

    //a loop to calculate the best possible transfer of state(or to stay in the same state)
    for (int i = 0; i < 8; ++i)
    {
        float check = (WEIGHT_OF_CARS * presentState[i]) - (WEIGHT_OF_STATES * history[i]);
        if (check >= maxChance && presentState[i] > 0)
        {
            maxButOne = maxChance;
            maxButOneIndex = maxIndex;
            maxChance = check;
            maxIndex = i;
        }
    }

    if (presentState[maxIndex] > 0)
    {
        //check if we can go with max at present case
        found = checkValidState(maxChance, presentState, history);
        if (found == 1)
            return;
    }

    if (presentState[maxButOneIndex] > 0)
    {
        //if not yet, we apply the same to same method for max but one case
        if (presentState[maxButOneIndex] > 0)
            found = checkValidState(maxButOne, presentState, history);

        if (found == 1)
            return;
    }

    //if not  yet, means that we simply decrease the maximum traffic by one
    *(presentState + maxIndex) -= 1;
    *(history + maxIndex) += 1;
    printf("\n");
    trafficLightStatePrinter(maxIndex);
    printf("\n");

    return;
}

bool checkValidState(float input, int* presentState, int* history)
{
    int i, coloumnCount;
    float check;
    bool theRow = 0;
    for (i = 0; i < 8; ++i)
    {
        check = (WEIGHT_OF_CARS * presentState[i]) - (WEIGHT_OF_STATES * history[i]);
        if (check == input && presentState[i] > 0)
        {
            for (int rowCount = 0; rowCount < 12; ++rowCount)
            {
                if (validCases[rowCount][i] == 1)
                {
                    for (coloumnCount = 0; coloumnCount < 8; ++coloumnCount)
                    {
                        if (validCases[rowCount][coloumnCount] == 1 && coloumnCount != i)
                        {
                            if (*(presentState + coloumnCount) > 0)
                                theRow = 1;
                        }
                        if (theRow == 1)
                            break;
                    }
                }
                if (theRow == 1)
                    break;
            }
        }
        if (theRow == 1)
            break;
    }

    //if we get our result, then print on terminal
    if (theRow == 1)
    {
        //modify the Present State
        *(presentState + i) -= 1;
        *(history + i) += 1;
        *(presentState + coloumnCount) -= 1;
        *(history + coloumnCount) += 1;

        //printing the condition of traffic lights
        printf("\n");
        trafficLightStatePrinter(coloumnCount);
        printf("\n");
        trafficLightStatePrinter(i);
    }
    return theRow;
}

/***************************A fxn to finally print the output and return a condition to terminate*********************/

int outputFunction(int* presentState)
{
    int exitCondition = 0;
    printf("\n\nFinal queue is:\n");
    for (int i = 0; i < 8; ++i)
    {
        printf("%d", *(presentState + i));
        if (*(presentState + i) != 0)
            exitCondition += 1;
    }

    printf("\n-----------------------------------------------------------------------\n");

    if (exitCondition != 0)
    {
        return 1;
    }

    return 0;
}
