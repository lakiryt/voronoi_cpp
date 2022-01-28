#include <iostream>
#include "DCEL.h"

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
}