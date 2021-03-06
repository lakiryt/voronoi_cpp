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
	Coord position(double sweepLineY);
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
		parent = NULL;
	};

	void display();

	void insert(double x, double sweepLineY, DCEL* interim_diag, EventQueue* event_queue);
	void shrink(CircleEvent* e, DCEL* interim_diag, EventQueue* event_queue);

	// Initial same-y's
	void handleSameY(Coord leftSite, Coord rightSite, DCEL* interim_diag);
	void insertSameY(Coord site, DCEL* interim_diag);

	std::vector<BreakPoint> getHalfInfinites();

private:
	void setData(std::variant<Arc*, BreakPoint> d) {
		data = d;
		updateHeight();
	}
	void setLeftChild(BeachLine* l) {
		left = l;
		l->parent = this;
		updateHeight();
	}
	void setRightChild(BeachLine* r) {
		right = r;
		r->parent = this;
		updateHeight();
	}
	void updateHeight();
	void checkCircle(Arc* arcdata, double y, EventQueue* evq);

	std::optional<std::variant<Arc*, BreakPoint>> data;
	int height;

	BeachLine* parent;
	BeachLine* left;
	BeachLine* right;
};

