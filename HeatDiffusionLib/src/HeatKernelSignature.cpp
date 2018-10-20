#include "HeatDiffusion/Descriptor/HeatKernelSignature.h"
#include <cmath>
#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

namespace HeatDiffusion{
    namespace Descriptor{

        HeatKernelSignature :: HeatKernelSignature(){
            evals = evecs = areas = hks = 0;
            num_vertex = num_eigenvalues = 0;
            dimension = 0;

        }

        HeatKernelSignature :: HeatKernelSignature(float* eval, float* evec, float* area, unsigned int nV, unsigned int nE, unsigned int dim){
            evals = eval;
            evecs = evec;
            areas = area;

            num_vertex = nV;
            num_eigenvalues = nE;
            dimension = dim;
            hks = 0;
        }

        float* HeatKernelSignature :: getDescriptors(){
            return hks;
        }

        void HeatKernelSignature :: computeDescriptor(){
            float tmin = fabs(4 * log(10.0) / evals[num_eigenvalues-1]);
            float tmax = fabs(4 * log(10.0) / evals[1]);

            int nstep = dimension;
            float stepsize = (log(tmax) - log(tmin))/nstep;

            vector<float> logts;
            for(float ti = log(tmin); ti < log(tmax); ti = ti + stepsize)
                logts.push_back(ti);

            assert(dimension==logts.size());
            //dimension = logts.size();

            float* ts = new float[dimension];

            for(unsigned int i = 0; i < dimension; i++)
                ts[i] = exp(logts[i]);

            hks = new float[num_vertex * dimension];

            for(unsigned int i = 0; i < num_vertex; i++){
                for(unsigned int j = 0; j < dimension; j++){
                    float sum = 0.0;
                    for(unsigned int k = 1; k < num_eigenvalues; k++){
                        sum += evecs[num_vertex*k + i]*evecs[num_vertex*k + i]*exp(ts[j]*(fabs(evals[1]) - fabs(evals[k])));
                    }
                    hks[i*dimension + j] = sum;
                }
            }

            float* scale = new float[dimension];
            memset(scale, 0, sizeof(float)*dimension);

            for(unsigned int i = 0; i < num_vertex; i++){
                for(unsigned int j = 0; j < dimension; j++){
                    scale[j] += hks[i*dimension + j]*areas[i];
                }
            }

            for(unsigned int i = 0; i < dimension; i++)
                scale[i] = 1.0/scale[i];

            for(unsigned int i = 0; i < num_vertex; i++){
                for(unsigned int j = 0; j < dimension; j++){
                    hks[i*dimension + j] = hks[i*dimension + j]*scale[j];
                }
            }

            //cout << hks << endl;
            delete[] ts;
            delete[] scale;

            //std::cout << "HKS:" << hks << std::endl;
        }
    }
}
