#include "HeatDiffusion/Distance/ConmuteTimeDistance.h"
#include <cmath>

namespace HeatDiffusion{
    namespace Distance{

        ConmuteTimeDistance::ConmuteTimeDistance(){
            evals = evecs = 0;
            num_vertex = num_eigenvalues = 0;
        }

        ConmuteTimeDistance::ConmuteTimeDistance(float* eval, float* evec, unsigned int nV, unsigned int nE){
            evals = eval;
            evecs = evec;
            num_vertex = nV;
            num_eigenvalues = nE;
        }

        float ConmuteTimeDistance::computeSquaredDistance(unsigned int v1, unsigned int v2){

            float sum = 0.0;
            for(int i = 1; i < num_eigenvalues; i++){
                float temp = (evecs[num_vertex * i + v1] - evecs[num_vertex * i + v2]);
                sum += temp * temp * (1.0/fabs(evals[i]));
            }

            return sum;
        }

    }
}
