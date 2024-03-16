#pragma once

namespace DLX
{
	struct Node;
	struct Candidate;
	class Grid;

	void Cover(Node*);
	void Uncover(Node*);

	enum SearchOption;
	int Search(Node*, Grid&, SearchOption);
}