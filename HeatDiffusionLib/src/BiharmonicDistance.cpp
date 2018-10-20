#include "HeatDiffusion/Distance/BiharmonicDistance.h"
#include <cmath>

namespace HeatDiffusion{
    namespace Distance{

        BiharmonicDistance::BiharmonicDistance(){
            evals = evecs = 0;
            num_vertex = num_eigenvalues = 0;
        }

        BiharmonicDistance::BiharmonicDistance(float* eval, float* evec, unsigned int nV, unsigned int nE){
            evals = eval;
            evecs = evec;
            num_vertex = nV;
            num_eigenvalues = nE;
        }

        float BiharmonicDistance::computeSquaredDistance(unsigned int v1, unsigned int v2){

            float sum = 0.0;
            for(int i = 1; i < num_eigenvalues; i++){
                float temp = (evecs[num_vertex * i + v1] - evecs[num_vertex * i + v2]);
                float temp2 = evals[i]*evals[i];
                sum += temp * temp * (1.0/temp2);
            }

            return sum;
        }

    }
}

