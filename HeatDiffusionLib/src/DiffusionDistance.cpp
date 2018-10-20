#include "HeatDiffusion/Distance/DiffusionDistance.h"
#include <cmath>

namespace HeatDiffusion{
    namespace Distance{

        DiffusionDistance::DiffusionDistance(){
            evals = evecs = 0;
            num_vertex = num_eigenvalues = 0;
        }

        DiffusionDistance::DiffusionDistance(float* eval, float* evec, unsigned int nV, unsigned int nE){
            evals = eval;
            evecs = evec;
            num_vertex = nV;
            num_eigenvalues = nE;
        }

        float DiffusionDistance::computeSquaredDistance(unsigned int v1, unsigned int v2, float t){
            float t0 = t / (2*evals[1]);

            float sum = 0.0;
            for(int i = 1; i < num_eigenvalues; i++){
                float temp = (evecs[num_vertex * i + v1] - evecs[num_vertex * i + v2]);
                sum += exp(-2*t0*fabs(evals[i]))* temp * temp;
            }

            return sum;
        }

    }
}
