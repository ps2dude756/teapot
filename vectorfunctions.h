#ifndef __VECTORFUNCTIONS_H__
#define __VECTORFUNCTIONS_H__

#include <vector>
#include <cmath>

using namespace std;

vector< double > crossProduct(vector< double > a, vector< double > b);
vector< double > add(vector< double > a, vector< double > b);
vector< double > subtract(vector< double > a, vector< double > b);
double length(vector< double > a);
vector< double > normalize(vector< double > a);

#endif
