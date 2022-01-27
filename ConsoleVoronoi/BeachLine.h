#pragma once

#include <optional>
#include<algorithm>
#include "DCEL.h"
#include "EventQueue.h"

struct Arc {
	Coord siteAbove;
	CircleEvent* disappear;

	// ?? For circle checking
	Arc* left;
	Arc* right;
};

class BreakPoint {
public:
	double xCoord(double sweepLineY);
	double yCoord(double sweepLineY);
	HalfEdge* bisector;
	Coord siteLeft;
	Coord siteRight;
};

class BeachLine
{
public:
	BeachLine() {
		height = 0;
		data = std::nullopt;
		left = NULL;
		right = NULL;
	};

	void display();

	void insert(double x, double sweepLineY, DCEL* interim_diag, EventQueue* event_queue);

private:
	void setData(std::variant<Arc*, BreakPoint> d) {
		data = d;
		updateHeight();
	}
	void setLeftChild(BeachLine* l) {
		left = l;
		updateHeight();
	}
	void setRightChild(BeachLine* r) {
		right = r;
		updateHeight();
	}
	void updateHeight();

	std::optional<std::variant<Arc*, BreakPoint>> data;
	int height;

	BeachLine* left;
	BeachLine* right;
};

