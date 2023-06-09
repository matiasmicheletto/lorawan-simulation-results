#include "random.h"


OptimizationResults randomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose){
    /* Fully random uniformly distributed solutions are generated */

    auto start = std::chrono::high_resolution_clock::now();

    if(verbose)
        std::cout << std::endl << "-------------- RS ----------------" << std::endl << std::endl;

    // Optimization variable
    unsigned int* gw = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    unsigned int* sf = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    // Best
    unsigned int* gwBest = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    unsigned int* sfBest = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());

    // Random generators use uniform distribution
    Uniform gwGenerator = Uniform(0, l->getGWCount());
    Uniform sfGenerator = Uniform(7, 12);

    double bestQ = __DBL_MAX__; // Cost minimization
    bool found = false;

    if(verbose)
        std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    for(unsigned int k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(unsigned int i = 0; i < l->getEDCount(); i++){
            gw[i] = gwGenerator.random();
            sf[i] = sfGenerator.random();
        }
        // Test generated solution
        unsigned int gwCount, energy;
        double totalUF;
        bool feasible;
        const double q = o->eval(gw, sf, gwCount, energy, totalUF, feasible);

        if(q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            std::copy(gw, gw + l->getEDCount(), gwBest);
            std::copy(sf, sf + l->getEDCount(), sfBest);
            if(verbose){
                std::cout << "New best at iteration: " << k << std::endl;
                o->printSolution(gw, sf, false);
                std::cout << std::endl << std::endl;
            }
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        
    if(verbose){    
        std::cout << "Random search finished, " << maxIters << " iterations executed in " << duration << " milliseconds." << std::endl;
        if(found){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
        }
    }

	OptimizationResults results;
    results.ready = found; // Set export flag to ready if solution was found
    if(found){
        results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
        results.execTime = duration;
        results.tp = o->tp;
    }

    // Release memory
    free(gw);
    free(sf);
    free(gwBest);
    free(sfBest);

    return results;
}

OptimizationResults improvedRandomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose) {
    /* Random solutions are generated within feasible and more convenient SF and GW values */
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose)
        std::cout << std::endl << "-------------- IRS ---------------" << std::endl << std::endl;

    // Optimization variable
    unsigned int* gw = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    unsigned int* sf = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    // Best
    unsigned int* gwBest = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());
    unsigned int* sfBest = (unsigned int*) malloc( sizeof(unsigned int) * l->getEDCount());

    Uniform uniform = Uniform(0.0, 1.0);

    double bestQ = __DBL_MAX__; // Cost minimization
    bool found = false;

    if(verbose)
        std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    for(unsigned int k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(unsigned int i = 0; i < l->getEDCount(); i++){
            std::vector<unsigned int> gwList = l->getGWList(i); // Valid gw for this ed
            gw[i] = gwList.at((unsigned int)floor(uniform.random()*gwList.size())); // Pick random gw
            // Pick random SF from valid range
            const unsigned int minSF = l->getMinSF(i, gw[i]);
            const unsigned int maxSF = l->getMaxSF(i);
            sf[i] = uniform.random()*(maxSF - minSF) + minSF;
        }
        // Test generated solution
        unsigned int gwCount, energy;
        double totalUF;
        bool feasible;
        const double q = o->eval(gw, sf, gwCount, energy, totalUF, feasible);

        if(q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            std::copy(gw, gw + l->getEDCount(), gwBest);
            std::copy(sf, sf + l->getEDCount(), sfBest);
            if(verbose){
                std::cout << "New best at iteration: " << k << std::endl;
                o->printSolution(gw, sf, false);
                std::cout << std::endl;
            }
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

    if(verbose){
        std::cout << "Improved random search finished, " << maxIters << " iterations executed in " << duration << " milliseconds." << std::endl;
        if(found){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
        }
    }

    OptimizationResults results;
    results.ready = found; // Set export flag to ready if solution was found
    if(found){
        results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
        results.execTime = duration;
        results.tp = o->tp;
    }

    // Release memory
    free(gw);
    free(sf);
    free(gwBest);
    free(sfBest);

    return results;
}