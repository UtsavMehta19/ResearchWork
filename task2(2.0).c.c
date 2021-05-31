#include <stdio.h>
#include <stdlib.h>

#define WEIGHT_OF_CARS 0.5
#define WEIGHT_OF_STATES 0.5

int optimizationFunction(int previousState);
int outputFunction();
void trafficLightStatePrinter(int stateOfOneLight);

const int validCases[12][8] = {
    {1,1}, {[2]=1,1}, {[4]=1,1}, {[6]=1,1}, {1,[2]=1}, {[4]=1,[6]=1}, {[1]=1, [3]=1}, {[5]=1,[7]=1}, {1,[7]=1}, {[2]=1,[5]=1}, {[1]=1,[4]=1}, {[3]=1,[6]=1}};

int presentCars[8] = {0};
int history[8] = {0};

int main()
{
    int t;
    printf("Enter the number of time-steps: ");
    scanf("%d", &t);
    int exitCondition = 1;
    int newState=0, previousState=0;
    printf("\nStart entering the values:\n\n");

/***A loop to go t time-steps***/
    for(int i=0; i<t; ++i)
    {
        int value;
        for (int i=0; i<8; ++i)
        {
            scanf("%d", &value);
            presentCars[i] += value;
        }

        printf("\nInitial, updated queue:\n");
        for (int i = 0; i < 8; ++i)
            printf("%d ",presentCars[i]);

        printf("\n");
        newState = optimizationFunction(previousState);

        if(i<t-1)
            outputFunction();
        else
            exitCondition = outputFunction();
        previousState = newState;
    }

    if (exitCondition == 0)
        return 0;

    printf("\n\n==============================================\nNow we start clearing the accumulated traffic\n==============================================\n\n");

/***A loop to clear the traffic after t time-steps***/
    while(1)
    {
        newState = optimizationFunction(previousState);
        previousState = newState;
        exitCondition = outputFunction();
        if (exitCondition == 0)
            break;
    }

    printf("\n\nTASK-2 COMPLETED SUCCESSFULLY\n\n");
    return 0;
}

/**************A fxn to print the traffic light conditions for a case*************/
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

/**********************A fxn to give the optimized soln**************************/
int optimizationFunction(int previousState)
{
    float maxChance= -9999.9999;
    float maxButOne=0.0;
    int maxIndex=0;

    //a loop to calculate the best possible transfer of state(or to stay in the same state)
    for(int i=0; i<8; ++i)
    {
        float check = (WEIGHT_OF_CARS * presentCars[i]) - (WEIGHT_OF_STATES * history[i]);
        if(check >= maxChance && presentCars[i] > 0)
        {
            maxButOne = maxChance;
            maxChance = check;
            maxIndex = i;
        }
    }

    int newState = 0;
    for(int x=0; x<2; ++x)
    {
        for(int a=0; a<8; ++a)
        {
            if((WEIGHT_OF_CARS * presentCars[a]) - (WEIGHT_OF_STATES * history[a]) == maxChance && presentCars[a] > 0)
            {
                switch(a)
                {
                    case 0:
                        if(presentCars[1] > 0)
                            newState = 1;
                        else if(presentCars[2] > 0)
                            newState = 5;
                        else if(presentCars[7] > 0)
                            newState = 9;
                        break;
                    case 1:
                        if(presentCars[0] > 0)
                            newState = 1;
                        else if(presentCars[3] > 0)
                            newState = 7;
                        else if(presentCars[4] > 0)
                            newState = 11;
                        break;
                    case 2:
                        if(presentCars[3] > 0)
                            newState = 2;
                        else if(presentCars[0] > 0)
                            newState = 5;
                        else if(presentCars[5] > 0)
                            newState = 10;
                        else
                            newState=0;
                        break;
                    case 3:
                        if(presentCars[2] > 0)
                            newState = 2;
                        else if(presentCars[1] > 0)
                            newState = 7;
                        else if(presentCars[6] > 0)
                            newState = 12;
                        break;
                    case 4:
                        if(presentCars[5] > 0)
                            newState = 3;
                        else if(presentCars[6] > 0)
                            newState = 6;
                        else if(presentCars[1] > 0)
                            newState = 11;
                        break;
                    case 5:
                        if(presentCars[4] > 0)
                            newState = 3;
                        else if(presentCars[7] > 0)
                            newState = 8;
                        else if(presentCars[2] > 0)
                            newState = 10;
                        break;
                    case 6:
                        if(presentCars[7] > 0)
                            newState = 4;
                        else if(presentCars[4] > 0)
                            newState = 6;
                        else if(presentCars[3] > 0)
                            newState = 12;
                        break;
                    case 7:
                        if(presentCars[6] > 0)
                            newState = 4;
                        else if(presentCars[5] > 0)
                            newState = 8;
                        else if(presentCars[0] > 0)
                            newState = 9;
                        break;
                }
            }
        }
        if(newState!=0 && newState!=previousState)
            break;
        else
            maxChance = maxButOne;
    }
    if(newState != 0)
    {
        for(int j=0; j<8; ++j)
        {
            if(validCases[newState - 1][j] == 1)
            {
                printf("\n");
                trafficLightStatePrinter(j);
                presentCars[j] -= 1;
                history[j] += 1;
            }
        }
        return newState;
    }

    //if not  yet, means that we simply decrease the maximum traffic by one
    newState = maxIndex + 13;
    presentCars[maxIndex] -= 1;
    history[maxIndex] +=1;
    printf("\n");
    trafficLightStatePrinter(maxIndex);

    return newState;
}

/***********A fxn to finally print the output and return a condition to terminate*********/
int outputFunction()
{
    int exitCondition = 0;
    printf("\n\nFinal queue is:\n");
    for (int i = 0; i < 8; ++i)
    {
        printf("%d ",presentCars[i]);
        if (presentCars[i] != 0)
            exitCondition += 1;
    }

    printf("\n-----------------------------------------------------------------------\n");

    if (exitCondition != 0)
        return 1;

    return 0;
}
