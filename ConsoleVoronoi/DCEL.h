#pragma once
#include<vector>
#include<queue>
#include<variant>

struct Coord
{
    double x;
    double y;
};

class HalfEdge;

class Vertex
{
public:
    // new vertex without incident edge.
    Vertex(Coord c = { 0, 0 })
    {
        this->position = c;
        this->incidentEdge = NULL;
    }
    void setIncident(HalfEdge* halfedge)
    {
        incidentEdge = halfedge;
    }

private:
    Coord position;
    HalfEdge* incidentEdge;
};

/*
class Face
{
public:
    // New face with no (null pointer) boundary.
    Face() {};

private:
    HalfEdge* randomCounterClockwise;
};
*/

class HalfEdge
{
public:
    HalfEdge(Vertex* origin = NULL) {
        this->origin = origin;
        this->twin = NULL;
        this->next = NULL;
        this->prev = NULL;
    };
    void setTwin(HalfEdge* newTwin) {
        this->twin = newTwin;
        newTwin->twin = this;
    };
    void setNext(HalfEdge* newNext) {
        this->next = newNext;
        newNext->prev = this;
    };
    void setPrev(HalfEdge* newPrev) {
        this->prev = newPrev;
        newPrev->next = this;
    };
private:
    HalfEdge* twin;
    HalfEdge* next;
    HalfEdge* prev;
    Vertex* origin;
    // Face* faceLeft;
};

class DCEL
{
public:
    // Generates empty DCEL with one face.
    DCEL();
    // void addVertex(Coord c)
    // {
    //    vertices.push_back(new Vertex(c));
    // }
    HalfEdge* createNewEdge()
    {
        HalfEdge* edge = new HalfEdge();
        edges.push_back(edge);
        return edge;
    }

    void display();

private:
    std::vector<Vertex*> vertices;
    // std::vector<Face*> faces;
    std::vector<HalfEdge*> edges;
};
