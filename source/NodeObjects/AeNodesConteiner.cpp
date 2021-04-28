#include "AeNodesConteiner.h"
namespace RenderBeamer {
template<typename T>
AeNodesConteiner<T>::AeNodesConteiner() 
	: std::list<T>()
{
}

template<typename T>
bool AeNodesConteiner<T>::pushUnique(T node)
{
	beg = this->cbegin();
	end = this->cend();

	while (beg != end) {
		if (node == (*beg))
			break;
		beg++;
	}
	if (beg == end) {
		this->push_back(node);
		return true;
	}
	return false;
}

template<typename T>
bool AeNodesConteiner<T>::pushUniqueSorted(T node)
{
	beg = this->cbegin();
	end = this->cend();

	while (beg != end) {
		if (node > (*beg))
			beg++;
		else
			break;
	}
	if (node == (*beg))
		return false;
	if (beg == end)
		this->push_back(node);
	else
		this->insert(beg, node);
	return true;
}

template<typename T>
bool AeNodesConteiner<T>::checkIfUnique(T node)
{
	beg = this->cbegin();
	end = this->cend();

	while (beg != end) {
		if (node == (*beg))
			return false;
		beg++;
	}
	return false;
}

template<typename T>
bool AeNodesConteiner<T>::removeDuplicate(T node)
{
	beg = this->cbegin();
	end = this->cend();

	while (beg != end) {
		if (node == (*beg)) {
			this->remove(beg);
			return true;
		}
		beg++;
	}
	return false;
}
} // namespace RenderBeamer
