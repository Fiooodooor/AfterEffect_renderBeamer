#pragma once
#include <list>

template<typename T> 
class AeNodesConteiner : public std::list<T>
{
public:
	using aeLst = typename std::list<T>;
	using aeIter = typename aeLst::iterator;
	using aeConstIter = typename aeLst::const_iterator;
	
	AeNodesConteiner();

	bool pushUnique(T node);
	bool pushUniqueSorted(T node);

	bool checkIfUnique(T node);
	bool removeDuplicate(T node);
	
private:
	aeConstIter beg, end;

};

/* 
using aeLst = typename std::list<T>;
using aeIter = typename aeLst::iterator;
using aeConstIter = typename aeLst::const_iterator;
*/