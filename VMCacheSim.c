#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int cache_size = 0;
    int block_size = 0;
    int associativity = 0;
    char rep_policy[15];
    int p_memory = 0;
    double usage = 0.0;
    int inst_per_time = 0;
    char *files[3];
    int count = 0;

    // Parse the arguments

    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 >= argc)
        {
            printf("Error. Flag %s missing value.\n", argv[i]);
            return 1;
        }

        if (strcmp(argv[i], "-s") == 0)
        {
            cache_size = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            block_size = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            associativity = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            if (strcmp(argv[i + 1], "rr") == 0)
            {
                strcpy(rep_policy, "Round Robin");
            }
            else if (strcmp(argv[i + 1], "rnd") == 0)
            {
                strcpy(rep_policy, "Random");
            }
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            p_memory = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
            usage = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            inst_per_time = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            if (count >= 3)
            {
                printf("Error. Maximun of 3 trace files allowed.\n");
                return 1;
            }
            files[count] = argv[i + 1];
            count++;
        }
        else
        {
            printf("Error. Invalid argument.\n");
            return 1;
        }
    }

    // Print trace files
    printf("Trace File(s):\n");
    for (int i = 0; i < count; i++)
    {
        printf("\t%s\n", files[i]);
    }

    // Print input paramenters
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size:%24d KB\n", cache_size);
    printf("Block Size:%23d bytes\n", block_size);
    printf("Associativity:%19d\n", associativity);
    printf("Replacement Policy:%24s\n", rep_policy);
    printf("Physical Memory:%20d MB\n", p_memory);
    printf("Percent Memory Used by System:%6.1f%%\n", usage);
    printf("Instructions / Time Slice:%9d\n", inst_per_time);

    return 0;
}