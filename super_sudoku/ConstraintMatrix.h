#pragma once

#include "Grid.h"
#include <array>

namespace Sudoku
{
	struct Node;
	struct Candidate;

	class ConstraintMatrix
	{
		// There are 9 candidates for each of the 81 cells, represented by the rows.
		// // (e.g. a 5 at (2,3) is represented by its own row.)

		// There are 4 constraints represented by columns that each row satisfies:
		// // 1) One number for each of the 81 cells.
		// // 2) 9 unique numbers in each of the 9 rows.
		// // 3) 9 unique numbers in each of the 9 columns.
		// // 4) 9 unique numbers in each of the 9 boxes.

		// (e.g. 5 at (2,3) satisfies having 1) a number in cell (2,3), 2) a 5 in the 2nd row,
		// // 3) a 5 in the 3rd column, and 4) a 5 in the 1st box.)

	private:
		Node* root;
		Candidate* candidates;

		// Indexes into constraint matrix.
		Node* At(int, int);
		void DoubleLinkColumns(Node*&, Node**);
		void DoubleLink(const Candidate* const, Node*&, Node*&);
		int Search(Grid* grid = nullptr);
		std::array<Node*, 81>::reverse_iterator Cover(const Grid*, std::array<Node*, 81>&);
		void Uncover(const Grid*, std::array<Node*, 81>::reverse_iterator, std::array<Node*, 81>::reverse_iterator);

	public:
		ConstraintMatrix();
		~ConstraintMatrix();

		void Generate(Grid*);
		bool Solve(const Grid*);
	};
}