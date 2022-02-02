#pragma once

#include "Util.h"

// BeachLine.h
class BeachLine;

struct SiteEvent
{
    Coord position;
};

struct CircleEvent
{
    Coord position;
    Coord circleCenter;
    BeachLine* disappearingArc;
    bool valid;
};

struct Event
{
    std::variant<SiteEvent*, CircleEvent*> i;
};

struct CompareY_gt {
    bool operator()(Event const& e1, Event const& e2)
    {
        Coord c1, c2;
        std::visit([&](auto e1i, auto e2i) {
            c1 = e1i->position;
            c2 = e2i->position;
            }, e1.i, e2.i);
        return c1.y < c2.y;
    }
};

class EventQueue
{
public:
    EventQueue(std::vector<Coord> sites = {});

    bool isEmpty() { return events.empty(); }
    Event top() { return events.top(); }
    Event pop();
    void push(Event e);
    size_t size() { return events.size(); }

private:
    std::priority_queue<Event, std::vector<Event>, CompareY_gt> events;
};
