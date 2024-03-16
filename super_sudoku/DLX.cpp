#include "DLX.h"
#include <algorithm>
#include <array>
#include <random>

using DLX::Node;
using DLX::Candidate;
using DLX::Grid;
using DLX::Cover;
using namespace std;

struct Node
{
	Candidate* candidate;
	Node* left, * right, * up, * down, * column;

	Node() {}
	Node(Candidate* candidate)
	{
		this->candidate = candidate;
	}
	Node(int i, int j, int value)
	{
		candidate = new Candidate(i, j, value);
	}
};

struct Candidate
{
	int i, j, value;
	Candidate(int i, int j, int value)
	{
		this->i = i;
		this->j = j;
		this->value = value;
	}
};

class ConstraintMatrix
{
private:
	Node* root;

	Node* At(int r, int c) // Indexes into constraint matrix.
	{
		Node* node = root;
		for (int j = 0; j < c + 1; j++)
		{
			node = node->right;
		}

		for (int i = 0; i < r + 1; i++)
		{
			node = node->down;
		}

		return node;
	}

	void DoubleLinkColumns(Node* root, Node*& prev, Node* (&cols)[81])
	{
		for (int i = 0; i < 81; i++)
		{
			cols[i] = new Node();

			// Sets up vertical links.
			cols[i]->up = cols[i]->down = cols[i]->column = cols[i];

			// Sets up right links.
			cols[i]->right = root;
			root->left = cols[i];

			// Sets up left links.
			cols[i]->left = prev;
			prev->right = cols[i];

			prev = cols[i];
		}
	}

	void DoubleLink(Candidate* cand, Node*& prev)
	{
		Node* node = new Node(cand);

		// Sets up up links.
		prev->down = node;
		node->up = prev;

		// Sets up down links.
		node->down = node->column = prev->column;
		node->column->up = node;

		prev = node;
	}

	void Shuffle(array<int, 9>& indexer)
	{
		random_device rd;
		mt19937 g(rd());
		shuffle(indexer.begin(), indexer.begin() + 9, g);
	}

public:
	ConstraintMatrix()
	{
		// Creates the headers and links them up.
		root = new Node();

		Node* header = root;
		Node* pos[81]{};
		DoubleLinkColumns(root, header, pos);
		Node* row[81]{};
		DoubleLinkColumns(root, header, row);
		Node* col[81]{};
		DoubleLinkColumns(root, header, col);
		Node* box[81]{};
		DoubleLinkColumns(root, header, box);

		// Each array of headers stores the last node that was added, makes vertical linking easier.
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				Node*& p = pos[i * 9 + j];
				array<int, 9> valueIndexer = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
				Shuffle(valueIndexer);
				for (int v = 0; v < 9; v++)
				{
					// Creates candidate for this position.
					int value = valueIndexer[v];
					Candidate* cand = new Candidate(i, j, value);

					// Creates position constraint.
					DoubleLink(cand, p);

					// Creates row constraint.
					Node*& r = row[i * 9 + value];
					DoubleLink(cand, r);
					p->right = r;
					r->left = p;

					// Creates column constraint.
					Node*& c = col[j * 9 + value];
					DoubleLink(cand, c);
					r->right = c;
					c->left = r;

					// Creates box constraint.
					Node*& b = box[((i / 3) * 3 + j / 3) * 9 + value];
					DoubleLink(cand, b);
					c->right = b;
					b->left = c;

					// Completes the circular list.
					b->right = p;
					p->left = b;
				}
			}
		}
	}

	//ConstraintMatrix(Grid* grid) : ConstraintMatrix()
	//{
	//	// Given the values on the grid, reduces the constraint matrix accordingly.
	//	this->grid = grid;
	//	for (int i = 0; i < 9; i++)
	//	{
	//		for (int j = 0; j < 9; j++)
	//		{
	//			if (!(*grid)(i, j))
	//			{
	//				continue;
	//			}
	//
	//			Node* node = At((*grid)(i, j) - 1, i * 9 + j);
	//			Cover(node->column);
	//			for (Node* x = node->right; x != node; x = x->right)
	//			{
	//				Cover(x->column);
	//			}
	//		}
	//	}
	//}

	void FindFirstSolution(Grid* grid)
	{

	}

	enum NumSolutions
	{
		None,
		One,
		Multiple
	};

	NumSolutions CheckSolutions()
	{
		// Need a version of search that returns as soon as multiple solutions are found.
		return None;
	}
};

class Grid
{
private:
	int values[9][9];
public:
	Grid() : values{}
	{
	}

	void Generate()
	{
		// Shuffle rows then get first solution as our complete grid.
		// Remove a random number and solve: if no solution or multiple, pick different number; else repeat.
	}

	void Draw()
	{

	}

	const int& operator()(int i, int j) const { return values[i][j]; }
	int& operator()(int i, int j) { return values[i][j]; }
};

void DLX::Cover(Node* column)
{
	// Removes column from header list.
	column->right->left = column->left;
	column->left->right = column->right;

	// Removes column's rows from other columns.
	for (Node* i = column->down; i != column; i = i->down)
	{
		for (Node* j = i->right; j != i; j = j->right)
		{
			j->down->up = j->up;
			j->up->down = j->down;
		}
	}
}

void DLX::Uncover(Node* column)
{
	// Adds column's rows to other columns in reverse order.
	for (Node* i = column->up; i != column; i = i->up)
	{
		for (Node* j = i->left; j != i; j = j->left)
		{
			j->down->up = j;
			j->up->down = j;
		}
	}

	// Adds column to header list.
	column->right->left = column;
	column->left->right = column;
}

enum DLX::SearchOption
{
	FindAll,
	FindFirst,
	FindUnique
};

#pragma warning( disable : 28182 )
int DLX::Search(Node* root, Grid& grid, SearchOption option = FindAll)
{
	// If no more columns present, a solution is found.
	if (root->right = root)
	{
		grid.Draw();
		return 1;
	}

	// Chooses column to add to solution.
	Node* col = root->right;
	Cover(col);

	int solnCount = 0;
	for (Node* i = col->down; i != col; i = i->down)
	{
		// Adds the row's candidate to the grid.
		Candidate* cand = i->candidate;
		grid(cand->i, cand->j) = cand->value + 1;

		for (Node* j = i->right; j != i; j = j->right)
		{
			Cover(j->column);
		}

		solnCount += Search(root, grid);

		for (Node* j = i->left; j != i; j = j->left)
		{
			Uncover(j->column);
		}

		if (solnCount > 0)
		{
			if (option == FindFirst)
			{
				break;
			}
			else if (option == FindUnique && solnCount > 1)
			{
				break;
			}
		}
	}

	Uncover(col);
	return solnCount;
}
#pragma warning( default : 28182 )
