#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
    printf("Cache Simulator - CS 3853 – Team #4\n\n");

    int virtual_address_space_bits = 32;
    int cache_size_KB = 0;
    int block_size = 0;
    int associativity = 0;
    char rep_policy[15];
    int p_memory_MB = 0;
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
            cache_size_KB = atoi(argv[i + 1]);
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
            else
            {
                printf("Error. Replacement Policy must be either rr or rnd.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            p_memory_MB = atoi(argv[i + 1]);
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
    printf("***** Cache Input Parameters *****\n\n");
    printf("%-33s%d KB\n", "Cache Size:", cache_size_KB);
    printf("%-33s%d bytes\n", "Block Size:", block_size);
    printf("%-33s%d\n", "Associativity:", associativity);
    printf("%-33s%s\n", "Replacement Policy:", rep_policy);
    printf("%-33s%d MB\n", "Physical Memory:", p_memory_MB);
    printf("%-33s%.1f%%\n", "Percent Memory Used by System:", usage);
    printf("%-33s%d\n", "Instructions / Time Slice:", inst_per_time);

    // Calculate cache values
    int num_blocks = (cache_size_KB * pow(2, 10)) / block_size;

    int num_rows = num_blocks / associativity;

    int index = log2(num_rows);

    int offset = log2(block_size);

    int total_address_bits = log2(p_memory_MB) + 20;

    int tag = total_address_bits - index - offset;

    int valid = 1;

    int overhead = num_blocks * (tag + valid) / 8;

    int implementation_size = overhead + (cache_size_KB * pow(2, 10));

    double implementation_size_KB = implementation_size / pow(2, 10);

    double cost_per_KB = 0.07;

    double total_cost = implementation_size_KB * cost_per_KB;

    // Print cache calculated values
    printf("\n***** Cache Calculated Values  *****\n\n");
    printf("%-33s%d\n", "Total # Blocks:", num_blocks);
    printf("%-33s%d bits\n", "Tag Size:", tag);
    printf("%-33s%d bits\n", "Index Size:", index);
    printf("%-33s%d\n", "Total # Rows:", num_rows);
    printf("%-33s%d bytes\n", "Overhead Size:", overhead);
    printf("%-33s%.2f KB (%d bytes)\n", "Implementation Memory Size:", implementation_size_KB, implementation_size);
    printf("%-33s$%.2f @ $%.2f per KB\n", "Cost:", total_cost, cost_per_KB);

    return 0;
}