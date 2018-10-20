#ifndef __POINTND_H__
#define __POINTND_H__

#include "adaptive_clustering_global.h"
#include <cmath>
#include <vector>
using namespace std;

namespace AdaptiveClustering{

class ADAPTIVE_API PointND{
	public:
        vector<float> descriptor;
		int index;
		int cla;

	public:
		PointND(){cla = 0;}
        void addValue(float value){descriptor.push_back(value);}
		void addIndex(int ind){ index = ind;}
        vector<float> getDescriptor(){ return descriptor;}
		void setClass(int c){cla = c;}
		int getClass(){return cla;}
};

}
#endif
