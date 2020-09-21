#include "Map.h"

Map::Map()
{
	head = new Node;        //creates a head pointer to the dummy node
	m_size = 0;             //even though there is a dummy node, we don't include it in size
	head->next = head;
	head->prev = head;
}

Map::Map(const Map& old)
{
	head = new Node;
	head->next = head;
	head->prev = head;

	for (int i = old.size() - 1; i >= 0; i--)
	{
		KeyType k;
		ValueType v;
		old.get(i, k, v);
		insert(k, v);
	}
	m_size = old.m_size; //need this at the end so insert() doesn't affect it
}

Map::~Map()
{
	Node* finder = head->next;
	Node* killMe;
	while (finder != head)
	{
		killMe = finder;
		finder = finder->next;
		delete killMe;
	}
	delete finder;   //deletes the dummy node (which doesn't have a next anymore)
}

Map& Map::operator=(const Map& rhs)
{
	if (this != &rhs)  //checks for aliasing in case both sides are the same
	{
		Map temp(rhs);  //uses the copy constructor to make a temp
		swap(temp);
	}                  //leaving the scope destructs the temp
	return *this;
}

bool Map::empty() const
{
	if (m_size == 0)
		return true;
	return false;
}

int Map::size() const
{
	return m_size;
}

bool Map::insert(const KeyType & key, const ValueType & value) //adds new node to the front after dummy var
{
	if (!contains(key)) {
		Node* n = new Node;
		n->value = value;
		n->key = key;          //creates new node and initializes non-pointer variables

		if (empty()) {             //if list contains only the dummy node
			n->next = head;        //first sets pointers of new node
			n->prev = head;

			head->next = n;        //then sets pointers of dummy node
			head->prev = n;
		}
		else                       //the list has at least one non-dummy node
		{
			n->prev = head;
			n->next = head->next;
			head->next = n;        //head->prev will still be the last item in the list
			n->next->prev = n;
		}

		m_size++;                  //will always increase size by one and return true
		return true;
	}
	else
		return false;
}

bool Map::update(const KeyType & key, const ValueType & value)
{
	if (!contains(key))   //this statement ensures there are at least 2 nodes (dummy and key)
		return false;

	Node* finder = head->next;
	while (finder != head)
	{
		if (finder->key == key)
		{
			finder->value = value;
			return true;
		}
		finder = finder->next;
	}
	return false;
}

bool Map::insertOrUpdate(const KeyType & key, const ValueType & value)
{
	insert(key, value) || update(key, value);
	return true;
}

bool Map::erase(const KeyType & key)
{
	if (contains(key))  //contains ensures there are at least 2 nodes
	{
		if (head->next->next == head)  //if there are exactly 2 nodes
		{
			Node* killMe = head->next;
			head->next = head;
			head->prev = head;
			delete killMe;
		}
		else                             //there are more than two nodes
		{
			Node* finder = head->next;   //sets finder to first non-dummy node
			while (finder != head)
			{
				if (finder->key == key)
					break;
				finder = finder->next;
			}
			finder->prev->next = finder->next;
			finder->next->prev = finder->prev;
			delete finder;
		}
		m_size--;
		return true;
	}
	else
		return false;
}
bool Map::contains(const KeyType & key) const
{
	Node* finder = head->next; //does not check the dummy node key since its undefined
	while (finder != head)
	{
		if (finder->key == key)
			return true;
		finder = finder->next;
	}
	return false;
}

bool Map::get(const KeyType & key, ValueType & value) const
{
	if (!contains(key))
		return false;

	Node* finder = head->next;
	while (finder != head)
	{
		if (finder->key == key)
			break;
		finder = finder->next;
	}

	value = finder->value;
	return true;
}

bool Map::get(int i, KeyType & key, ValueType & value) const
{
	if (i < 0 || i >= size())   //i is either too large or small to be a valid index
		return false;

	Node * finder = head->next;
	int counter = 0;
	while (finder != head)
	{
		if (counter == i)
			break;
		finder = finder->next;
		counter++;
	}

	key = finder->key;
	value = finder->value;
	return true;
}

void Map::swap(Map & other)
{
	Node* tempHead = head;
	int tempSize = m_size;

	head = other.head;
	m_size = other.m_size;

	other.head = tempHead;
	other.m_size = tempSize;
}

bool combine(const Map & m1, const Map & m2, Map & result)
{
	result = m1;  //will work if result is empty or has nodes
	bool returnval = true;

	for (int i = 0; i < m2.size(); i++)
	{
		KeyType k;
		ValueType v;
		m2.get(i, k, v);

		if (!m1.contains(k))
			result.insert(k, v);

		else       //checks if the m2 key is in m1
		{
			ValueType newV;
			m1.get(k, newV);
			if (v != newV)    //checks if the m1 and m2 values are different
				returnval = false;
		}
	}
	return returnval;
}

void reassign(const Map & m, Map & result) //make a test case where m and result are the same map
{
	result = m;

	if (m.empty())
		return;
	//if it gets here, there is at least 1 node
	KeyType firstKey;        //we need to get the first node's info before it changes
	ValueType firstVal;
	result.get(0, firstKey, firstVal);

	for (int i = 0; i < result.size() - 1; i++) //we only go to the second-to-last node
	{
		KeyType prevKey;
		ValueType prevVal;
		result.get(i, prevKey, prevVal);

		KeyType nextKey;
		ValueType nextVal;
		result.get(i + 1, nextKey, nextVal);

		result.update(prevKey, nextVal);
	}
	KeyType lastKey;
	ValueType lastVal;
	result.get(result.size() - 1, lastKey, lastVal);

	result.update(lastKey, firstVal);      //the last node's key gets the first node's value
}
