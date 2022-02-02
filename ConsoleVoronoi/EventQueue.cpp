#include "EventQueue.h"

EventQueue::EventQueue(std::vector<Coord> sites)
{
    for (const Coord& c : sites) {
        SiteEvent* new_site = new SiteEvent();
        new_site->position = c;
        std::variant<SiteEvent*, CircleEvent*> i = new_site;
        events.push({ i });
    }
}

Event EventQueue::pop()
{
    Event e = events.top();
    events.pop();
    return e;
}

void EventQueue::push(Event e)
{
    /*
    std::cout << "Pushed ";
    std::visit(overload{
        [](SiteEvent* ei) {std::cout << "site (" << ei->position.x << "," << ei->position.y << ")\n"; },
        [](CircleEvent* ei) {std::cout << "circle (" << ei->position.x << "," << ei->position.y << ")\n"; }
        }, e.i);
    */
    events.push(e);
}
