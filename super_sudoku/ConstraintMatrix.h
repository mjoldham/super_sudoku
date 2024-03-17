#pragma once

#include <array>
#include "Grid.h"

namespace Sudoku
{
	struct Node;
	struct Candidate;

	class ConstraintMatrix
	{
	private:
		Node* root;
		Candidate* candidates;

		// Indexes into constraint matrix.
		Node* At(int, int);
		void DoubleLinkColumns(Node*&, Node**);
		void DoubleLink(const Candidate* const, Node*&, Node*&);
		void Shuffle(std::array<int, 9>&);

	public:
		ConstraintMatrix();
		~ConstraintMatrix();

		void Print();

		enum SearchOption
		{
			FindAll,
			FindFirst,
			FindUnique
		};

		int Search(Grid*, SearchOption, int&);
		void FindFirstSolution(Grid*);

		enum NumSolutions
		{
			None,
			One,
			Multiple
		};

		NumSolutions CheckSolutions();
	};
}