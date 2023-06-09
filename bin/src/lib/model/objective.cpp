#include "objective.h"

Objective::Objective(Instance* instance, const TunningParameters& tp) {
    this->instance = instance;
    this->tp = tp;
}

Objective::~Objective() {

}

const uint Objective::unfeasibleIncrement = 1000000;

double Objective::eval(unsigned int* gw, unsigned int* sf, unsigned int &gwCount, unsigned int &energy, double &maxUF, bool &feasible) {    
    // Reset objectives (if unfeasible solution, these values remain 0 and _DBL_MAX_ is returned)
    gwCount = 0;
    energy = 0;
    maxUF = 0.0;
    feasible = true;
    double cost = 0.0; // Cost value have meaning when solutions are feasible, else will take large values

    UtilizationFactor gwuf[this->instance->getGWCount()]; // Array of UF objects
    std::fill(gwuf, gwuf + this->instance->getGWCount(), UtilizationFactor()); // Initialize all elements to 0.0 (all SF from 7 to 12)

    for(unsigned int i = 0; i < this->instance->getEDCount(); i++){ // For each ED    
        // Check if feasible SF
        unsigned int minSF = this->instance->getMinSF(i, gw[i]);
        unsigned int maxSF = this->instance->getMaxSF(i);
        if(sf[i] > maxSF || sf[i] < minSF){ // Unfeasibility condition: not valid SF available for this GW
            feasible = false;
            cost += unfeasibleIncrement;
        }

        // Compute GW UF
        gwuf[gw[i]] += this->instance->getUF(i, sf[i]);
        if(gwuf[gw[i]].isFull()){ // Unfeasibility condition: UF > 1 for some SF
            feasible = false;
            cost += unfeasibleIncrement;
        }
        double maxUFTemp = gwuf[gw[i]].getMax(); // Max UF value between all SF
        if(maxUFTemp > maxUF) // Update max UF
            maxUF = maxUFTemp;
    }

    // Count number of used gw
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(gwuf[j].isUsed()) 
            gwCount++;

    // Compute energy cost
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += this->instance->sf2e(sf[i]);// energy += pow(2, sf[i] - 7);

    // If solution is feasible, at this point, cost should equal 0.0
    cost += this->tp.alpha * (double) gwCount + 
            this->tp.beta * (double) energy + 
            this->tp.gamma * maxUF;

    return cost; 
}

void Objective::printSolution(unsigned int* gw, unsigned int* sf, bool allocation, bool highlight){
    unsigned int gwCount;
    unsigned int energy;
    double maxUF;
    bool feasible;

    const double result = this->eval(gw, sf, gwCount, energy, maxUF, feasible);

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font

    std::cout << "Cost=" << result 
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << maxUF
                << ")" << std::endl;
    
    if(allocation){
        std::cout << "GW allocation (GW[SF]):" << std::endl;
        for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED    
            std::cout << gw[i] << "[" << sf[i] << "]  ";
        std::cout << std::endl;
    }
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
}
