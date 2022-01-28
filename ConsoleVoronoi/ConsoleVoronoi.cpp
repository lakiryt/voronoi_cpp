// ConsoleVoronoi.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "BeachLine.h"
#include <iostream>


DCEL diagram;
BeachLine beach;
EventQueue events;

void handleSiteEvent(SiteEvent*);
void handleCircleEvent(CircleEvent*);

int main()
{
    std::cout << "Voronoi Test\n\n";

    // std::vector<Coord> input = { {0,0}, {0,1}, {3,0} };
    std::vector<Coord> input = { {0.5,0}, {0,1}, {-1,-1} };

    beach = BeachLine();
    diagram = DCEL();
    events = EventQueue(input);

    while (!events.isEmpty())
    {
        std::visit(overload{
            [](SiteEvent* e) {handleSiteEvent(e); },
            [](CircleEvent* e) { if (e->valid) handleCircleEvent(e); }
            }, events.pop().i);
    }
    // TODO: Compute bounding box & half infinite edge crossing it
    // TODO: Add cell records
    // beach.display();
}

void handleSiteEvent(SiteEvent* e)
{
    std::cout << "\n * Insert (" << e->position.x << "," << e->position.y << ")...\n";
    beach.insert(e->position.x, e->position.y, &diagram, &events);
    // beach.display();
}

// Assume validity
void handleCircleEvent(CircleEvent* e)
{
    std::cout << "Circle event detected.";
    e->disappearingArc->shrink(e, &diagram);
}