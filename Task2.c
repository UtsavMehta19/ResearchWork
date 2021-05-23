#include <stdio.h>
#include <stdlib.h>

#define WEIGHT_OF_CARS 0.5
#define WEIGHT_OF_STATES 0.5

void inputFunction(int* presentState);
void optimizationFunction(int* presentState, int *history);
int outputFunction(int* presentState);
void trafficLightStatePrinter(int stateOfOneLight);
_Bool checkValidState(float input, int *presentS, int*history);

/*
An array to store valid cases
These are the cases in which we allow two cars to pass
For the single car-pass-case, if that case is reached,
it is reached in a condition such that it will be the most optimal pass, and such a case is treated easily.
*/

const int validCases[12][8] = {
    {1,1}, {[2]=1,1}, {[4]=1,1}, {[6]=1,1}, {1,[2]=1}, {[4]=1,[6]=1}, {[1]=1, [3]=1}, {[5]=1,[7]=1}, {1,[7]=1}, {[2]=1,[5]=1}, {[1]=1,[4]=1}, {[3]=1,[6]=1}};

int main()
{
    //number of time-steps
    int t;
    printf("Enter the number of time-steps: ");
    scanf("%d", &t);

    //array to store present State
    int* presentState = (int *)malloc(8 * sizeof(int));
    int* history = (int *)malloc(8 * sizeof(int));
    int exitCondition = 1;

    printf("\nStart entering the values:\n\n");

/**A loop to go t time-steps**/
    for(int i=0; i<t; ++i)
    {
        inputFunction(presentState);
        optimizationFunction(presentState, history);

        if(i<t-1)
            outputFunction(presentState);
        else
            exitCondition = outputFunction(presentState);
    }

    if (exitCondition == 0)
        return 0;

    printf("\n\n=======================================================================\n");
    printf("Now we start clearing the accumulated traffic\n");
    printf("=======================================================================\n\n");

/**A loop to empty everything after t time-steps**/
    while(1)
    {
        optimizationFunction(presentState, history);
        exitCondition = outputFunction(presentState);
        if (exitCondition == 0)
            break;
    }

    printf("\n\nTASK-2 COMPLETED SUCCESSFULLY\n\n");

    free(presentState);
    free(history);
    return 0;
}

/*********A fxn to accept and modify present array*******/
void inputFunction(int* presentState)
{
    int value;
    for (int i=0; i<8; ++i)
    {
        scanf("%d", &value);
        *(presentState + i) += value;
    }

    printf("\nInitial, updated queue:\n");
    for (int i = 0; i < 8; ++i)
        printf("%d ",*(presentState + i));

    printf("\n");
}

/*****A fxn to print the traffic light conditions for a case****/
void trafficLightStatePrinter(int stateOfOneLight)
{
    switch(stateOfOneLight)
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

/***A fxn to check which traffic light state fits with the one, we choose, Also print on the terminal****/
_Bool checkValidState(float input, int* presentState, int* history)
{
    int i,coloumnCount;
    float check;
    _Bool theRow = 0;
    for(i=0; i<8; ++i)
    {
        check = (WEIGHT_OF_CARS * presentState[i]) - (WEIGHT_OF_STATES * history[i]);
        if(check == input && presentState[i] > 0)
        {
            for(int rowCount=0; rowCount<12; ++rowCount)
            {
                if(validCases[rowCount][i] == 1)
                {
                    for(coloumnCount=0; coloumnCount<8; ++coloumnCount)
                    {
                        if(validCases[rowCount][coloumnCount] == 1 && coloumnCount!=i)
                        {
                            if(*(presentState+coloumnCount)>0)
                                theRow = 1;
                        }
                        if(theRow == 1)
                            break;
                    }
                }
                if(theRow == 1)
                    break;
            }
        }
        if(theRow == 1)
            break;
    }

    //if we get our result, then print on terminal
    if(theRow == 1)
    {
        //modify the Present State
        *(presentState + i) -=1;
        *(history + i) +=1;
        *(presentState + coloumnCount) -=1;
        *(history + coloumnCount) +=1;

        //printing the condition of traffic lights
        printf("\n");
        trafficLightStatePrinter(coloumnCount);
        printf("\n");
        trafficLightStatePrinter(i);
    }
    return theRow;
}

/*******A fxn to give the optimized soln*********/
void optimizationFunction(int* presentState, int* history)
{
    float maxChance= -9999.9999;
    float maxButOne=0.0;
    _Bool found = 0;
    int maxIndex=0, maxButOneIndex=0;

    //a loop to calculate the best possible transfer of state(or to stay in the same state)
    for(int i=0; i<8; ++i)
    {
        float check = (WEIGHT_OF_CARS * presentState[i]) - (WEIGHT_OF_STATES * history[i]);
        if(check >= maxChance && presentState[i] > 0)
        {
            maxButOne = maxChance;
            maxButOneIndex = maxIndex;
            maxChance = check;
            maxIndex = i;
        }
    }

    if(presentState[maxIndex] > 0)
    {
        //check if we can go with max at present case
        found = checkValidState(maxChance, presentState, history);
        if(found == 1)
            return;
    }

    if(presentState[maxButOneIndex] > 0)
    {
        //if not yet, we apply the same to same method for max but one case
        if(presentState[maxButOneIndex] > 0)
            found = checkValidState(maxButOne, presentState, history);

        if(found == 1)
            return;
    }

    //if not  yet, means that we simply decrease the maximum traffic by one
    *(presentState + maxIndex) -= 1;
    *(history + maxIndex) +=1;
    printf("\n");
    trafficLightStatePrinter(maxIndex);
    printf("\n");

    return;
}

/****A fxn to finally print the output and return a condition to terminate****/
int outputFunction(int* presentState)
{
    int exitCondition = 0;
    printf("\n\nFinal queue is:\n");
    for (int i = 0; i < 8; ++i)
    {
        printf("%d ",*(presentState + i));
        if (*(presentState + i) != 0)
            exitCondition += 1;
    }

    printf("\n-----------------------------------------------------------------------\n");

    if (exitCondition != 0)
        return 1;

    return 0;
}