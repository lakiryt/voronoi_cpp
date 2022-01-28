#pragma once

#include "Util.h"

// BeachLine.h
class BeachLine;
struct Arc;

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
    EventQueue(std::vector<Coord> sites = {})
    {
        for (Coord c : sites) {
            SiteEvent* new_site = new SiteEvent();
            new_site->position = c;
            std::variant<SiteEvent*, CircleEvent*> i = new_site;
            events.push({ i });
        }
    };

    bool isEmpty()
    {
        return events.empty();
    }

    Event pop()
    {
        Event e = events.top();
        events.pop();
        return e;
    }

    void push(Event e)
    {
        events.push(e);
    }

private:
    std::priority_queue<Event, std::vector<Event>, CompareY_gt> events;
};
