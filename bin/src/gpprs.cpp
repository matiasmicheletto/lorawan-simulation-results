#define MANUAL "readme_gpprs.txt"
#define LOGFILE "summary.csv"

#include <cstring>
#include "lib/util/util.h"
#include "lib/instance/instance.h"
#include "lib/objective/objective.h"
#include "lib/optimization/random.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/ga.h"
#include "lib/optimization/results.h"

int main(int argc, char **argv) {
    
    srand(time(NULL));

    Instance *l = 0;
    unsigned long maxIters = 1e5;
    TunningParameters tp; // alpha, beta and gamma
    bool verbose = false; // Disable printing to terminal
    int method = 1; // Default is improved random search
    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc) 
                l = new Instance(argv[i + 1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iter") == 0) {
            if(i+1 < argc) 
                maxIters = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0){
            if(i+1 < argc)
                tp.alpha = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                tp.beta = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gamma") == 0){
            if(i+1 < argc)
                tp.gamma = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--method") == 0) {
            if(i+1 < argc) 
                method = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            verbose = true;
        }
    }

    if(l == 0) printHelp(MANUAL);

    if(verbose) {    
        std::cout << "Input file loaded." << std::endl;
        std::cout << "GW Count: " << l->getGWCount() << std::endl;
        std::cout << "ED Count: " << l->getEDCount() << std::endl; 
        std::cout << "Tunning parameters:" << std::endl;
        std::cout << "  Alpha: " << tp.alpha << std::endl;
        std::cout << "  Beta: " << tp.beta << std::endl;
        std::cout << "  Gamma: " << tp.gamma << std::endl;
        std::cout << "Optimization method: " << method << std::endl << std::endl;
    }

    Objective *o = new Objective(l, tp);
    OptimizationResults opr;

    switch (method) {
        case 0:
            randomSearch(l, o, maxIters, verbose);    
            break;
        case 1:
            improvedRandomSearch(l, o, maxIters, verbose);
            break;
        case 2:
            opr = greedy(l, o, maxIters, verbose);
            break;
        case 3:{
            GAConfig gaconfig;
            gaconfig.maxgen = maxIters/gaconfig.popsize;
            ga(l, o, gaconfig, verbose);
            break;
        }
        default:
            if(verbose)
                std::cout << "Unknown optimization method" << std::endl;
            break;
    }
    
    if(opr.withResults)
        logResultsToCSV(opr, LOGFILE);

    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
