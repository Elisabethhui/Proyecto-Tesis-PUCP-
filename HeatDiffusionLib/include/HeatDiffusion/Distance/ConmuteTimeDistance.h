#ifndef __CONMUTE_TIME_DISTANCE_H__
#define __CONMUTE_TIME_DISTANCE_H__

#include "HeatDiffusion/HeatDiffusion_global.h"

namespace HeatDiffusion{
    namespace Distance{

        //! Compute the Conmute-time distance between vertices in a 3D mesh
        class HEATDIFFUSION_API ConmuteTimeDistance{
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
                ConmuteTimeDistance();

                //! Constructor
                /*!
                    It creates an object with the provided data
                    \param eval the buffer of eigenvalues
                    \param evecs the buffer of eigenvectors
                    \param nV the number of vertices
                    \param nE the number of eigenvalues
                */
                ConmuteTimeDistance(float* eval, float* evec, unsigned int nV, unsigned int nE);

                //! Setter
                /*!
                    Sets the eigenvalues and eigenvectors of the Laplacian matrix
                */
                void setEigendecomposition(float* eval, float* evec){evals = eval; evecs = evec; }

                //! Setter method
                /*!
                    Sets the number of vertices
                    \param nV the number of vertices
                */
                void setNumberVertices(unsigned int nV){num_vertex = nV;}

                //! Setter method
                /*!
                    Sets the number of eigen-pairs
                    \param nE the number of eigen-pairs
                */
                void setNumberEigenvalues(unsigned int nE){num_eigenvalues = nE;}

                //! Computation of distance
                /*!
                    Computes the squared conmute-time distance between two vertices
                    \param v1 the index of the first vertex
                    \param v2 the index of the second vertex
                */
                float computeSquaredDistance(unsigned int v1, unsigned int v2);

        };
    }
}

#endif
