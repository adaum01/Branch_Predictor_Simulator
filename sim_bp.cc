#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"

#include <vector>
#include <math.h>

// Input format
// ./sim bimodal 6 testTrace.txt    - BIMODAL FORMAT
// ./sim bimodal <M_value> <trace_file>

// ./sim gshare 9 3 testTrace.txt   - GSHARE FORMAT
// ./sim gshare <M_value> <N_value> <trace_file>

// Starter code -------------------------------------------------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
// Starter code -------------------------------------------------------------------------------------------------------------------------

    
    // Initialize predictor object
    bimodal bimodalPredictor = bimodal(params.M2);
    gshare gsharePredictor = gshare(params.M1, params.N);


// Starter code -------------------------------------------------------------------------------------------------------------------------
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF){
        outcome = str[0];
// Starter code -------------------------------------------------------------------------------------------------------------------------


        if(strcmp(params.bp_name, "bimodal") == 0){
            bimodalPredictor.predict(addr, outcome);
        }
        else if(strcmp(params.bp_name, "gshare") == 0){
            gsharePredictor.predict(addr, outcome);
        }
    }

    if(strcmp(params.bp_name, "bimodal") == 0){
        bimodalPredictor.printMeasurements();
        bimodalPredictor.printContents();
    }
    if(strcmp(params.bp_name, "gshare") == 0){
        gsharePredictor.printMeasurements();
        gsharePredictor.printContents();
    }
    return 0;
}
