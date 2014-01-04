#include "vectorfunctions.h"

using namespace std;

const int COORDS = 3;

vector< double > crossProduct(vector< double > a, vector< double > b)
{
	vector< double > retval;
	for (int i = 0; i < COORDS; ++i)
	{
		retval.push_back(0);
	}

	retval[0] = a[1]*b[2] - a[2]*b[1];
	retval[1] = -1*(a[0]*b[2] - a[2]*b[0]);
	retval[2] = a[0]*b[1] - a[1]*b[0];

	return retval;
}

vector< double > add(vector< double > a, vector< double > b)
{
	vector< double > c;
	for (int i = 0; i < COORDS; ++i)
	{
		c.push_back(a[i] + b[i]);
	}
	return c;
}

vector< double > subtract(vector< double > a, vector< double > b)
{
	vector< double > c;
	for (int i = 0; i < COORDS; ++i)
	{
		c.push_back(a[i] - b[i]);
	}
	return c;
}

double length(vector< double > a)
{
	double temp = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	return pow(temp, .5);
}

vector< double > normalize(vector< double > a)
{
	vector< double > retval;
	double temp = length(a);

	for (int i = 0; i < COORDS; ++i)
	{
		retval.push_back(a[i]/temp);
	}
	
	return retval;
}
