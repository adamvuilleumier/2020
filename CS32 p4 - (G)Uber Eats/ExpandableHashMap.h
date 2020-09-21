// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <list>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Node;

	std::list<Node*>* m_list; //m_list points to one of the lists in a bucket in the hashmap
	int m_buckets;
	double m_maxLoad;
	int m_associations;

	unsigned int getPosition(KeyType key) const
	{
		unsigned int hasher(const KeyType & k);
		unsigned int position = hasher(key);

		position = position % m_buckets; //sets correct posiiton in array based on hasher() return val and size of array
		return position;
	}

	struct Node
	{
		KeyType key;
		ValueType value;

		Node(KeyType k, ValueType v)
		{
			key = k;
			value = v;
		}
	};
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_maxLoad = maximumLoadFactor;
	m_buckets = 8;
	m_associations = 0;

	m_list = new std::list<Node*>[m_buckets]; //m_list points to the first list in the hashmap
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	for (int i = 0; i < m_buckets; i++)
	{
		for (auto it = m_list[i].begin(); it != m_list[i].end(); it++)
		{
			delete* it;
		}
	}
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	delete[] m_list;
	m_buckets = 8;
	m_associations = 0;
	m_list = new std::list<Node*>[m_buckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_associations;
}

//if key is already in map, update it so it associates to new value
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	double loadFactor = (m_associations + 1) / m_buckets;
	if (loadFactor > m_maxLoad)  //if load factor is too high
	{
		int oldBuckets = m_buckets; //make new table with twice as many buckets
		m_buckets *= 2;
		std::list<Node*>* newTable = new std::list<Node*>[m_buckets];

		for (int i = 0; i < oldBuckets; i++) //search through old table and populate all nodes into new table
		{
			if (!m_list[i].empty())
			{
				for (auto it = m_list[i].begin(); it != m_list[i].end(); it++)
				{
					unsigned int position = getPosition((*it)->key);
					Node* nodeToTransfer = *it;
					newTable[position].push_back(nodeToTransfer);
				}
			}
		}
		m_list = newTable; //set m_list to point to the new expanded table
	}


	ValueType* foundVal = find(key); //updates value if key is already in map
	if (foundVal != nullptr)
	{
		*foundVal = value;
		return;
	}

	unsigned int position = getPosition(key); //chooses position based on hasher() to stick pairing

	Node* newNode = new Node(key, value); //creates a new node and adds it to correct list
	m_list[position].push_back(newNode); //make sure this actually adds a pointer

	m_associations++;
}

//return nullptr if the key is not in the hashmap
template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	int searchBucket = getPosition(key); //uses same hasher() to find right bucket to search in

	ValueType* valPtr = nullptr;

	for (auto it = m_list[searchBucket].begin();
		it != m_list[searchBucket].end(); it++)
	{
		if ((*it)->key == key)
			valPtr = &((*it)->value); //if key is found, valPtr set to value
	}

	return valPtr;
}
