#include <cassert>;
#include <string>;

using namespace std;

const string ELEMENTS_SEPARATOR = ", ";

class ListItem {
private:
	ListItem* prev = nullptr;
	ListItem* next = nullptr;

public:
	int value;

	ListItem(int value) {
		this->value = value;
	}

	ListItem* getPrev() {
		return this->prev;
	}
	ListItem* getNext() {
		return this->next;
	}

	void setPrev(ListItem* newPrev) {
		this->prev = newPrev;
	}
	void setNext(ListItem* newNext) {
		this->next = newNext;
	}
};

class List {
private:
	ListItem* head = nullptr;
	ListItem* tail = nullptr;

public:
	// Returns the first element of a list
	ListItem* getFirst() {
		return this->head;
	}

	// Returns the last element of a list
	ListItem* getLast() {
		return this->tail;
	}

	// Adds element to the end of a list
	void push(ListItem* item) {
		if (this->tail == nullptr) {
			this->head = item;
			this->tail = item;
		}
		else {
			this->tail->setNext(item);
			item->setPrev(this->tail);
			this->tail = item;
		}
	}

	// Adds element to the start of a list 
	void unshift(ListItem* item) {
		if (this->head == nullptr) {
			this->head = item;
			this->tail = item;
		}
		else {
			item->setNext(this->head);
			this->head->setPrev(item);
			this->head = item;
		}
	}

	// Removes the last item of a list and returns it
	ListItem* pop() {
		if (this->tail == nullptr) {
			return nullptr;
		}

		ListItem* item = this->getLast();
		this->tail = item->getPrev();
		this->tail->setNext(nullptr);

		item->setPrev(nullptr);

		return item;
	}

	// Removes the first item of a list and returns it
	ListItem* shift() {
		if (this->head == nullptr) {
			return nullptr;
		}

		ListItem* item = this->getFirst();
		this->head = item->getNext();
		this->head->setPrev(nullptr);

		item->setNext(nullptr);

		return item;
	}

	// Returns element of a list at index
	ListItem* at(int index) {
		assert(index >= 0);

		int curIndex = 0;
		ListItem* curElement = this->head;

		while (curIndex < index && curElement != nullptr) {
			curIndex++;
			curElement = curElement->getNext();
		}

		return curElement;
	}

	// Inserts element before element of a list at index
	void insert(ListItem* item, int index) {
		ListItem* elementAtIndex = this->at(index);

		assert(elementAtIndex != nullptr);

		ListItem* prev = elementAtIndex->getPrev();

		prev->setNext(item);
		item->setPrev(prev);
		item->setNext(elementAtIndex);
		elementAtIndex->setPrev(item);
	}

	// Removes element of a list at index and returns it
	ListItem* remove(int index) {
		ListItem* elementAtIndex = this->at(index);

		assert(elementAtIndex != nullptr);

		elementAtIndex->getPrev()->setNext(elementAtIndex->getNext());
		elementAtIndex->getNext()->setPrev(elementAtIndex->getPrev());

		elementAtIndex->setPrev(nullptr);
		elementAtIndex->setNext(nullptr);

		return elementAtIndex;
	}

	// Returns a number of items in a list
	int length() {
		int index = 0;
		ListItem* curElement = this->head;

		while (curElement != nullptr) {
			curElement = curElement->getNext();
			index++;
		}

		return index;
	}

	// Removes all items of a list
	void clean() {
		while (this->head != nullptr) {
			ListItem* tempItem = this->head;

			this->head = tempItem->getNext();

			delete tempItem;
		}

		this->tail = nullptr;
	}

	// Replaces element of a list at index by item and returns replaced element
	ListItem* replace(ListItem* item, int index) {
		ListItem* replacedElement = this->remove(index);
		this->insert(item, index);

		return replacedElement;
	}

	bool isEmpty() {
		return this->getFirst() == nullptr;
	}

	// Inserts a list before element at index
	void insertList(List* list, int index) {
		ListItem* elementAtIndex = this->at(index);

		assert(elementAtIndex != nullptr);

		ListItem* next = elementAtIndex->getNext();
		elementAtIndex->setNext(list->getFirst());
		list->getFirst()->setPrev(elementAtIndex);

		if (elementAtIndex == this->getLast()) {
			this->tail = list->getLast();
		}
		else {
			next->setPrev(list->getLast());
			list->getLast()->setNext(next);
		}
	}

	// Converts a list to a string
	string toString() {
		string res = "";
		ListItem* cur = this->head;

		while (cur != nullptr) {
			res += to_string(cur->value);

			if (cur != this->getLast()) {
				res += ELEMENTS_SEPARATOR;
			}

			cur = cur->getNext();
		}

		return res;
	}
};