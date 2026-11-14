#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES (1 << 19)

typedef struct
{
    int valid;
    int physical_page;
} PageTableEntry;

PageTableEntry *page_tables[3] = {NULL, NULL, NULL};
int total_physical_pages = 0;
int next_free_page = 0;
unsigned long long page_table_hits = 0;
unsigned long long pages_from_free = 0;

int setup_page_tables(int count, int num_physical_pages, int num_system_pages){
    total_physical_pages = num_physical_pages;
    next_free_page = num_system_pages;

    // Each trace file represents a process with its own page table
    for (int i = 0; i < count; i++)
    {
        page_tables[i] = calloc(PAGE_TABLE_ENTRIES, sizeof(PageTableEntry));

        if (page_tables[i] == NULL)
        {
            printf("Error. Could not allocate the page table for process %d\n", i);
            return -1;
        }
    }

    return 0;
}

void free_process(int a){
    // TODO: Free physical pages and invalidate cache entries
    (void)a;
}

int process_memory_access(int a, unsigned int v, int c)
{
    if (c <= 0)
    {
        return 0;
    }

    unsigned int first_page = v / PAGE_SIZE;
    unsigned int last_page = (v + c - 1) / PAGE_SIZE;

    // An access may span two virtual pages
    for (unsigned int i = first_page; i <= last_page; i++)
    {
        if (i >= PAGE_TABLE_ENTRIES)
        {
            printf("Error. Virtual address is outside the supported page table.\n");
            exit(1);
        }

        if (page_tables[a][i].valid == 1)
        {
            // Page table hit: this process already has the virtual page mapped
            page_table_hits++;
        }
        else if (next_free_page < total_physical_pages)
        {
            // Map an unused physical page to the virtual page
            page_tables[a][i].physical_page = next_free_page;
            page_tables[a][i].valid = 1;
            next_free_page++;
            pages_from_free++;
        }
        else
        {
            // TODO: Page faults and page replacement are not implemented yet
            printf("Error. No free physical pages; page replacement is not implemented.\n");
            exit(1);
        }
    }

    return 0;
}
void run_simulation(char *files[], int count, int inst_per_time)
{
    FILE *fp[3];
    int file_completed[3] = {0, 0, 0};
    int active_processes = count;

    // Open all files
    for (int i = 0; i < count; i++)
    {
        fp[i] = fopen(files[i], "r");

        if (fp[i] == NULL)
        {
            printf("Error. Could not open the file %s\n", files[i]);
            exit(1);
        }
    }

    char line1[256];
    char line2[256];
    int current_process = 0;

    while (active_processes > 0)
    {
        if (file_completed[current_process] == 0)
        {
            int inst_count = 0;

            while (inst_per_time == -1 || inst_count < inst_per_time)
            {
                // Check if we have already read all the lines of a file
                if (fgets(line1, sizeof(line1), fp[current_process]) == NULL || fgets(line2, sizeof(line2), fp[current_process]) == NULL)
                {
                    file_completed[current_process] = 1;
                    fclose(fp[current_process]);
                    active_processes--;
                    // TODO: Free the proccess
                    free_process(current_process);
                    break;
                }

                int inst_length;
                unsigned int start_address;
                unsigned int destM;
                char destval[16];
                unsigned int srcM;
                char srcval[16];

                if (sscanf(line1, "EIP (%d): %x", &inst_length, &start_address) == 2)
                {
                    // TODO: access the address on the page table
                    process_memory_access(current_process, start_address, inst_length);
                }
                if (sscanf(line2, "dstM: %x %s srcM: %x %s", &destM, destval, &srcM, srcval) == 4)
                {
                    if (destM != 0 && strcmp(destval, "--------") != 0)
                    {
                        // TODO: access the address on the page table
                        process_memory_access(current_process, destM, 4);
                    }
                    if (srcM != 0 && strcmp(srcval, "--------") != 0)
                    {
                        // TODO: access the address on the page table
                        process_memory_access(current_process, srcM, 4);
                    }
                }

                inst_count++;
            }
        }
        // Each process take a turn
        current_process = (current_process + 1) % count;
    }
}

int main(int argc, char *argv[])
{
    printf("Cache Simulator - CS 3853 – Team #04\n\n");

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

    printf("\n");

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

    // Calculate physical values
    int page_size = pow(2, 12);

    int num_physical_pages = (p_memory_MB * pow(2, 20)) / page_size;

    int num_system_pages = num_physical_pages * (usage / 100);

    int page_table_entry_size = log2(num_physical_pages) + 1;

    int pages_table_entries = pow(2, 19);

    int total_page_table_ram = (pages_table_entries * count * page_table_entry_size) / 8;

    printf("\n***** Psyhical Memory Calculated Values  *****\n\n");
    printf("%-33s%d\n", "Number of Physical Pages: ", num_physical_pages);
    printf("%-33s%d\n", "Number of Pages for System: ", num_system_pages);
    printf("%-33s%d bits\n", "Size of Page Table Entry: ", page_table_entry_size);
    printf("%-33s%d bytes\n", "Total RAM for Page Table: ", total_page_table_ram);

    // Setup the page tables before reading the trace files
    if (setup_page_tables(count, num_physical_pages, num_system_pages) != 0){
        for (int i = 0; i < count; i++){
            free(page_tables[i]);
        }
        return 1;
    }
    // Virtual memory simulation
    run_simulation(files, count, inst_per_time);

    // Print the virtual memory results implemented so far
    printf("\n***** VIRTUAL MEMORY SIMULATION RESULTS *****\n\n");
    printf("%-33s%d\n", "Physical Pages Used By SYSTEM:", num_system_pages);
    printf("%-33s%d\n", "Pages Available to User:", num_physical_pages - num_system_pages);
    printf("\n%-33s%llu\n", "Virtual Pages Mapped:", page_table_hits + pages_from_free);
    printf("        ------------------------------\n");
    printf("%-33s%llu\n", "        Page Table Hits:", page_table_hits);
    printf("%-33s%llu\n", "        Pages from Free:", pages_from_free);

    for (int i = 0; i < count; i++){
        free(page_tables[i]);
    }
    return 0;
}