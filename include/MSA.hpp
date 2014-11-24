#ifndef MSA_HPP
#define MSA_HPP
#include "DGraph.hpp"
#include <list>

/**Solve the minimum arborescence problem using the algorithm proposed
   in the following papers:
   -Tarjan, R. E., Finding optimum branchings, Networks, 1977
   -Camerini, P. M., L. Fratta, and F. Maffioli. 
    A note on finding optimum branchings. Networks 9.4 (1979): 309-312.
   INPUT:
   -g is the directed graph describing the instance
   -r is the root of the arborescence
   OUTPUT:
   The function returns the objective function value and in the list
   solution will be put the arcs contained in the optimal arborescence
   The following prepocessor directives can be used:
   -MSA_OUTPUT: if defined the function prints some output describing
    what the algorithm is doing
   -NDEBUG: if NOT defined at the end we check the feasibility of 
    solution
*/
double minimumArborescence(DGraph& g, int r, DGraph& solution);



#endif