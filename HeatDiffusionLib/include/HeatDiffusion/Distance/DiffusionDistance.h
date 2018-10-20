#ifndef __DIFFUSION_DISTANCE_H__
#define __DIFFUSION_DISTANCE_H__

#include "HeatDiffusion/HeatDiffusion_global.h"

namespace HeatDiffusion{
    namespace Distance{

        //! Compute the Diffusion distance between vertices in a 3D mesh
        class HEATDIFFUSION_API DiffusionDistance{
            protected:
                float* evals;
                float* evecs;

                unsigned int num_vertex;
                unsigned int num_eigenvalues;

             public:
                //! Constructor
                /*!
                    Default constructor.
                */
                DiffusionDistance();

                //! Constructor
                /*!
                    It creates an object with the provided data
                    \param eval the buffer of eigenvalues
                    \param evecs the buffer of eigenvectors
                    \param nV the number of vertices
                    \param nE the number of eigenvalues
                */
                DiffusionDistance(float* eval, float* evec, unsigned int nV, unsigned int nE);

                //! Setter
                /*!
                    Sets the eigenvalues and eigenvectors of the Laplacian matrix
                */
                void setEigendecomposition(float* eval, float* evec){evals = eval; evecs = evec;}

                //! Setter method
                /*!
                    Sets the number of vertices
                    \param nV the number of vertices
                */
                void setNumberVertices(unsigned int nV) {num_vertex = nV;}

                //! Setter method
                /*!
                    Sets the number of eigen-pairs
                    \param nE the number of eigen-pairs
                */
                void setNumberEigenvalues(unsigned int nE) {num_eigenvalues = nE;}

                //! Computation of distance
                /*!
                    Computes the squared diffusion distance between two vertices given a value of time
                    \param v1 the index of the first vertex
                    \param v2 the index of the second vertex
                    \param t the value of time
                */
                float computeSquaredDistance(unsigned int v1, unsigned int v2, float t);

        };
    }
}

#endif
