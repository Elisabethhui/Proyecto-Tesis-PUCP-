#include "AdaptiveClustering/adaptiveClustering.h"
#include <iostream>

using namespace std;

namespace AdaptiveClustering{

void adaptiveClusteringMedoids(float* distances, int numPoints, float R, float S, int Nm, int Iter, int option, vector<Cluster>& clusters){

	vector<int> mark(numPoints, 0);

	for(int i = 0; i < Iter; i++){
		vector<int> deleted;

		//For each point p
		for(int j = 0; j < numPoints; j++){

			if(mark[j])
				continue;

			double mindist = DBL_MAX;

			//Find the nearest cluster k to point p
			int pos;
			for(int k = 0; k < clusters.size(); k++){
				int cen = clusters[k].getIndexCentroid();
				double dist = distances[cen*numPoints + j];
				if(dist < mindist){
					mindist = dist;
					pos = k;
				}
			}

			if(mindist == DBL_MAX || mindist >= S){
				Cluster clu;
				clu.addIndex(j);
				clu.computeMedoid(distances, numPoints);
				clusters.push_back(clu);
				deleted.push_back(j);
			}else if(mindist <=R){
				clusters[pos].addIndex(j);
				deleted.push_back(j);
			}
		}

		cout << "Iter:" << i << " -> Num. clusters:" << clusters.size() << endl;

		reverse(deleted.begin(), deleted.end());

		for(int j = 0; j < deleted.size(); j++){
			mark[deleted[j]] = 1;
		}

		deleted.clear();

		for(int j = 0; j < clusters.size(); j++){
			if(clusters[j].getSizeIndex() >=Nm)
				clusters[j].computeMedoid(distances, numPoints);
			else{
				clusters[j].removeIndex(mark);
				deleted.push_back(j);
			}
		}

		reverse(deleted.begin(), deleted.end());
		for(int j = 0; j < deleted.size(); j++){
			clusters.erase(clusters.begin() + deleted[j]);
		}
	}

	if(option){
		for(int i = 0; i < numPoints; i++){
			if(mark[i])
				continue;

			double mindist = DBL_MAX;
			int pos;

			for(int j = 0; j < clusters.size(); j++){
				int cen = clusters[j].getIndexCentroid();
				double dist = distances[cen*numPoints + i];

				if(dist < mindist){
					mindist = dist;
					pos = j;
				}
			}

			clusters[pos].addIndex(i);
		}
	}
}

void adaptiveClustering(vector<PointND>& points, double R, double S, double Nm, int Iter, int option,  vector<Cluster>& clusters){
	//Set of clusters
	//vector<Cluster> clusters;

	/*cout <<"Parameters"<< endl;
	cout << "R=" << R << endl;
	cou<< "S=" << S << endl;
	cout << "Nm=" << Nm << endl;
	cout << "Iter=" << Iter << endl;*/
	int colLen = points[0].getDescriptor().size();
	for(int i = 0; i < Iter; i++){

		//cout << "Iter " << i << endl;
		vector<int> deleted;
		//For each point p
		for(int j = 0; j < points.size(); j++){
			//if(j == 9466)
			//	cout << "Aqui hay error" << endl;
			//cout << "Point " << j << endl;
			//Find the nearest cluster k to point p
			double mindist = DBL_MAX;
			int pos;
			for(int k = 0; k < clusters.size(); k++){
				PointND cen = clusters[k].getCentroid();
				double dist = clusters[k].computeDistance(cen, points[j]);
				if(dist < mindist){
					mindist = dist;
					pos = k;
				}
			}
			//cout << "Search finalized" << endl;
			if(mindist == DBL_MAX || mindist >= S){
				Cluster clu(colLen);
				clu.addPoint(points[j]);
				clu.computeCentroid();
				//clu.addIndex(j);
				clusters.push_back(clu);
				deleted.push_back(j);
			}else if(mindist <= R){
				clusters[pos].addPoint(points[j]);
				//clusters[pos].addIndex(j);
				deleted.push_back(j);
			}
		}

		cout << "Iter:" << i << " -> Num. clusters:" << clusters.size() << endl;

		//cout << "Finish step 1" << endl;
		reverse(deleted.begin(), deleted.end());

		for(int j = 0; j < deleted.size(); j++){
			points.erase(points.begin() + deleted[j]);
		}

		deleted.clear();
		for(int j = 0; j < clusters.size(); j++){
			if(clusters[j].getSize() >= Nm)
				clusters[j].computeCentroid();
			else{
				//cout << "?" << endl;
				clusters[j].removePoints(points);
				deleted.push_back(j);
			}
		}

		reverse(deleted.begin(), deleted.end());
		for(int j = 0; j < deleted.size(); j++){
			clusters.erase(clusters.begin() + deleted[j]);
		}
	}

	cout << "Clusters num ->" << clusters.size()<< endl;

	if(option){
		//Unclustered points
		for(int i = 0; i < points.size(); i++){
			double mindist = DBL_MAX;
			int pos;

			for(int j = 0; j < clusters.size();j++){
				PointND cen = clusters[j].getCentroid();
				double dist = clusters[j].computeDistance(cen, points[i]);

				if(dist < mindist){
					mindist = dist;
					pos = j;
				}
			}
			clusters[pos].addPoint(points[i]);
		}
		points.clear();
	}
}
}
