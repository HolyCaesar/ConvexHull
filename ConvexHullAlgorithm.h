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

private:
	/* Only used when the point number of a point set below 8 */
	virtual DCEL BruceForceCH( vector<VERTEX*>* pVertex, const unsigned int offset ) = 0;
protected:
	//TODO what list do we need here?
	DCEL* m_pDCEL;
};