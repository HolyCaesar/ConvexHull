#include "ConvexHullAlgorithm.h"

ConvexHullAlgorithm::ConvexHullAlgorithm()
{
	m_pDCEL = new DCEL;
}

ConvexHullAlgorithm::~ConvexHullAlgorithm()
{
	if( m_pDCEL )
	{
		delete m_pDCEL;
	}
}