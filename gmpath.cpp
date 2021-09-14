#define fn_export extern "C" __declspec (dllexport)

#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

fn_export double test_function()
{
	double d = 2.0;
	return d;
}

class Node {
private:
	int xval;
	int yval;
public:
	double value = 1.0;
	double f = 0;
	double g = 0;
	int raw_length = 0;
	bool checked = false;
	Node* prev = nullptr;
	Node(int x, int y) {
		xval = x;
		yval = y;
	}
	double getx() {
		return (double)xval;
	}
	double gety() {
		return (double)yval;
	}
	void set_h(Node* _prev, int xg, int yg, double cost) {//set heuristics
		prev = _prev;
		if (prev == nullptr) {
			f = 0;
			raw_length = 0;
		}
		else {
			f = prev->f + cost;
			raw_length = prev->raw_length + 1;
		}
		//calculate g value from euclidean distance
		int xdif = abs(xg - xval);
		int ydif = abs(yg - yval);
		g = sqrt(xdif*xdif + ydif * ydif);
		checked = true;
	}
	bool operator<(const Node& n) {
		return (f + g < n.f + n.g);
	}
	bool operator>(const Node& n) {
		return (f + g > n.f + n.g);
	}
	bool operator<=(const Node& n) {
		return (f + g <= n.f + n.g);
	}
	bool operator>=(const Node& n) {
		return (f + g >= n.f + n.g);
	}
	bool operator==(const Node& n) {
		return (f + g == n.f + n.g);
	}
};

/*
	The path class is used to store pathing information once a path has been created. For speed efficiency, paths are stored in reverse order
	such that the last element in the path vector is the next node a pathing instance will go to.
*/
class Path {
private:
	vector<Node*> path;
public:
	Path(Node* n) {
		while (n != nullptr && n->prev != nullptr) {
			path.push_back(n);
			n = n->prev;
		}
	}
	Node* get_front() {
		if (path.empty()) {
			return nullptr;
		}
		return path[path.size() - 1];
	}
	Node* get_index(int index) {
		if (index >= path.size() || index < 0) {
			return nullptr;
		}
		return path[path.size() - index - 1];
	}
	double get_front_x() {
		if (get_front() == nullptr) {
			return -1;
		}
		return get_front()->getx();
	}
	double get_front_y() {
		if (get_front() == nullptr) {
			return -1;
		}
		return get_front()->gety();
	}
	double get_x(int index) {
		if (get_index(index) == nullptr) {
			return -1;
		}
		return get_index(index)->getx();
	}
	double get_y(int index) {
		if (get_index(index) == nullptr) {
			return -1;
		}
		return get_index(index)->gety();
	}
	double get_length() {
		return path.size();
	}
	void pop() {
		if (!path.empty()) {
			path.pop_back();
		}
	}
};

Path* get_path(double path_addr) {
	Path* p = nullptr;
	memcpy(&p, &path_addr, sizeof(p));
	return p;
}

fn_export double path_get_length(double path_addr) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return 0;
	}
	return p->get_length();
}

fn_export double path_get_xfirst(double path_addr) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return -1;
	}
	return p->get_front_x();
}

fn_export double path_get_yfirst(double path_addr) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return -1;
	}
	return p->get_front_y();
}

fn_export double path_get_x(double path_addr, double index) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return -1;
	}
	return p->get_x((int)index);
}

fn_export double path_get_y(double path_addr, double index) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return -1;
	}
	return p->get_y((int)index);
}

fn_export void path_pop(double path_addr) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return;
	}
	p->pop();
}

fn_export void path_destroy(double path_addr) {
	Path* p = get_path(path_addr);
	if (p == nullptr) {
		return;
	}
	delete p;
}

class Grid {
private:
	vector<vector<Node>> _grid;
	int _w = 0;
	int _h = 0;
public:
	//constructor
	Grid(int width, int height) {
		_w = max(0, width);
		_h = max(0, height);
		for (int i = 0; i < _w; i++) {
			vector<Node> v;
			for (int j = 0; j < _h; j++) {
				Node n(i, j);
				v.push_back(n);
			}
			_grid.push_back(v);
		}
	}
	//destructor
	~Grid() {
		for (vector<Node> outer : _grid) {
			for (Node inner : outer) {
				delete &inner;
			}
		}
	}
	//accessors
	int get_width() {
		return _w;
	}
	int get_height() {
		return _h;
	}
	double get_val(int x, int y) {
		if (x >= _w || x < 0 || y >= _h || y < 0) {
			return -1;
		}
		return _grid[x][y].value;
	}
	Node* get_node(int x, int y) {
		if (x >= _w || x < 0 || y >= _h || y < 0) {
			return nullptr;
		}
		return &_grid[x][y];
	}
	//value change
	void set_val(int x, int y, double val) {
		_grid[x][y].value = val;
		return;
	}
};

//access grid object from address double
Grid* get_grid(double grid_addr) {
	Grid* g = nullptr;
	memcpy(&g, &grid_addr, sizeof(g));
	return g;
}

//create grid
fn_export double make_grid(double w, double h) {
	//create the grid
	Grid* new_grid = new Grid(int(w), int(h));
	//cast the address of the grid to a double to return to gamemaker
	double grid_addr = 1.0;
	memcpy(&grid_addr, &new_grid, sizeof(grid_addr));
	return grid_addr;
	/* Old code for turning the address into a string
	stringstream ss;
	ss << address;
	static string s;
	s = ss.str();
	return (char*)s.c_str();
	*/
}

//destroy grid
fn_export void destroy_grid(double addr) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return;
	}
	delete g;
}

//accessors
fn_export double get_grid_width(double addr) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return 0;
	}
	return (double)g->get_width();
}

fn_export double get_grid_height(double addr) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return 0;
	}
	return (double)g->get_height();
}

fn_export double get_grid_value(double addr, double x, double y) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return -1;
	}
	if (x < 0 || y < 0 || x >= g->get_width() || y >= g->get_height()) {
		//out of bounds
		return -1;
	}
	return (double)g->get_val((int)x, (int)y);
}

//value changes
/*
*	set_grid_value
*	returns: double
*	This function sets a given value in a grid object to a new double value. It returns a double representing a boolean of whether or not the
*	new value was set successfully.
*/
fn_export double set_grid_value(double addr, double x, double y, double new_value) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return 0;
	}
	if (x < 0 || y < 0 || x >= g->get_width() || y >= g->get_height()) {
		//out of bounds
		return 0;
	}
	g->set_val((int)x, (int)y, new_value);
	return 1;
}

//overload if we've already got a grid object pointer
double set_grid_value(Grid* g, double x, double y, double new_value) {
	if (g == nullptr) {
		return 0;
	}
	if (x < 0 || y < 0 || x >= g->get_width() || y >= g->get_height()) {
		//out of bounds
		return 0;
	}
	g->set_val((int)x, (int)y, new_value);
	return 1;
}

/*
*	set_grid_rect
*	returns: double
*	This function sets all values in a given rectangle to a new double value. It returns a double representing a boolean of whether or not the
*	new value was set successfully. This will be true if ANY node value was changed by the function.
*/
fn_export double set_grid_rect(double addr, double x, double y, double width, double height, double new_value) {
	Grid* g = get_grid(addr);
	if (g == nullptr) {
		return 0;
	}
	if (x + width < 0 || y + height < 0 || x >= g->get_width() || y >= g->get_height() || width <= 0 || height <= 0) {
		//out of bounds or illegal arguments
		return 0;
	}
	//we know at least one node will be updated. We've already safety-checked our set_grid_value function, so we can call with impunity
	for (int i = (int)max(x, 0.0); i < (int)min(x + width, (double)g->get_width()); i++) {
		for (int j = (int)max(y, 0.0); j < (int)min(y + height, (double)g->get_height()); j++) {
			set_grid_value(g, i, j, new_value);
		}
	}
	return 1;
}

//helper function for binary inserting into vector
//sorted high cost -> low cost
void binary_insert(vector<Node*>& v, Node* np) {
	if (v.empty()) {
		v.push_back(np);
		return;
	}
	int low = 0;
	int high = v.size() - 1;
	while (low != high) {
		int center = (high + low) / 2;
		Node* other = v[center];
		if (*np < *other) {
			if (low == center) {
				low++;
			}
			else {
				low = center;
			}
		}
		else {
			if (high == center) {
				high--;
			}
			else {
				high = center;
			}
		}
	}
	int offset = 0;
	if (*np < *v[low]) {
		offset++;
	}
	//if offset is 0, np will be inserted before the low value, if offset is one it will be inserted after
	v.insert(v.begin() + low + offset, np);
	return;
}

//helper function to insert a node into the open list if the node is valid and not present in either open or closed
//returns true if inserted, else false
bool insert_if_valid(bool diag_valid, Node* n, Node* prev, vector<Node*>& open, Node* goal, int max_len) {
	if (diag_valid &&
		n != nullptr &&
		n->checked == false &&
		n->value >= 0 &&
		(max_len < 0 || prev->raw_length < max_len)) {
		//unfortunately we can't mark out of range nodes as checked because they may be in range for a costlier path
		n->set_h(prev, goal->getx(), goal->gety(), n->value);
		binary_insert(open, n);
		return true;
	}
	return false;
}

/*
	make_path
	returns: double
	This function takes a given starting point and finds the fastest path to the given endpoint. An A* algorithm with euclidean heuristic is used.
	The function returns a double representing the pointer to the resultant Path object to pass back into gamemaker.
	NOTE: diag_type argument takes 0 for no diagonal, 1 for diagonals allowed, 2 for only non-corner-cutting diagonals
*/
fn_export double make_path(double addr, double _xstart, double _ystart, double _xgoal, double _ygoal, double _diag_type, double _max_len) {
	int xstart = (int)_xstart;
	int ystart = (int)_ystart;
	int xgoal = (int)_xgoal;
	int ygoal = (int)_ygoal;
	int diag_type = (int)_diag_type;
	int max_len = (int)_max_len;
	Grid* g = get_grid(addr);
	Node* goal = g->get_node(xgoal, ygoal);
	if (goal->value < 0) {
		return 0; //early exit in case of inaccessible goal node -- can save a LOT of time
	}
	vector<Node*> open;
	open.push_back(g->get_node(xstart, ystart));
	open[0]->set_h(nullptr, xgoal, ygoal, 0);
	vector<Node*> closed;
	bool success = false;
	int loop_count = 0;
	while (!open.empty()) {
		//set up lists to check first node
		Node* n = open[open.size() - 1];
		open.pop_back();
		closed.push_back(n);
		//check open node against goal
		if (n == goal) {
			success = true;
			break;
		}
		//goal was not achieved, now spread out to new nodes
		int cx = n->getx();
		int cy = n->gety();
		int gw = g->get_width();
		int gh = g->get_height();
		//right
		Node* checked_node = g->get_node(cx + 1, cy);
		insert_if_valid(true, checked_node, n, open, goal, max_len);
		//down
		checked_node = g->get_node(cx, cy + 1);
		insert_if_valid(true, checked_node, n, open, goal, max_len);
		//left
		checked_node = g->get_node(cx - 1, cy);
		insert_if_valid(true, checked_node, n, open, goal, max_len);
		//up
		checked_node = g->get_node(cx, cy - 1);
		insert_if_valid(true, checked_node, n, open, goal, max_len);
		//set up checks for diag
		if (diag_type > 0) {
			bool nw, ne, sw, se;
			nw = ne = sw = se = false;
			if (diag_type == 1) {
				nw = ne = sw = se = true;
			}
			else { //diag_type is 2 i.e. cut corners only if both cut tiles are valid as well
				Node* n1 = g->get_node(cx - 1, cy);
				Node* n2 = g->get_node(cx, cy - 1);
				if (n1 != nullptr && n2 != nullptr && n1->value >= 0 && n2->value >= 0) {
					nw = true;
				}
				n1 = g->get_node(cx + 1, cy);
				if (n1 != nullptr && n2 != nullptr && n1->value >= 0 && n2->value >= 0) {
					ne = true;
				}
				n2 = g->get_node(cx, cy + 1);
				if (n1 != nullptr && n2 != nullptr && n1->value >= 0 && n2->value >= 0) {
					se = true;
				}
				n1 = g->get_node(cx - 1, cy);
				if (n1 != nullptr && n2 != nullptr && n1->value >= 0 && n2->value >= 0) {
					sw = true;
				}
			}
			//up left
			checked_node = g->get_node(cx - 1, cy - 1);
			insert_if_valid(nw, checked_node, n, open, goal, max_len);
			//up right
			checked_node = g->get_node(cx + 1, cy - 1);
			insert_if_valid(ne, checked_node, n, open, goal, max_len);
			//down left
			checked_node = g->get_node(cx - 1, cy + 1);
			insert_if_valid(sw, checked_node, n, open, goal, max_len);
			//down right
			checked_node = g->get_node(cx + 1, cy + 1);
			insert_if_valid(se, checked_node, n, open, goal, max_len);
		}
		loop_count++;
	}
	//return
	if (success) {
		Path* p = new Path(goal);
		double path_addr = 1.0;
		memcpy(&path_addr, &p, sizeof(path_addr));
		//cleanup
		for (Node* n : closed) {
			n->f = 0;
			n->g = 0;
			n->prev = nullptr;
			n->checked = false;
			n->raw_length = 0;
		}
		for (Node* n : open) {
			n->f = 0;
			n->g = 0;
			n->prev = nullptr;
			n->checked = false;
			n->raw_length = 0;
		}
		return path_addr;
	}
	else {
		//cleanup
		for (Node* n : closed) {
			n->f = 0;
			n->g = 0;
			n->prev = nullptr;
			n->checked = false;
			n->raw_length = 0;
		}
		for (Node* n : open) {
			n->f = 0;
			n->g = 0;
			n->prev = nullptr;
			n->checked = false;
			n->raw_length = 0;
		}
		return 0;
	}
}