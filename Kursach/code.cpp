#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

/**
 * List of all operators
 */
vector<char> operators = {'-', '+', '*', '/', '^'};

// ---- Lists of operators by priority -----
vector<char> levelOneOperators = { '+', '-' };

vector<char> levelTwoOperators = { '/', '*' };

vector<char> levelThreeOperators = { '^' };
// ----------------------------------------

/**
 * Enum of priority levels
 */
enum Level {
    Any,
    First,
    Second,
    Third,
};

/**
 * Enum of notations
 */
enum Notation {
    Prefix,
    Infix,
    Suffix
};

/**
 * Determines if item is in vector or not
 */
template <typename T>
bool isIn(vector<T> v, T item) {
    return count(v.begin(), v.end(), item) != 0;
}

/**
 * Returns list of operators for given list of priority levels
 */
vector<char> getOperatorsList(vector<Level> levels) {
    vector<char> _operators;

    for (Level l : levels) {
        if (l == Any) return operators;
        else if (l == First) _operators.insert(_operators.end(), levelOneOperators.begin(), levelOneOperators.end());
        else if (l == Second) _operators.insert(_operators.end(), levelTwoOperators.begin(), levelTwoOperators.end());
        else if (l == Third) _operators.insert(_operators.end(), levelThreeOperators.begin(), levelThreeOperators.end());
    }

    return _operators;
}

/**
 * Returns first expression that doesn't include levels' operators
 */
string getFirstExpression(string s, vector<Level> levels = {Any}) {
    auto levelOperators = getOperatorsList(levels);

    string buf;

    for (int i = 0; i < s.length(); i++) {
        char c = s[i];

        if (isIn(levelOperators, c) && (c != '-' || s[i + 1] == ' ')) {
            break;
        } else if (c == ' ' && (levelOperators == operators || isIn(levelOperators,s[i + 1]))) {
            break;
        }

        buf += c;
    }

    return buf;
}

/**
 * Returns last expression that doesn't include levels' operators
 */
string getLastInt(string s, vector<Level> l = {Any}) {
    auto levelOperators = getOperatorsList(l);

    string buf;

    for (int i = s.length() - 1; i >= 0; i--) {
        char c = s[i];

        if (isIn(levelOperators, c) && (c != '-' || s[i - 1] == ' ')) {
            break;
        } else if (c == ' ' && (levelOperators == operators || isIn(levelOperators,s[i - 1]))) {
            break;
        }

        buf = c + buf;
    }

    return buf;
}

/**
 * Removes first n characters of a string
 */
string removeFirst(string s, int n) {
    n = min(n, (int)s.length());

    return s.substr(n, s.length() - n);
}

class Node {
private:
    Node* left;
    Node* right;
    string value;

public:
    explicit Node(string value) {
        this->setValue(value);
        left = nullptr;
        right = nullptr;
    }
    Node() : value(nullptr), left(nullptr), right(nullptr) {}

    Node* setLeft(Node* ptr) {
        left = ptr;
        return ptr;
    }

    Node* setRight(Node* ptr) {
        right = ptr;
        return ptr;
    }

    Node* getLeft() {
        return left;
    }

    Node* getRight() {
        return right;
    }
    
    string getValue() {
        return this->value;
    };
    
    string setValue(string newValue) {
        return this->value = newValue;
    }

    /**
     * Computes result of a node
     */
    int computeValue() {
        if (this->value.length() == 1 && isIn(operators, this->value[0])) {
            switch (this->value[0]) {
                case '+': {
                    return this->getLeft()->computeValue() + this->getRight()->computeValue();
                }
                case '-': {
                    return this->getLeft()->computeValue() - this->getRight()->computeValue();
                }
                case '*': {
                    return this->getLeft()->computeValue() * this->getRight()->computeValue();
                }
                case '/': {
                    return this->getLeft()->computeValue() / this->getRight()->computeValue();
                }
                case '^': {
                    return pow(this->getLeft()->computeValue(), this->getRight()->computeValue());
                }
                default: {
                    throw exception();
                }
            }
        } else {
            return stoi(this->value);
        }
    }
 };

class Tree {
public:
    Node* head;
    Tree() :head(nullptr) {}

    /**
     * Builds a tree from a given expression
     */
    int fromExpression(string expression, Notation notation, Node* toNode = nullptr) {
        if (toNode == nullptr) {
            toNode = this->head = new Node("");
        }

        if (notation == Prefix) {
            if (isIn(operators, expression[0]) && expression[1] == ' ') {
                toNode->setValue(expression.substr(0, 1));

                auto left = toNode->setLeft(new Node(""));
                auto right = toNode->setRight(new Node(""));

                int computed = fromExpression(expression.substr(2, expression.length() - 2), notation, left);
                int computed2 = fromExpression(expression.substr(2 + computed, expression.length() - 2 - computed), notation, right);

                return computed + computed2 + 2;
            } else {
                string firstInt = getFirstExpression(expression);

                toNode->setValue(firstInt);

                return firstInt.length() + 1;
            }
        } else if (notation == Suffix) {
            if (isIn(operators, expression[expression.length() - 1]) && expression[expression.length() - 2] == ' ') {
                toNode->setValue(expression.substr(expression.length() - 1, 1));

                auto left = toNode->setLeft(new Node(""));
                auto right = toNode->setRight(new Node(""));

                int computed = fromExpression(expression.substr(0, expression.length() - 2), notation, right);
                int computed2 = fromExpression(expression.substr(0, expression.length() - 2 - computed), notation, left);

                return computed + computed2 + 2;
            } else {
                string lastInt = getLastInt(expression);

                toNode->setValue(lastInt);

                return lastInt.length() + 1;
            }
        } else {
            auto res = fromInfix(expression);

            toNode->setValue(res->getValue());
            toNode->setRight(res->getRight());
            toNode->setLeft(res->getLeft());
        }
    }

    /**
     * Utility function to process an infix expression
     * Firstly it process + and -
     * Then * and /
     * And then ^ operator
     */
    Node* fromInfix(string expression, Level l = First) {
        string firstInt = getFirstExpression(expression, {l});
        auto firstIntNode = new Node(firstInt);

        expression = removeFirst(expression, firstInt.length() + 1);

        if (expression.length() == 0) {
            return firstIntNode;
        }

        string op = expression.substr(0, 1);
        auto opNode = new Node(op);

        expression = removeFirst(expression, 2);

        opNode->setLeft(firstIntNode);
        auto rightNode = opNode->setRight(fromInfix(expression));

        /**
         * If node is an expression, e.g. not operator it processes it to check if there are higher level operators
         */
        auto processHigherPriorityOperators =  [this](Node *node, Level l) {
                if (!node->getRight() && !node->getLeft()) {
                    auto subTree = this->fromInfix(node->getValue(), l);

                    node->setValue(subTree->getValue());
                    node->setRight(subTree->getRight());
                    node->setLeft(subTree->getLeft());
                }
            };

        /**
         * Firstly checks * and /
         */
        processHigherPriorityOperators(firstIntNode, Second);
        processHigherPriorityOperators(rightNode, Second);

        /**
         * Then checks ^
         */
        processHigherPriorityOperators(firstIntNode, Third);
        processHigherPriorityOperators(rightNode, Third);

        return opNode;
    }

    /**
     * Alias for computing value of head node
     */
    int Compute() {
        return this->head->computeValue();
    }
};

int main() {
    string expression;

    loop: while (true) {
        cout << "Write P for prefix, I for Infix and S for Suffix (Postfix) notation: ";

        char n;
        cin >> n;

        if (!isIn({'S', 'P', 'I'}, n)) {
            cout << "Wrong notation, try again";
            goto loop;
        }

        map<char, Notation> notations = {{'S', Suffix}, {'I', Infix}, {'P', Prefix}};
        Notation notation = notations[n];

        cout << "Enter your expression: ";

        getline(cin, expression);
        getline(cin, expression);

        try {
            Tree expressionTree;

            expressionTree.fromExpression(expression, notation);

            auto result = expressionTree.Compute();

            cout << "Result of your expression is: " << result;

            break;
        } catch (...) {
            cout << "Invalid expression" << endl;
        }
    }

    return 0;
}