#pragma once
#include <list>
namespace RenderBeamer {
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
} // namespace RenderBeamer
/* 
using aeLst = typename std::list<T>;
using aeIter = typename aeLst::iterator;
using aeConstIter = typename aeLst::const_iterator;
*/
