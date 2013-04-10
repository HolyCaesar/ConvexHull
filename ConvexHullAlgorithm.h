#pragma once

#include "Header.h"
#include "GeoDefinition.h"
#include "DCEL.h"

class ConvexHullAlgorithm
{
public:
	ConvexHullAlgorithm();
	~ConvexHullAlgorithm();
public:
	/* Only used when the point number of a point set below 8 */
	virtual void BruceForceCH( vector<VERTEX>* pVertex ) = 0;

private:

protected:
	//TODO what list do we need here?
	DCEL* m_pDCEL;
};

