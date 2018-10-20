#include "HeatDiffusion/Descriptor/WaveKernelSignature.h"
#include <cmath>
#include <vector>
#include <cassert>
#include <cstring>

using namespace std;

namespace HeatDiffusion{
    namespace Descriptor{

        WaveKernelSignature :: WaveKernelSignature(){
            evals = evecs = areas = wks = 0;
            num_vertex = num_eigenvalues = 0;
            dimension = 0;
            variance = 0.0;

        }

        WaveKernelSignature :: WaveKernelSignature(float* eval, float* evec, float* area, unsigned int nV, unsigned int nE, unsigned int dim, float var){
            evals = eval;
            evecs = evec;
            areas = area;

            num_vertex = nV;
            num_eigenvalues = nE;
            dimension = dim;
            variance = var;
            wks = 0;
        }

        float* WaveKernelSignature :: getDescriptors(){
            return wks;
        }

        void WaveKernelSignature :: computeDescriptor(){
            float *E = new float[num_eigenvalues];

            float maxElem = -100000;
            for(int i = 0; i < num_eigenvalues; i++){
                E[i] = fabs(evals[i]);
                if(E[i] < 1e-3){
                    E[i] = 1e-3;
                }
                E[i] = log(E[i]);

                if(E[i] > maxElem)
                    maxElem = E[i];
            }

            float stepsize = ((maxElem/1.02)-E[1])/dimension;
            vector<float> e;

            for(float ti = E[1]; ti < (maxElem/1.02); ti = ti + stepsize)
                e.push_back(ti);

            assert(dimension == e.size());

            float sigma = (e[1] - e[0])*variance;

            wks = new float[num_vertex * dimension];
            float* C = new float[dimension];

            for(unsigned int i = 0; i < num_vertex; i++){
                for(unsigned int j = 0; j < dimension; j++){
                    float sum = 0.0;
                    for(unsigned int k = 0; k < num_eigenvalues; k++){
                        float aux = e[j] - E[k];
                        sum += evecs[num_vertex * k + i]*evecs[num_vertex * k + i]*exp((-aux*aux)/(2 * sigma * sigma));
                    }
                    wks[i * dimension + j] = sum;
                }
            }

            for(unsigned int i = 0; i < dimension; i++){
                C[i] = 0.0;
                for(unsigned int j = 0; j < num_eigenvalues; j++){
                    float aux = e[i] - E[j];
                    C[i] += exp((-aux*aux)/(2 * sigma * sigma));
                }
            }

            for(unsigned int i =0; i < num_vertex; i++){
                for(unsigned int j = 0; j < dimension; j++){
                    wks[i * dimension + j] = wks[i * dimension + j]/C[j];
                }
            }

            delete[] E;
            delete[] C;

        }
    }
}


