#include <iostream>
#include "BeachLine.h"


Coord BreakPoint::position(double sweepLineY)
{
	double a = -(siteRight.x - siteLeft.x) / (siteRight.y - siteLeft.y);
	double b = (siteRight.y + siteLeft.y) / 2 - a * (siteRight.x + siteLeft.x) / 2;

	// ax+b = |(x,y)-siteLeft| = |(x,y)-siteRight| = y
	// (x-siteLeft.x)^2 + (y-siteLeft.y)^2 = y^2
	// (x^2 - 2*x*siteLeft.x + siteLeft.x^2) -2*y*siteLeft.y + siteLeft.y^2 = 0
	// (x^2 - 2*x*siteLeft.x + siteLeft.x^2) -2*(ax+b)*siteLeft.y + siteLeft.y^2 = 0
	// x^2 - 2*(siteLeft.x + a*siteLeft.y)*x + siteLeft.x^2 -2*b*siteLeft.y + siteLeft.y^2 = 0
	double solb = -2 * (siteLeft.x + a * siteLeft.y);
	double solc = std::pow(siteLeft.x, 2) - 2 * b * siteLeft.y + std::pow(siteLeft.y, 2);
	double discriminant = std::sqrt(std::pow(solb, 2) + 4 * solc);
	double factor;
	if (siteLeft.x < siteRight.x) factor = 1; else factor = -1;
	double solx = (-solb - factor * discriminant) / 2;

	return { solx, a * solx + b };
}
/*
double BreakPoint::yCoord(double sweepLineY)
{
	auto parabola1 = [&](double x) {
		// (x - p1.x)^2 + (y - p1.y)^2 = (y-sweepLineY)^2
		return (pow(x - siteLeft.x, 2) + pow(siteLeft.y, 2) - pow(sweepLineY, 2)) / (2 * (siteLeft.y - sweepLineY));
	};

	return parabola1(xCoord(sweepLineY));
}
*/


/// <summary>
/// BeachLine
/// </summary>

void BeachLine::display()
{
	std::cout << "Height: " << height << "\n";
	if (data.has_value())
	{
		std::visit(overload{
			[](Arc* a) {
				std::cout << "Arc of site (" << a->siteAbove.x << "," << a->siteAbove.y << ")\n";
				if (a->disappear != NULL)
				std::cout << "    potentially disappearing at (" << a->disappear->position.x << "," << a->disappear->position.y << ")\n";
				if (a->left)
					std::cout << "Left neighbour: (" << a->left->siteAbove.x << "," << a->left->siteAbove.y << ")\n";
				if (a->right)
					std::cout << "Right neighbour: (" << a->right->siteAbove.x << "," << a->right->siteAbove.y << ")\n";
			},
			[](BreakPoint b) {
				std::cout << "BreakPt between (" << b.siteLeft.x << ", " << b.siteLeft.y << ") and (" << b.siteRight.x << ", " << b.siteRight.y << ")\n";
				std::cout << "        with ";
				if (b.bisector == NULL)
					std::cout << "no ";
				std::cout << "HalfEdge set.\n";
			}
			}, data.value());
	}

	if (left != NULL) {
		std::cout << ">>Left\n";
		left->display();
		std::cout << "<<Left\n";
	}
	if (right != NULL) {
		std::cout << ">>Right\n";
		right->display();
		std::cout << "<<Right\n";
	}
}

void BeachLine::updateHeight()
{
	int rh, lh;
	if (left == NULL)
	{
		lh = 0;
	}
	else
	{
		lh = left->height;
	}

	if (right == NULL)
	{
		rh = 0;
	}
	else
	{
		rh = right->height;
	}

	this->height = std::max(lh, rh) + 1;
}


std::optional<Coord> center3(Coord p1, Coord p2, Coord p3)
{
	//std::cout << "Calculating the center of the points:\n("
	//	<< p1.x << "," << p1.y << ") (" << p2.x << "," << p2.y << ") (" << p3.x << "," << p3.y << ")\n";
	// Line between p1 and p2, described by equation y = a12 * x + b12
	double a12 = -(p2.x - p1.x) / (p2.y - p1.y); // negative inverse rise-over-run from p1 to p2
	double b12 = (p1.y + p2.y) / 2 - a12 * (p1.x + p2.x) / 2; // midpoint should satisfy equation

	// do the same for p2 and p3
	double a23 = -(p3.x - p2.x) / (p3.y - p2.y);
	double b23 = (p2.y + p3.y) / 2 - a23 * (p2.x + p3.x) / 2;

	//std::cout << "Intersection between lines ";
	//std::cout << a12 << "x + " << b12 << " = y and ";
	//std::cout << a23 << "x + " << b23 << " = y.";

	// Handle collinearity // ??
	if (std::abs(a12 - a23) < DBL_EPSILON)
		return std::nullopt;

	// Intersection of y = a12 * x + b12 and y = a23 * x + b23
	double x = (b23 - b12) / (a12 - a23);
	double y = a12 * x + b12; // = a23 * x + b23;

	//std::cout << "(" << x << "," << y << ")";
	return std::optional<Coord>({ x,y });
}

void BeachLine::checkCircle(Arc* arc, double sweepLineY, EventQueue* event_queue)
{
	// Check for validity
	// view from site left.
	Coord middle_ = { arc->siteAbove.x - arc->left->siteAbove.x , arc->siteAbove.y - arc->left->siteAbove.y };
	Coord right_ = { arc->right->siteAbove.x - arc->left->siteAbove.x, arc->right->siteAbove.y - arc->left->siteAbove.y };
	Coord right_ortho = { -right_.y, right_.x };
	double dot_prod = right_ortho.x * middle_.x + right_ortho.y * middle_.y;
	if (dot_prod <= 0) return; // 0??

	std::optional<Coord> center = center3(arc->left->siteAbove, arc->siteAbove, arc->right->siteAbove);
	if (center.has_value())
	{
		double sq_dist = std::pow(center.value().x - arc->siteAbove.x, 2) + std::pow(center.value().y - arc->siteAbove.y, 2);
		double event_y = center.value().y - std::sqrt(sq_dist);
		if (event_y <= sweepLineY) // TODO: handle '==' case (four points on circle)
		{
			Coord ev_pos = { center.value().x, event_y };
			CircleEvent* cev = new CircleEvent{ ev_pos, center.value(), this, true };
			event_queue->push({ cev });
		}
	}

}

void BeachLine::insert(double x, double sweepLineY, DCEL* interim_diag, EventQueue* event_queue)
{
	if (!data.has_value()) // empty beach
	{
		height = 1;
		Arc* arc = new Arc{ { x,sweepLineY }, NULL, NULL, NULL };
		data = std::make_optional(arc);
	}
	else
	{ // TODO: balance
		std::variant<Arc*, BreakPoint> data_i = data.value();

		std::visit(overload{
			[&](Arc* a) {
				// leaf

				// ?? Check if edge case ok (left and right arcs above are different)

				// Delete false alarm (Invalidate)
				// delete a.disappear; // ??
				if (a->disappear)
				{
					a->disappear->valid = false;
					a->disappear = NULL;
				}

				// # construct subtree #
				Coord outerSite = a->siteAbove;
				Coord innerSite = { x, sweepLineY };
				// Arcs
				Arc* leftBrokenArc = new Arc{ outerSite, NULL, a->left, NULL };
				Arc* newMiddleArc = new Arc{ innerSite, NULL, leftBrokenArc, NULL };
				Arc* rightBrokenArc = new Arc{ outerSite, NULL, newMiddleArc, a->right };
				leftBrokenArc->right = newMiddleArc;
				newMiddleArc->right = rightBrokenArc;
				if (a->left) a->left->right = leftBrokenArc;
				if (a->right) a->right->left = rightBrokenArc;
				// BreakPoints
				BreakPoint left_bp = BreakPoint();
				left_bp.siteLeft = outerSite;
				left_bp.siteRight = innerSite;
				BreakPoint right_bp = BreakPoint();
				right_bp.siteLeft = innerSite;
				right_bp.siteRight = outerSite;
				// New Halfedges
				HalfEdge* leftEdge = interim_diag->createNewEdge();
				HalfEdge* rightEdge = interim_diag->createNewEdge();
				leftEdge->setTwin(rightEdge);
				left_bp.bisector = leftEdge;
				right_bp.bisector = rightEdge;

				// Beaches
				left = new BeachLine(); // Left Arc
				left->parent = this;
				// this // Left BPt
				BeachLine* rleft = new BeachLine(); // Middle Arc
				right = new BeachLine(); // Right BPt
				right->parent = this;
				BeachLine* rright = new BeachLine(); // Right Arc
				// set data and beach relations
				left->setData(leftBrokenArc);
				this->setData(left_bp);
				rleft->setData(newMiddleArc);
				right->setData(right_bp);
				rright->setData(rightBrokenArc);
				right->setLeftChild(rleft);
				right->setRightChild(rright);

				// Check left triple for circle
				if (a->left)
					left->checkCircle(leftBrokenArc, sweepLineY, event_queue);
				// Check right triple for circle
				if (a->right)
					rright->checkCircle(rightBrokenArc, sweepLineY, event_queue);
			},
			[&](BreakPoint b) {
				// internal node
				if (x < b.position(sweepLineY).x)
				{
					left->insert(x, sweepLineY, interim_diag, event_queue);
				}
				else // ??
				{
					right->insert(x, sweepLineY, interim_diag, event_queue);
				}
			}
			}, data_i);
		updateHeight();
	}
}

// Assume this is an arc surrounded with neighbours
void BeachLine::shrink(CircleEvent* e, DCEL* interim_diag, EventQueue* event_queue)
{
	std::cout << "\nshrink() called on:\n";
	this->display();

	// (Delete leaf)
	// Detach this and this->parent from tree
	BeachLine* foldpoint;
	BeachLine* former_sibling;
	BreakPoint left_bp_data, right_bp_data;
	BeachLine* left_arc_beach;
	BeachLine* right_arc_beach;
	if (this->parent->left == this)
	{
		former_sibling = this->parent->right;

		foldpoint = this->parent;
		while (foldpoint->parent->right != foldpoint)
			foldpoint = foldpoint->parent;
		// Now, foldpoint->parent->right == foldpoint, i.e. foldpoint is the right child of its parent.
		BeachLine* left_bp = foldpoint->parent;

		if (this->parent->parent == left_bp)
		{
			left_bp->setRightChild(former_sibling);
		} 
		else
		{
			this->parent->parent->setLeftChild(former_sibling);
		}
		former_sibling->parent = this->parent->parent;

		// Retrieve old bp data
		left_bp_data = std::get<BreakPoint>(left_bp->data.value());
		right_bp_data = std::get<BreakPoint>(this->parent->data.value());

		left_arc_beach = left_bp->left;
		while (left_arc_beach->right)
			left_arc_beach = left_arc_beach->right;
		right_arc_beach = former_sibling;
		while (right_arc_beach->left)
			right_arc_beach = right_arc_beach->left;
	}
	else // this->parent->right == this
	{
		former_sibling = this->parent->left;

		foldpoint = this->parent;
		while (foldpoint->parent->left != foldpoint)
			foldpoint = foldpoint->parent;
		// Now, search->parent->left == search, i.e. search is the left child of its parent.
		BeachLine* right_bp = foldpoint->parent;

		if (this->parent->parent == right_bp)
		{
			right_bp->setLeftChild(former_sibling);
		}
		else
		{
			this->parent->parent->setRightChild(former_sibling);
		}
		former_sibling->parent = this->parent->parent;

		right_bp_data = std::get<BreakPoint>(right_bp->data.value());
		left_bp_data = std::get<BreakPoint>(this->parent->data.value());

		// Get pointers to the beach lines of the neighbour arcs
		left_arc_beach = former_sibling;
		while (left_arc_beach->right)
			left_arc_beach = left_arc_beach->right;
		right_arc_beach = right_bp->right;
		while (right_arc_beach->left)
			right_arc_beach = right_arc_beach->left;
	}
	std::cout << "left breakpoint: " << left_bp_data.siteLeft.x << "," << left_bp_data.siteLeft.y << " | " << left_bp_data.siteRight.x << "," << left_bp_data.siteRight.y;
	if (left_bp_data.bisector) std::cout << " with "; else std::cout << " without ";
	std::cout << "bisector\n";
	std::cout << "right breakpoint: " << right_bp_data.siteLeft.x << "," << right_bp_data.siteLeft.y << " | " << right_bp_data.siteRight.x << "," << right_bp_data.siteRight.y;
	if (right_bp_data.bisector)std::cout << " with "; else std::cout << " without ";
	std::cout << "bisector\n";

	// Update breakpoint
	HalfEdge* in_edge = interim_diag->createNewEdge();
	HalfEdge* out_edge = interim_diag->createNewEdge();
	in_edge->setTwin(out_edge);
	BreakPoint new_bp = {out_edge, left_bp_data.siteLeft, right_bp_data.siteRight };
	foldpoint->parent->setData(new_bp);

	// Delete all events involving this arc ; detach this arc from neighbours
	Arc* thisArc = std::get<Arc*>(this->data.value());
	if (thisArc->left)
	{
		if (thisArc->left->disappear)
		{
			thisArc->left->disappear->valid = false;
			thisArc->left->disappear = NULL;
		}

		thisArc->left->right = thisArc->right;

	}
	if (thisArc->right)
	{
		if (thisArc->right->disappear)
		{
			thisArc->right->disappear->valid = false;
			thisArc->right->disappear = NULL;
		}

		thisArc->right->left = thisArc->left;
	}
	// Check for new circles
	if (thisArc->left)
		if (thisArc->left->left)
			left_arc_beach->checkCircle(thisArc->left, e->position.y, event_queue);
	if (thisArc->right)
		if (thisArc->right->right)
			right_arc_beach->checkCircle(thisArc->right, e->position.y, event_queue);

	// Add vertex and set pointers
	Vertex* new_vtx = interim_diag->createNewVertex(e->circleCenter);
	new_vtx->setIncident(out_edge);
	out_edge->setOrigin(new_vtx);
	// Connect old HalfEdges to vertex
	left_bp_data.bisector->getTwin()->setOrigin(new_vtx);
	right_bp_data.bisector->getTwin()->setOrigin(new_vtx);
	// Connect edges
	right_bp_data.bisector->setNext(out_edge);
	left_bp_data.bisector->getTwin()->setPrev(in_edge);

	// updateHeight for all ancestors
	BeachLine* search = former_sibling;
	while (search->parent)
	{
		search = search->parent;
		search->updateHeight();
	}

	delete this->parent;
	delete this;
}

std::vector<BreakPoint> BeachLine::getHalfInfinites()
{
	std::vector<BreakPoint> res;

	if (this->data.has_value())
	{
		std::visit(overload{ [](Arc* a) {}, [&](BreakPoint b) {
			res.push_back(b);
			//std::cout << "pbok\n";
			for (const BreakPoint &bp : this->left->getHalfInfinites())
				res.push_back(bp);
			for (const BreakPoint &bp : this->right->getHalfInfinites())
				res.push_back(bp);
			/*
			res.insert(
				res.end(),
				std::make_move_iterator(this->left->getHalfInfinites().begin()),
				std::make_move_iterator(this->left->getHalfInfinites().end())
			);
			std::cout << "in1ok\n";
			res.insert(
				res.end(),
				std::make_move_iterator(this->right->getHalfInfinites().begin()),
				std::make_move_iterator(this->right->getHalfInfinites().end())
			);
			std::cout << "in2ok\n";
			*/
			} }, this->data.value());
	}

	return res;
}
