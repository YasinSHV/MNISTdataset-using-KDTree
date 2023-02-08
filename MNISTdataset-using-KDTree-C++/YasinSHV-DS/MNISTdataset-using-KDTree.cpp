//by Yasin Shabani Varaki
#include <vector>
#include <iostream>
using namespace std;

class Point
{
private:
	const vector<float> dimensions;
	Point* left = nullptr;
	Point* right = nullptr;

public:
	Point(vector<float> _dimensions) : dimensions(_dimensions) {}

	vector<float> getDimensions() const { return dimensions; }
	float getDimension(int i) const { return dimensions[i]; }

	Point getLeft() { return *left; }
	void setLeft(Point* _left) { left = _left; }

	Point getRight() { return *right; }
	void setRight(Point* _right) { right = _right; }

	bool isEqual(Point* check) 
	{
		for(int i = 0; i < dimensions.size(); i++)
			if (check->getDimension(i) != dimensions[i])
				return false;
		
		return true;
	}
};

class KDNode 
{
public:
	Point* point;
	KDNode* parent = nullptr;
	KDNode* right = nullptr;
	KDNode* left = nullptr;

	KDNode(vector<float> axis) { point = new Point(axis); }

	KDNode(KDNode* _parent, vector<float> axis) 
	{
		parent = _parent;
		point = new Point(axis);
	}

	int getLevel() const
	{
		int level = 0;
		KDNode* node = parent;
		while (node)
		{
			level++;
			node = node->parent;
		}

		return level;
	}

	bool isEqual(const KDNode& kdNode) const {
		return point->isEqual(kdNode.point) && parent == kdNode.parent &&
			right == kdNode.right && left == kdNode.left;
	}
};

class KDTree 
{
private:
	const int k = 0;
	KDNode* root = nullptr;
	int count = 0;
	vector<Point>* points = new vector<Point>;

	KDNode* min = nullptr;
	KDNode* max = nullptr;

	//helper functions
	void getMax(KDNode* node, int idx) 
	{
		if(!node) return;
		if (node->point->getDimension(idx) >= node->point->getDimension(idx))
			max = node;
		getMax(node->left, idx);
		getMax(node->right, idx);
	}

	void getMin(KDNode* node, int idx) 
	{
		if (!node) return;
		if (node->point->getDimension(idx) <= node->point->getDimension(idx))
			min = node;
		getMin(node->left, idx);
		getMin(node->right, idx);
	}

	void deleteHelper(KDNode* node) 
	{
		KDNode* q = nullptr;
		if (!node->left) 
		{
			getMax(node->left, node->getLevel());
			q = max;
		}
		else if (!node->right) 
		{
			getMin(node->right, node->getLevel());
			q = min;
		}
		if (!q) 
		{
			if (node->parent->right) 
			{
				if (node->parent->right && node->parent->right->isEqual(*node))
					node->parent->right = nullptr;
			}
			if (node->parent->left) 
			{
				if (node->parent->left && node->parent->left->isEqual(*node))
				{
					node->parent->left = nullptr;
				}
			}
		}
		else 
		{
			node->point = q->point;
			deleteHelper(q);
		}
	}

	vector<Point> searchRangeHelper(KDNode* node, vector<float> lower_Bounds, vector<float> upper_Bounds)
	{
		if (!node) return {};
		int c = 0;

		for (int i = 0; i < k; i++)
			if (node->point->getDimension(i) >= lower_Bounds[i]
				&& node->point->getDimension(i) <= upper_Bounds[i])
				c++;
			else
				break;

		if (c == k)
			points->push_back(*node->point);

		if (!(node->point->getDimension(node->getLevel() % k) < lower_Bounds[node->getLevel() % k]))
			searchRangeHelper(node->left, lower_Bounds, upper_Bounds);

		if (!(node->point->getDimension(node->getLevel() % k) > upper_Bounds[node->getLevel() % k]))
			searchRangeHelper(node->right, lower_Bounds, upper_Bounds);

		return *points;
	}

public:
	KDTree(int _k) : k(_k) {}

	KDTree *createTree(vector<Point> _points) 
	{
		KDTree* tree = new KDTree(k);
		for (Point point : _points) {
			tree->insert(point.getDimensions());
		}
		return tree;
	}

	void insert(vector<float> axis) 
	{
		if (!root) {
			root = new KDNode(axis);
			return;
		}

		KDNode* p = root;
		int level = 0;
		while (true) 
		{
			if (pointExists(axis))
				return;
			if (axis[level % k] > p->point->getDimension(level % k)) 
			{
				if (!p->right) {
					p->right = new KDNode(p, axis);
					count++;
					return;
				}
				p = p->right;
					level++;
			}
			else if (axis[level % k] <= p->point->getDimension(level % k)) 
			{
				if (!p->left) 
				{
					p->left = new KDNode(p, axis);
					count++;
					return;
				}
				p = p->left;
				level++;
			}
		}
	}

	vector<Point> searchRange(vector<float> lower_bounds, vector<float> upper_bounds)
	{
		vector<Point> points;
		points = searchRangeHelper(root, lower_bounds, upper_bounds);
		return points;
	}

	bool pointExists(vector<float> axis)
	{
		KDNode* p = root;
		int level = 0;
		while (p)
		{
			bool equal = true;
			for (int i = 0; i < k; i++)
			{
				if (axis[i] != p->point->getDimension(i))
				{
					equal = false;
					break;
				}
			}
			if (equal)
				return true;

			if (isGreater(axis, level, p))
			{
				if (!p->right)
					return false;
				p = p->right;
				level++;
			}
			else if (isLess(axis, level, p))
			{
				if (!p->left)
					return false;
				p = p->left;
				level++;
			}
		}
		return false;
	}

	bool deletePoint(vector<float> axis) 
	{
		KDNode *p = root;
		int level = 0;
		while (p) 
		{
			if (isEqual(axis, level, p)) 
			{
				int c = 0, i = 0;
				while (i < k)
				{
					if (axis[i] == p->point->getDimension(i)) 
					{
						c++;
					}
					else 
					{
						c = -1;
						break;
					}
					i++;
				}
				if (c == k) 
				{
					deleteHelper(p);
					count--;
					return true;
				}
			}
			if (isGreater(axis, level, p)) 
			{
				if (!p->right)
					return false;
				p = p->right;
				level++;
			}
			else if (isLess(axis, level, p)) 
			{
				if (!p->left)
					return false;
				p = p->left;
				level++;
			}
		}
	}

	bool isEqual(vector<float> axis, int level, KDNode* p)
	{
		return axis[level % k] == p->point->getDimension(level % k);
	}
	bool isGreater(vector<float> axis, int level, KDNode* p)
	{
		return axis[level % k] > p->point->getDimension(level % k);
	}
	bool isLess(vector<float> axis, int level, KDNode* p)
	{
		return axis[level % k] <= p->point->getDimension(level % k);
	}
};

int main() 
{
	//vector<float> babaBoy = { {1,2,3} };
	//vector<float> babaBoy2 = { {3,4,5} };
	//vector<float> babaBoy3 = { {3,5,5} };
	//vector<Point> p = { Point(babaBoy), Point(babaBoy2) };
	//KDTree tree = KDTree(3);
	//KDTree *newT = tree.createTree(p);
	//cout << newT->pointExists(babaBoy2) << endl;
	//newT->deletePoint(babaBoy2);
	//cout << newT->pointExists(babaBoy2);
	
	return 0;
}
