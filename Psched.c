/*
 Jason Chu, jc3237
 Psched.c is a program that tries to provide the lowest maximum workload to a processor scheduling problem. It uses a
 backtracking algorithm so each possible move is checked, but a list of heuristics is applied to shorten the possible moves.
 In order to use backtracking and apply all the heuristics, several ancillary functions were written to shorten the code.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


//returns the minimum of two numbers
static int min(int x, int y)
{
    if (x < y)
        return x;
    else
        return y;
}

//returns the maximum int in an array of ints with nProcs elements
static int findmax(int nProcs, int schedule[])
{
    int max = 0;
    for (int i=0; i<nProcs; i++)
    {
        if (schedule[i]>max)
        {
            max = schedule[i];
        }
    }
    return max;
}


static void bubblesort(int nums[], int size)
{
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size - 1; y++)
        {
            if (nums[y] < nums[y+1])
            {
                int temp = nums[y+1];
                nums[y+1] = nums[y];
                nums[y] = temp;
            }
        }
    }
}

// finds the sum of numbers in an array
static int sumArray(int nums[], int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum+=nums[i];
    }
    return sum;
}

// division that rounds up the decimal
static int roundUpDivision(int dividend, int divisor)
{
    return ((dividend-1)/divisor) + 1;
}


// backtracking algorithm
static int backtracking(int nProcs, int runtimes[], int numTimes, int runindex, int schedule[], int bestload, int lowbound, int lastProc)
{
    // all tasks have been assigned
    int currentmax = findmax(nProcs, schedule);
    if (runindex == numTimes)
    {
        if (currentmax < bestload)
        {
            return currentmax;
        }
        else
        {
            return bestload;
        }
    }
    //Heuristic B: stop searching if a complete assignment with lowest possible workload is ever found.
    if (bestload == lowbound)
    {
        
        return bestload;
    }
    
    //iterate through each processor
    for (int i = 0; i < nProcs && lowbound != bestload; i++)
    {
        // returns true if at least one previous processor had the same workload; false if none had the same workload
        bool sameload = false;
        for (int j = 0; j < i; j++)
        {
            if (schedule[j] == schedule[i])
            {
                sameload = true;
            }
        }
        
        if (runindex != 0 && runtimes[runindex] == runtimes[runindex-1] && lastProc >= 0 && i < lastProc)
        {
            continue;
        }
        //Heuristic D: Not assign a task to a processor with the same (current) workload as a lower-numbered processor.
        if (!sameload)
        {
            // Heuristic C: ignore any partial assignment that makes the workload for some processor at least as large as smallest max load
            if (schedule[i] + runtimes[runindex] < bestload)
            {
                schedule[i] = schedule[i] + runtimes[runindex];
                
                /* Keep track of the smallest maximum workload found previously (initially this is the workload for the -lwd assignment
                 * as noted above) and ignore any partial assignment that makes the workload for some processor at least this large.*/
                bool morethanbest = false;
                for (int j = 0; j < nProcs; j++) {
                    if (schedule[j] > bestload) {
                        morethanbest = true;
                    }
                }
                if (morethanbest != true) {
                    bestload = min(bestload, backtracking(nProcs, runtimes, numTimes, runindex + 1, schedule, bestload, lowbound, i));
                }
                
                schedule[i] = schedule[i] - runtimes[runindex];
                
                for (int j = nProcs-1; j >= 0; j--)
                {
                    if (schedule[j] >= bestload)
                    {
                        return bestload;
                        
                    }
                }
                continue;
            }
            
        }
    }
    return bestload;
    
}

// function call to backtracking
static int backtrackingcall(int nProcs, int runtimes[], int numTimes, int runindex, int schedule[], int bestload, int lowbound, int lastProc)
{
    /*Heuristic A: Always pick an unassigned task with the longest run-time to assign next;
     or, equivalently, sort the tasks in decreasing order of run-time and then
     assign them in sequence.*/
    bubblesort(runtimes, numTimes);
    return backtracking(nProcs, runtimes, numTimes, runindex, schedule, bestload, lowbound, lastProc);
}

// handling of -lw flag; assign each to a processor that has the least workload at the time of the assignment.
static int leastwork(int nProcs, int numTimes, int runtimes[], int schedule[])
{
    /*for (int i = 0; i < numTimes; i++)
     {
     printf("%d\n", runtimes[i]);
     }*/
    for (int i = 0; i < numTimes; i++)
    {
        int lowindex = 0;
        for (int j = 0; j < nProcs; j++)
        {
            if (schedule[j] < schedule[lowindex])
            {
                lowindex = j;
            }
        }
        schedule[lowindex] += runtimes[i];
    }
    return findmax(nProcs, schedule);
}

// handling of -lwd flag; sorts by decreasing order and then calls leastwork
static int leastworksort(int nProcs, int numTimes, int runtimes[], int schedule[])
{
    bubblesort(runtimes, numTimes);
    return leastwork(nProcs, numTimes, runtimes, schedule);
}

// handling of -bw flag; assign each to a processor that has the least current workload, unless this assignment would NOT
// increase the maximum current workload, in which case assign that task to the busiest processor with this property
static int bestwork(int nProcs, int numTimes, int runtimes[], int schedule[])
{
    /*for (int i = 0; i < numTimes; i++)
     {
     printf("%d\n", runtimes[i]);
     }*/
    
    for (int i = 0; i < numTimes; i++)
    {
        bool wouldIncrease = true;
        int max = findmax(nProcs, schedule);
        
        for (int j = 0; j < nProcs; j++)
        {
            if ((schedule[j]+runtimes[i]) < max)
            {
                wouldIncrease = false;
            }
        }
        // if adding the runtime does guarantee an increase in the max workload (that is, adding the runtime to
        // every processor produces a higher workload) add to lowest
        if (wouldIncrease)
        {
            int lowindex = 0;
            for (int j = 0; j < nProcs; j++)
            {
                if (schedule[j] < schedule[lowindex])
                {
                    lowindex = j;
                }
            }
            //printf("ADD: %d to P(%d)\n", runtimes[i], lowindex);
            schedule[lowindex] += runtimes[i];
        }
        // if adding the runtime does not gaurantee an increase in the max workload, then find the processor that has the highest
        // current workload but adding the runtime will not increase it past the max workload
        else
        {
            int high = -1;
            int index;
            for (int j = 0; j < nProcs; j++)
            {
                int sum = schedule[j] + runtimes[i];
                if (sum <= max && schedule[j] > high)
                {
                    high = schedule[j];
                    index = j;
                }
            }
            schedule[index] += runtimes[i];
        }
    }
    return findmax(nProcs, schedule);
}

// handling of -bwd flag; sorts by decreasing order and then calls bestwork
static int bestworksort(int nProcs, int numTimes, int runtimes[], int schedule[])
{
    bubblesort(runtimes, numTimes);
    return bestwork(nProcs, numTimes, runtimes, schedule);
}


int main(int argc, char * argv[])
{
    
    //nProcs is number of processors
    int nProcs = atoi(argv[1]);
    
    if (nProcs > 0)
    {
        //schedule is an array of nProcs number of zeros
        int schedule[nProcs];
        int numTimes = 0;
        //finds the number of runtimes
        for (int i = 2; i<argc; i++)
        {
            if (argv[i][0] != '-')
            {
                numTimes++;
            }
        }
        //creating array of the runtimes
        int runtimes[numTimes];
        //printf("numtimes: %d\n", numTimes);
        int sorttimes[numTimes];
        for (int i = 0; i < numTimes; i++)
        {
            runtimes[i] = atoi(argv[i+2]);
            sorttimes[i] = atoi(argv[i+2]);
        }
        
        bool calledOpt = false;
        int opt;
        
        //accounting for each type of flag
        for (int i = numTimes+2; i < argc; i++)
        {
            // recreating the schedule array to be an array of zeros
            for (int j = 0; j < nProcs; j++)
            {
                schedule[j] = 0;
            }
            
            //-opt flag
            if (strcmp(argv[i], "-opt") == 0)
            {
                int lwd = leastworksort(nProcs, numTimes, runtimes, schedule);
                for (int j = 0; j < nProcs; j++)
                {
                    schedule[j] = 0;
                }
                if (calledOpt == false)
                {
                    calledOpt = true;
                    opt = backtrackingcall(nProcs, runtimes, numTimes, 0, schedule, lwd, roundUpDivision(sumArray(runtimes, numTimes), nProcs), -1);
                }
                printf("-opt %d\n", opt);
            }
            
            //-lw flag
            else if (strcmp(argv[i], "-lw") == 0)
            {
                printf("-lw  %d\n", leastwork(nProcs, numTimes, runtimes, schedule));
            }
            
            // -lwd flag
            else if (strcmp(argv[i], "-lwd") == 0)
            {
                printf("-lwd %d\n", leastworksort(nProcs, numTimes, sorttimes, schedule));
            }
            
            // -bw flag
            else if (strcmp(argv[i], "-bw") == 0)
            {
                
                printf("-bw  %d\n", bestwork(nProcs, numTimes, runtimes, schedule));
            }
            
            // -bwd flag
            else if (strcmp(argv[i], "-bwd") == 0)
            {
                printf("-bwd %d\n", bestworksort(nProcs, numTimes, sorttimes, schedule));
            }
        }
    }
    return 0;
}

