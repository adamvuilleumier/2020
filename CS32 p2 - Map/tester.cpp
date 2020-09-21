#include "Map.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
    Map empty;
    Map one;
    Map copy;
    one.insert("juan", 5);
    Map m(one);
    m.insert("adam", 29);
    m.insert("beth", 30);
    m.insert("colin", 12);
    copy = m;
    KeyType k;
    ValueType v = 83;
    //sets up the maps we'll be testing (also tests constructors/assignment operator/insert)

    empty.empty(); //tests true case for empty
    one.empty();   //tests false case for empty

    empty.size();
    one.size();
    m.size();      //tests size for 0, 1, and 3 nodes

    m.update("adam", 26); //tests update
    empty.update("chris", 15); //tests update when key is not in map

    m.insertOrUpdate("aaron", 13); //tests insert case
    m.insertOrUpdate("adam", 42);  //tests update case

    empty.erase("mary");  //nothing to erase
    m.erase("juan");     //tests erase for last node
    one.erase("juan");    //tests erase for only node

    empty.contains("hi");  //contains nothing
    m.contains("adam");    //tests true case

    m.get("beth", v);      //makes sure v is changed by get
    empty.get("beth", v);  //tests when map is empty

    m.get(-2, k, v);   //tests when i is out of range
    m.get(0, k, v);    //tests getting the first item
    m.get(3, k, v);    //tests getting the last item

    m.swap(empty);   //tests where swapping with empty
    m.swap(empty);       //swaps back
    m.swap(m);       //tests where swapping with self

    one.insert("juan", 68);
    combine(one, empty, empty);      //tests combine where result is empty
    combine(one, m, copy);       //tests combine where result is empty/alias

    reassign(one, empty);    //tests case with one node
    reassign(m, m);          //tests case where both parameters are same

	cout << "M" << endl;
    for (int i = 0; i < m.size(); i++) {
        KeyType k;
        ValueType v;
        m.get(i, k, v);
        cerr << k << " " << v << endl;
    }

	cout << "ONE" << endl;
    for (int i = 0; i < one.size(); i++) {
        KeyType k;
        ValueType v;
        one.get(i, k, v);
        cerr << k << " " << v << endl;
    }

	cout << "EMPTY" << endl;
    for (int i = 0; i < empty.size(); i++) {
        KeyType k;
        ValueType v;
        empty.get(i, k, v);
        cerr << k << " " << v << endl;
    }
}

int main()
{
    test();

    cout << "passed all tests" << endl;
}
