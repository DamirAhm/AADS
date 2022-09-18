#include <iostream>
#include "List.cpp";

int main()
{
    List* listA = new List();
    List* listB = new List();

    // Fill lists
    for (int i = 1; i <= 10; i++) {
        listA->push(new ListItem(i));
        listB->push(new ListItem(i * 2));
    }

    cout << "First list: " << listA->toString() << endl;
    cout << "Second list: " << listB->toString() << endl;

    // Add element to the beginning of the list
    listA->unshift(new ListItem(0));

    cout << "Add element to the beginning of the first list: " << listA->toString() << endl;

    // Remove last element
    listB->pop();

    cout << "Remove last element of the second list: " << listB->toString() << endl;

    // Remove first element
    listB->shift();

    cout << "Remove first element of the second list: " << listB->toString() << endl;

    // Insert element
    listA->insert(new ListItem(100), 5);

    cout << "Insert 100 before 5th element of the firt list: " << listA->toString() << endl;

    // Get element at Nth index
    cout << "4th element of the second list: " << listB->at(4)->value << endl;

    // Remove element at Nth index
    listB->remove(4);
    cout << "Remove 4th element of the second list: " << listB->toString() << endl;

    // Length of a list
    cout << "Length of the second list now is: " << listB->length() << endl;

    // Replace element
    listA->replace(new ListItem(42), 2);

    cout << "Replace 2nd element of the first list by 42: " << listA->toString() << endl;

    // Insert list
    listA->insertList(listB, 2);

    cout << "Insert the second list after 2nd element of the first list: " << listA->toString() << endl;

    // Clean list
    listA->clean();

    cout << "Clean the first list: " << listA->toString() << endl;

    // Check emptyness
    cout << "Is the first list empty: " << listA->isEmpty() << endl;

    return 0;
}