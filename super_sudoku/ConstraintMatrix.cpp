#include "ConstraintMatrix.h"
#include "Grid.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <random>

using namespace std;

namespace Sudoku
{
	struct Candidate
	{
		int i, j, value;
		Candidate() : i(), j(), value() {}
		Candidate(int i, int j, int value) : i(i), j(j), value(value) {}
	};

	struct Node
	{
		const Candidate* const candidate;
		Node* left, * right, * up, * down, * column;

		Node() : candidate(nullptr)
		{
			left = right = up = down = column = nullptr;
		}

		Node(const Candidate* const candidate) : candidate(candidate)
		{
			left = right = up = down = column = nullptr;
		}

		void RemoveFromRow()
		{
			right->left = left;
			left->right = right;
		}

		void RemoveFromCol()
		{
			down->up = up;
			up->down = down;
		}

		void RestoreToRow()
		{
			right->left = this;
			left->right = this;
		}

		void RestoreToCol()
		{
			down->up = this;
			up->down = this;
		}

		void Cover()
		{
			// Removes column from header list.
			column->RemoveFromRow();

			// Removes column's rows from other columns.
			for (Node* i = column->down; i != column; i = i->down)
			{
				for (Node* j = i->right; j != i; j = j->right)
				{
					j->RemoveFromCol();
				}
			}
		}

		void Uncover()
		{
			// Adds column's rows to other columns in reverse order.
			for (Node* i = column->up; i != column; i = i->up)
			{
				for (Node* j = i->left; j != i; j = j->left)
				{
					j->RestoreToCol();
				}
			}

			// Adds column to header list.
			column->RestoreToRow();
		}

	};

	Node* ConstraintMatrix::At(int r, int c)
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

	void ConstraintMatrix::DoubleLinkColumns(Node*& prev, Node** cols)
	{
		for (int i = 0; i < 81; i++)
		{
			cols[i] = new Node();

			// Sets vertical links.
			cols[i]->up = cols[i]->down = cols[i]->column = cols[i];

			// Sets left links.
			cols[i]->left = prev;
			prev->right = cols[i];

			// Sets right links.
			cols[i]->right = root;
			root->left = cols[i];

			prev = cols[i];
		}
	}

	void ConstraintMatrix::DoubleLink(const Candidate* const cand, Node*& prevRow, Node*& prevCol)
	{
		Node* node = new Node(cand);

		// Sets up links.
		node->up = prevRow;
		prevRow->down = node;

		// Sets down links.
		node->down = node->column = prevRow->column;
		node->column->up = node;

		if (prevCol)
		{
			// Sets left links.
			node->left = prevCol;
			prevCol->right = node;
		}

		prevRow = node;
		prevCol = node;
	}

	void ConstraintMatrix::Shuffle(std::array<int, 9>& indexer)
	{
		random_device rd;
		mt19937 g(rd());
		shuffle(indexer.begin(), indexer.begin() + 9, g);
	}

	ConstraintMatrix::ConstraintMatrix()
	{
		// Creates the headers and links them up.
		root = new Node();

		Node** pos = new Node * [81] {};
		Node** row = new Node * [81] {};
		Node** col = new Node * [81] {};
		Node** box = new Node * [81] {};

		Node* header = root;
		DoubleLinkColumns(header, pos);
		DoubleLinkColumns(header, row);
		DoubleLinkColumns(header, col);
		DoubleLinkColumns(header, box);

		// Each array of headers stores the last node that was added, makes vertical linking easier.
		candidates = new Candidate[9 * 81];
		array<int, 9> valueIndexer = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				Node*& p = pos[i * 9 + j];
				Shuffle(valueIndexer);
				for (int v = 0; v < 9; v++)
				{
					// Creates candidate for this position.
					int value = valueIndexer[v];
					int index = (i * 9 + j) * 9 + value;
					candidates[index] = Candidate(i, j, value + 1);

					// Creates position constraint.
					Node* c1 = nullptr;
					DoubleLink(&candidates[index], p, c1);

					// Creates row constraint.
					Node*& r = row[i * 9 + value];
					DoubleLink(&candidates[index], r, c1);

					// Creates column constraint.
					Node*& c = col[j * 9 + value];
					DoubleLink(&candidates[index], c, c1);

					// Creates box constraint.
					Node*& b = box[((i / 3) * 3 + j / 3) * 9 + value];
					DoubleLink(&candidates[index], b, c1);

					// Completes the circular list.
					b->right = p;
					p->left = b;
				}
			}
		}

		delete[] pos;
		delete[] row;
		delete[] col;
		delete[] box;
	}

	ConstraintMatrix::~ConstraintMatrix()
	{
		Node* col = root->left;
		while (col != root)
		{
			Node* node = col->up;
			while (node != col)
			{
				// Deletes nodes.
				Node* deleted = node;
				node = deleted->up;
				delete deleted;
			}

			// Deletes columns.
			Node* deleted = col;
			col = deleted->left;
			delete deleted;
		}

		// Deletes root and candidates.
		delete root;
		delete[] candidates;
	}

	void ConstraintMatrix::Print()
	{
		/*for (int i = 0; i < 9 * 81; i++)
		{
			Candidate cand = candidates[i];
			cout << "(" << cand.i << "," << cand.j << ") = " << cand.value << endl;
		}*/
	}

#pragma warning( disable : 28182 )
	int ConstraintMatrix::Search(Grid* grid, SearchOption option, int& iter)
	{
		// If no more columns present, a solution is found.
		if (root->right == root)
		{
			if (option != FindFirst)
			{
				grid->Draw();
			}
			else
			{
				cout << "Total iterations: " << iter << endl;
			}

			return 1;
		}

		iter++;

		// Chooses column to add to solution.
		Node* col = root->right;
		col->Cover();

		int solnCount = 0;
		for (Node* i = col->down; i != col; i = i->down)
		{
			// Adds the row's candidate to the grid.
			grid->Set(i->candidate->i, i->candidate->j, i->candidate->value);

			for (Node* j = i->right; j != i; j = j->right)
			{
				j->Cover();
			}

			solnCount += Search(grid, option, iter);

			for (Node* j = i->left; j != i; j = j->left)
			{
				j->Uncover();
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
			else
			{
				iter++;
			}
		}

		col->Uncover();
		return solnCount;
	}
#pragma warning( default : 28182 )

	void ConstraintMatrix::FindFirstSolution(Grid* grid)
	{

	}

	ConstraintMatrix::NumSolutions ConstraintMatrix::CheckSolutions()
	{
		// Need a version of search that returns as soon as multiple solutions are found.
		return None;
	}
}