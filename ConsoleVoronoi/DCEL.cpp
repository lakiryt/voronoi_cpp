#include <iostream>
#include "BeachLine.h"

DCEL::DCEL()
{
	// this->faces.push_back(new Face());
}

void DCEL::display()
{
	std::cout << "[DCEL] with " << this->edges.size() << " half-edges and " << this->vertices.size() << " vertices:\n";
	for (Vertex* v : vertices)
	{
		std::cout << "(" << v->getPosition().x << "," << v->getPosition().y << ")";
	}
	std::cout << "\n";
	for (HalfEdge* e : edges)
	{
		std::cout << "[" << e << "]:" << e->getTwin() << ":";
		if (e->getOrigin())
		{
			Coord opos = e->getOrigin()->getPosition();
			std::cout << "(" << opos.x << "," << opos.y << ")->*";
		}
		std::cout << "\n";
	}
}

// Assume sites to be non-empty
void DCEL::createBoundingBox(std::vector<Coord> sites, std::vector<BreakPoint> halfinfinites)
{
	// Compute bounding box
	Coord minBound, maxBound;
	// Include all sites (to also handle empty/2sites case)
	minBound = sites.at(0);
	maxBound = minBound;
	for (const Coord &p : sites)
	{
		minBound = { std::min(p.x, minBound.x), std::min(p.y, minBound.y) };
		maxBound = { std::max(p.x, maxBound.x), std::max(p.y, maxBound.y) };
	}
	for (Vertex* v : vertices)
	{
		minBound = { std::min(v->getPosition().x, minBound.x), std::min(v->getPosition().y, minBound.y) };
		maxBound = { std::max(v->getPosition().x, maxBound.x), std::max(v->getPosition().y, maxBound.y) };
	}
	minBound = { minBound.x - 1, minBound.y - 1 };
	maxBound = { maxBound.x + 1, maxBound.y + 1 };

	for (const BreakPoint &bp : halfinfinites)
	{
		// Find intersection point with boundary
		Coord intersection;
		if (std::abs(bp.siteLeft.y - bp.siteRight.y) < DBL_EPSILON)
		{
			if (bp.siteLeft.x < bp.siteRight.x)
				// intersection with bottom line
				intersection = { (bp.siteLeft.x + bp.siteRight.x) / 2, minBound.y };
			else // if (bp.siteLeft.x > bp.siteRight.x) // TODO: check example of case in which this exists
				// intersection with top line
				intersection = { (bp.siteLeft.x + bp.siteRight.x) / 2, maxBound.y };
		}
		else
		{
			double a = -(bp.siteRight.x - bp.siteLeft.x) / (bp.siteRight.y - bp.siteLeft.y); // negative inverse rise over run
			double b = (bp.siteLeft.y + bp.siteRight.y) / 2 - a * ((bp.siteLeft.x + bp.siteRight.x) / 2); // midpoint (x,y) satisfies ax+b=y

			// intersection with vertical (left/right) lines (guaranteed to exist in this case-distinction branch)
			if (bp.siteLeft.y < bp.siteRight.y)
				// intersection with right border
				intersection = { maxBound.x , a * (maxBound.x) + b };
			else
				// intersection with left border
				intersection = { minBound.x , a * (minBound.x) + b };

			// switch to intersection with horizontal lines, if needed
			if (intersection.y < minBound.y || maxBound.y < intersection.y)
			{
				if (bp.siteLeft.x < bp.siteRight.x)
					// bottom intersection
					intersection = { (minBound.y - b) / a, minBound.y };
				else
					// top intersection
					intersection = { (maxBound.y - b) / a, maxBound.y };
			}
		}
		// Create and connect new intersection point
		Vertex* new_vtx = this->createNewVertex(intersection);
		bp.bisector->getTwin()->setOrigin(new_vtx);
		new_vtx->setIncident(bp.bisector->getTwin());
	}
}