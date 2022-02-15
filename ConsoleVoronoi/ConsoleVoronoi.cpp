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

    // std::vector<Coord> input = { {0,0}, {0,1}, {3,0} };
    std::vector<Coord> input = { {0.5,0}, {0,1}, {-1,-1} };
    std::cout << "input = [";
    for (const Coord& c : input)
        std::cout << " " << c.x << "," << c.y << ";";
    std::cout << "\b];\n";
    std::cout << "fh  = figure;\nah = axes(fh);\nhold(ah, 'on');\n";
    std::cout << "plot(ah, input(:,1), input(:,2), '.')\n";

    beach = BeachLine();
    diagram = DCEL();
    events = EventQueue(input);

    // Handle same y-coordinate at the beginning
    if (events.size() >= 2)
    {
        double initial_y;
        Event e1 = events.pop();
        Event e2 = events.pop();
        SiteEvent* e1i = std::get<SiteEvent*>(e1.i);
        SiteEvent* e2i = std::get<SiteEvent*>(e2.i);
        if (std::abs(e1i->position.y - e2i->position.y) < DBL_EPSILON)
        {
            if (e1i->position.x < e2i->position.x)
                beach.handleSameY(e1i->position, e2i->position, &diagram);
            else
                beach.handleSameY(e2i->position, e1i->position, &diagram);
            if (!events.isEmpty()) {
                initial_y = e1i->position.y;
                // Handle succesive same y-coordinates
                while (!events.isEmpty() && std::abs(std::get<SiteEvent*>(events.top().i)->position.y - initial_y) < DBL_EPSILON)
                {
                    beach.display();
                    beach.insertSameY(std::get<SiteEvent*>(events.pop().i)->position, &diagram);
                }
            }
        }
        else
        {
            events.push(e2);
            events.push(e1);
        }
        beach.display();
    }

    while (!events.isEmpty())
    {
        std::visit(overload{
            [](SiteEvent* e) {handleSiteEvent(e); },
            [](CircleEvent* e) { if (e->valid) handleCircleEvent(e); }
            }, events.pop().i);
    }
    // Compute bounding box & half infinite edge crossing it
    diagram.createBoundingBox(input, beach.getHalfInfinites());

    // TODO: Add cell records
    // beach.display();
    diagram.display();
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
    std::cout << "\n * Circle (" << e->circleCenter.x <<"," << e->circleCenter.y << ") event at (" << e->position.x << "," << e->position.y << ") detected.\n";
    e->disappearingArc->shrink(e, &diagram, &events);
}