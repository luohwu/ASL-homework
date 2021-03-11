//#error Please comment out the next two lines under linux, then comment this error
//#include "stdafx.h"  //Visual studio expects this line to be the first one, comment out if different compiler
//#include <windows.h> // Include if under windows

#ifndef WIN32
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tsc_x86.h"

#define NUM_RUNS 1
#define CYCLES_REQUIRED 1e6
#define FREQUENCY 2.1e9
#define CALIBRATE

//Have to create the array like this, otherwise there will be "Segmentation fault (core dumped)" error on my computer
double x[1 << 23];
double y[1 << 23];

double scalar_product(double *x, double *y, int n)
{
    double s = 0;
    double s2 = 0;
    double s3 = 0;
    double s4 = 0;
    int i;
    for (i = 0; i < n - 5; i = i + 2)
    {
        s += x[i] * y[i];
        s2 += x[i + 1] * y[i + 1];
        // s3 += x[i+2] * y[i+2];
        // s4 += x[i+3] * y[i+3];
    }
    while (i < n)
    {

        s += x[i] * y[i];
        i++;
    }
    s = s + s2 + s3 + s4;

    return s;
}

/* 
 * Timing function based on the TimeStep Counter of the CPU.
 */

double rdtsc(int n)
{
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;

    for (int i = 0; i < n; i++)
    {
        x[i] = rand() % 1000 / 3. + 1.1;
        y[i] = rand() % 1000 / 3. + 1.1;
    }
    double s;

    /* 
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to 
     * avoid measurements bias due to the timing overhead.
     */
#ifdef CALIBRATE
    while (num_runs < (1 << 11))
    {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i)
        {
            s = scalar_product(x, y, n);
        }
        cycles = stop_tsc(start);

        if (cycles >= CYCLES_REQUIRED)
            break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i)
    {
        s = scalar_product(x, y, n);
    }

    // printf("used cycles:= %lf\n", (double)stop_tsc(start) / num_runs);
    cycles = stop_tsc(start) / num_runs;
    printf("the sum is %lf\n", s);
    return (double)cycles;
}

int main(int argc, char **argv)
{
    // if (argc!=2) {printf("usage: FW <n>\n"); return -1;}
    // int n = atoi(argv[1]);
    FILE *fp;
    fp = fopen("data_ex4.txt", "w");
    if (fp == NULL)
    {
        printf("failed to create file data.txt\n");
    }
    for (int i = 4; i <= 23; i++)
    {
        int n = 1 << i;
        int operations = 2 * n;
        for (int j = 0; j < 30; j++)
        {
            double cycles = rdtsc(n);
            fprintf(fp, "%lf ", operations / cycles);
            printf("This is i=:%d, n=:%d, j=:%d, performance:=%lf \n", i, n, j, operations / cycles);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    return 0;
}
