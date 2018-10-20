#ifndef _HEAT_KERNEL_SIGNATURE_H_
#define _HEAT_KERNEL_SIGNATURE_H_

#include "HeatDiffusion/HeatDiffusion_global.h"
#include <vector>

namespace HeatDiffusion{
    namespace Descriptor{

        //! HeatKernelSignature class
        /*!
            A class to compute Heat Kernel Signatures
        */
        class HEATDIFFUSION_API HeatKernelSignature{
            protected:
                float* evals;
                float* evecs;
                float* areas;

                unsigned int num_vertex;
                unsigned int num_eigenvalues;

                unsigned int dimension;
                float* hks;

            public:
                //! A default constructor
                /*!
                    It creates an empty HeatKernelSignature object. You must use the setter methods to provide the required information.
                    \sa setEigendecomposition(), setNumberVertices(), setNumberEigenvalues() and setDescriptorDimension().
                */
                HeatKernelSignature();

                //! A constructor
                /*!
                    It creates a HeatKernelSignature object with the provided information.
                    \param eval the buffer of eigenvalues
                    \param evec the buffer of eigenvectors
                    \param area the buffer of areas
                    \param nV the number of vertices
                    \param nE the number of eigen-pairs
                    \param dim the dimension of the output descriptors
                */
                HeatKernelSignature(float* eval, float* evec, float* area, unsigned int nV, unsigned int nE, unsigned int dim);

                //! Setter method
                /*!
                    Sets the information of the Eigendecomposition of the Laplacian
                    \param eval the buffer of eigenvalues
                    \param evec the buffer of eigenvectors
                    \param area the buffer of areas
                */
                void setEigendecomposition(float* eval, float* evec, float* area){evals = eval; evecs = evec; areas = area;}

                //! Setter method
                /*!
                    Sets the number of vertices
                    \param nV the number of vertices
                */
                void setNumberVertices(unsigned int nV){ num_vertex = nV;}

                //! Setter method
                /*!
                    Sets the number of eigen-pairs
                    \param nE the number of eigen-pairs
                */
                void setNumberEigenvalues(unsigned int nE){ num_eigenvalues = nE;}

                //! Setter method
                /*!
                    Sets the dimension of the output descriptors
                    \param dim the dimension of the output descriptors
                */
                void setDescriptorDimension(unsigned int dim){ dimension = dim;}

                //! Computation method
                /*!
                    It performs the computation of the HKS descriptors
                */
                void computeDescriptor();

                //! Access method
                /*!
                    Gets the buffer of descriptors. Descriptor matrix (num_vertex x dimension) are stored in row-wise format
                */
                float* getDescriptors();

        };
    }
}
#endif


