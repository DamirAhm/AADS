#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

vector<char> operators = {'-', '+', '*', '/', '^'};

vector<char> levelOneOperators = { '+', '-' };

vector<char> levelTwoOperators = { '/', '*' };

vector<char> levelThreeOperators = { '^' };

enum Level {
    Any,
    First,
    Second,
    Third,
};

enum Notation {
    Prefix,
    Infix,
    Suffix
};

bool isIn(vector<char> v, char item) {
    return count(v.begin(), v.end(), item) != 0;
}

vector<char> getOperatorsList(vector<Level> levels) {
    vector<char> operators;

    for (Level l : levels) {
        if (l == Any) return operators;
        else if (l == First) operators.insert(operators.end(), levelOneOperators.begin(), levelOneOperators.end());
        else if (l == Second) operators.insert(operators.end(), levelTwoOperators.begin(), levelTwoOperators.end());
        else if (l == Third) operators.insert(operators.end(), levelThreeOperators.begin(), levelThreeOperators.end());
    }

    return operators;
}

string getFirstInt(string s, vector<Level> l = { Any }) {
    auto levelOperators = getOperatorsList(l);

    string buf;

    for (int i = 0; i < s.length(); i++) {
        char c = s[i];

        if (isIn(levelOperators, c)) {
            if (c != '-' || s[i + 1] == ' ') break;
            buf += c;
        } else if (c == ' ' && isIn(levelOperators,s[i + 1])) {
            break;
        }

        buf += c;
    }

    return buf;
}

string getLastInt(string s) {
    string buf;

    for (int i = s.length() - 1; i >= 0; i--) {
        char c = s[i];
        if (isIn(operators, c) || c == ' ') break;

        buf += c;
    }

    return buf;
}

string removeFirst(string s, int n) {
    n = min(n, (int)s.length());

    return s.substr(n, s.length() - n);
}

template <class T>
class Node {
private:
    Node* left;
    Node* right;
    Node* parent;
    T value;

public:
    explicit Node(T value) {
        this->SetValue(value);
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }
    Node() : value(0), left(nullptr), right(nullptr), parent(nullptr) {}

    Node* SetLeft(Node* ptr) {
        left = ptr;
        return ptr;
    }

    Node* SetRight(Node* ptr) {
        right = ptr;
        return ptr;
    }

    Node* SetParent(Node* ptr){
        parent = ptr;
        return ptr;
    }

    Node* GetLeft() {
        return left;
    }

    Node* GetRight() {
        return right;
    }

    Node* GetParent() {
        return parent;
    }

    T GetValue() {
        return this->value;
    };

    T SetValue(T newValue) {
        return this->value = newValue;
    }

    void SetLowParent(Node* node, Node* lowerNode) {
        lowerNode->SetParent(node);
    }
};

template <class T>
class Tree {
private:

    void preorder(Node<T>* n) {
        if (n != nullptr) {
            cout << n->GetValue() << " ";
            preorder(n->GetLeft());
            preorder(n->GetRight());
        }
    }

    void inorder(Node<T>* n) {
        if (n != nullptr) {
            inorder(n->GetLeft());

            cout << n->GetValue() << " ";

            inorder(n->GetRight());
        }
    }

    void postorder(Node<T>* n) {
        if (n != nullptr) {
            postorder(n->GetRight());
            cout << n->GetValue() << " ";
            postorder(n->GetLeft());
        }
    }

    void printLevel(Node<T>* n, int level) {
        if (n == nullptr)
            return;
        else if (level == 1)
            cout << n->GetValue() << " ";
        else if (level > 1) {
            printLevel(n->GetLeft(), level - 1);
            printLevel(n->GetRight(), level - 1);
        }
    }

    void getLevelCount(Node<T>* n, int count, int& max_count) const {
        if (n == nullptr)
            max_count = (max_count > count) ? max_count : count;
        else {
            count++;
            getLevelCount(n->GetLeft(), count, max_count);
            getLevelCount(n->GetRight(), count, max_count);
        }
    }

    int levels() const {
        int ans = INT_MIN;
        getLevelCount(head, 0, ans);
        return ans;
    }

public:
    Node<T>* head;
    Tree() :head(nullptr) {}
    Tree(int value) {
        Node<T>* node = new Node<T>(value);
        head = node;
    }
    Tree(Node<T>* node) {
        head = node;
    }

    Node<T>* GetHead() {
        return this->head;
    }

    bool Add(Node<T>* node, int value) {
        if (head == nullptr) {
            Node<T>* node = new Node<T>(value);
            head = node;
            return true;
        }
        if (value >= node->GetValue()) {
            if (node->GetRight() != nullptr)
                return Add(node->GetRight(), value);
            else {
                Node<T>* newNode = new Node<T>(value);
                node->SetRight(newNode);
                node->SetLowParent(node, newNode);
                return true;
            }
        }
        else {
            if (node->GetLeft() != nullptr)
                Add(node->GetLeft(), value);
            else {
                Node<T>* newNode = new Node<T>(value);
                node->SetLeft(newNode);
                node->SetLowParent(node, newNode);
                return true;
            }
        }
    }

    bool Add(int value) {
        if (this->search(value))return false;

        if (head == nullptr) {
            Node<T>* node = new Node<T>(value);
            head = node;
            return true;
        }
        if (value >= head->GetValue()) {
            if (head->GetRight() != nullptr)
                return Add(head->GetRight(), value);
            else {
                Node<T>* newNode = new Node<T>(value);
                head->SetRight(newNode);
                head->SetLowParent(head, newNode);
                return true;
            }
        }
        else {
            if (head->GetLeft() != nullptr)
                return Add(head->GetLeft(), value);
            else {
                Node<T>* newNode = new Node<T>(value);
                head->SetLeft(newNode);
                head->SetLowParent(head, newNode);
                return true;
            }
        }
    }

    Node<T>* search(int value, Node<T>* node) {
        while (node != nullptr) {
            if (value == node->GetValue()) {
                return node;
            }
            else {
                if (value <= node->GetValue()) return search(value, node->GetLeft());
                else return search(value, node->GetRight());
            }
        }
        return nullptr;
    }

    Node<T>* search(int value) {
        while (head != nullptr) {
            if (value == head->GetValue()) {
                return head;
            }
            else {
                if (value <= head->GetValue()) return search(value, head->GetLeft());
                else return search(value, head->GetRight());
            }
        }
        return nullptr;
    }

    Node<T>* RemoveNode(T x)
    {
        auto nodeToRemove = this->search(x);

        if (nodeToRemove->GetRight() && nodeToRemove->GetLeft()) {
            auto nodeToMove = nodeToRemove->GetLeft();

            auto temp = nodeToRemove->GetRight();

            if (this->head == nodeToRemove) {
                this->head = temp;
            }

            while (temp->GetLeft() != nullptr) temp = temp->GetLeft();

            temp->SetLeft(nodeToMove);
        } else if (nodeToRemove->GetLeft()) {
            nodeToRemove->SetValue( nodeToRemove->GetLeft()->GetValue());
            nodeToRemove->SetRight(nodeToRemove->GetLeft()->GetRight());
            nodeToRemove->SetLeft(nodeToRemove->GetLeft()->GetLeft());
        } else if (nodeToRemove->GetRight()) {
            nodeToRemove->SetValue( nodeToRemove->GetRight()->GetValue());
            nodeToRemove->SetLeft(nodeToRemove->GetRight()->GetLeft());
            nodeToRemove->SetRight(nodeToRemove->GetRight()->GetRight());
        } else {
            delete nodeToRemove;
        }

        return nodeToRemove;
    }

    int GetMin(Node<T>* root = nullptr) {
        if (root == nullptr) root = this->head;

        Node<T>* current = root;

        while (current->GetLeft() != nullptr) current = current->GetLeft();

        return current->GetValue();
    }

    int GetMax(Node<T>* root = nullptr) {
        if (root == nullptr) root = this->head;

        Node<T>* current = root;

        while (current->GetRight() != nullptr) current = current->GetRight();

        return current->GetValue();
    }

    int GetNext(int n){
        Node<T>* node = search(n);
        if (!node)return 0;

        if (node->GetRight() != nullptr) return GetMin(node->GetRight());
        node = node->GetParent();
        while (node != head && node->GetValue() <= n) { node = node->GetParent(); }
        if (node->GetValue() > n) return node->GetValue();
        else return NULL;
    }

    int GetPrev(int n) {
        Node<T>* node = search(n);
        if (!node)return 0;

        if (node->GetLeft() != nullptr) return GetMax(node->GetLeft());
        node = node->GetParent();
        while (node != head && node->GetValue() >= n) { node = node->GetParent(); }
        if (node->GetValue() < n) return node->GetValue();
        else return NULL;
    }

    void inorderPrint() {
        inorder(head);
        cout << "\n";
    }

    void preorderPrint() {
        preorder(head);
        cout << "\n";
    }

    void postorderPrint() {
        postorder(head);
        cout << "\n";
    }

    int fromExpression(string expression, Notation notation, Node<T>* toNode = nullptr) {
        if (toNode == nullptr) {
            toNode = this->head = new Node<T>("");
        }

        if (notation == Prefix) {
            if (isIn(operators, expression[0])) {
                toNode->SetValue(expression.substr(0, 1));

                auto left = toNode->SetLeft(new Node<string>(""));
                auto right = toNode->SetRight(new Node<string>(""));

                int computed = fromExpression(expression.substr(2, expression.length() - 2), notation, left);
                int computed2 = fromExpression(expression.substr(2 + computed, expression.length() - 2 - computed), notation, right);

                return computed + computed2 + 2;
            } else {
                string firstInt = getFirstInt(expression);

                toNode->SetValue( firstInt );

                return firstInt.length() + 1;
            }
        } else if (notation == Suffix) {
            if (isIn(operators, *(end(expression) - 1))) {
                toNode->SetValue( expression.substr(expression.length() - 2, 1));

                auto left = toNode->SetLeft(new Node<string>(""));
                auto right = toNode->SetRight(new Node<string>(""));

                int computed = fromExpression(expression.substr(0, expression.length() - 2), notation, left);
                int computed2 = fromExpression(expression.substr(0, expression.length() - 2 - computed), notation, right);

                return computed + computed2 + 2;
            } else {
                string lastInt = getLastInt(expression);

                toNode->SetValue( lastInt );

                return lastInt.length() + 1;
            }
        } else {
            auto res = fromInfix(expression);

            toNode->SetValue(res->GetValue());
            toNode->SetRight(res->GetRight());
            toNode->SetLeft(res->GetLeft());
        }
    }

    Node<T>* fromInfix(string expression, Level l = First) {
        string firstInt = getFirstInt(expression, { l });
        auto firstIntNode = new Node(firstInt);

        expression = removeFirst(expression, firstInt.length() + 1);

        if (expression.length() == 0) {
            return firstIntNode;
        }

        string op = expression.substr(0, 1);
        auto opNode = new Node(op);

        expression = removeFirst(expression, 2);

        opNode->SetLeft(firstIntNode);
        opNode->SetRight(fromInfix(expression));

        auto getBfsCallback = [this](Level l) {
            return [l, this](Node<string> *node) {
                if (!node->GetRight() && !node->GetLeft()) {
                    auto subTree = this->fromInfix(node->GetValue(), l);

                    node->SetValue(subTree->GetValue());
                    node->SetRight(subTree->GetRight());
                    node->SetLeft(subTree->GetLeft());
                }
            };
        };

        this->bfs(getBfsCallback(Second), opNode);

        this->bfs(getBfsCallback(Third), opNode);

        return opNode;
    }

    string toExpression(Notation notation, Node<T>* node = nullptr) {
        if (node == nullptr) {
            node = this->head;
        }

        string res;

        if (!isIn(operators, node->GetValue()[0])) {
            cout << node->GetValue() << " ";
            res += node->GetValue() + " ";
            return res;
        } else {
            if (notation == Prefix) {
                cout << node->GetValue() << " ";
                res += node->GetValue() + " ";
                toExpression(notation, node->GetLeft());
                toExpression(notation, node->GetRight());
            } else if (notation == Infix) {
                toExpression(notation, node->GetLeft());
                cout << node->GetValue() << " ";
                res += node->GetValue() + " ";
                toExpression(notation, node->GetRight());
            } else {
                toExpression(notation, node->GetLeft());
                toExpression(notation, node->GetRight());
                cout << node->GetValue() << " ";
                res += node->GetValue() + " ";
            }
        }

        return res;
    }

    int getMaxElemWidth() {
        int maxWidth = 0;

        for (int h = this->levels(); h > 0; --h) {
            auto lev = this->getLevel(h);
            for (const auto &item: lev)
                if (item.size() > maxWidth)
                    maxWidth = item.size();

        }

        return maxWidth;
    }

    void getLevelRec(Node<string> *n, vector<string> &res, int level) const {
        if (n == nullptr && level == 1) {
            res.push_back("");
            return;
        } else if (n == nullptr) {
            for (int i = 0; i < level; ++i) {
                res.push_back("");
            }
            return;
        } else if (level == 1)
            res.push_back(n->GetValue());
        else if (level > 1) {
            getLevelRec(n->GetLeft(), res, level - 1);
            getLevelRec(n->GetRight(), res, level - 1);
        }
    }

    void getLevelRec(Node<int> *n, vector<string> &res, int level) const {
        if (n == nullptr && level == 1) {
            res.push_back("");
            return;
        } else if (n == nullptr) {
            for (int i = 0; i < level; ++i) {
                res.push_back("");
            }
            return;
        } else if (level == 1)
            res.push_back(to_string(n->GetValue()));
        else if (level > 1) {
            getLevelRec(n->GetLeft(), res, level - 1);
            getLevelRec(n->GetRight(), res, level - 1);
        }
    }

    vector<string> getLevel(int level) const {
        vector<string> res;
        getLevelRec(head, res, level);

        return res;
    }

    void beautyPrint() {
        int maxWidth = getMaxElemWidth();
        maxWidth = maxWidth % 2 ? maxWidth : maxWidth + 1;
        for (int h = this->levels(), ws = 1; h > 0; --h) {

            auto lev = this->getLevel(h);
            cout << string(ws / 2, ' ');
            for (int i = 0; i < lev.size(); ++i) {
                int curWidth = lev[i].size();
                if (curWidth == 0) {
                    lev[i] = string(maxWidth, '_');
                    curWidth = maxWidth;
                }
                int l_ws = (maxWidth - curWidth) / 2;
                int r_ws = curWidth % 2 ? l_ws : l_ws + 1;

                printf("%*s%s%*s", l_ws, "", lev[i].c_str(), r_ws, "");
                cout << string(ws, ' ');
            }

            cout << "\n";
            ws = ws * 2 + 1 + (maxWidth / 2) * 2;
        }
    }

    void bfs(auto cb, Node<T>* root = nullptr) {
        if (root == nullptr) root = this->head;

        vector<Node<T>*> nodes = {root};

        while (!nodes.empty()) {
            auto node = nodes[0];

            if (node->GetRight()) {
                nodes.push_back(node->GetRight());
            }
            if (node->GetLeft()) {
                nodes.push_back(node->GetLeft());
            }

            cb(node);

            nodes.erase(nodes.begin(), nodes.begin() + 1);
        }
    }

    void bfsPrint() {
        this->bfs([](Node<T>* node) { cout << node->GetValue() << " "; });
    }
};

int main() {
    Tree<string> expressionTree;

    expressionTree.fromExpression("1 * 2 * 3 + 3 * 4 ^ 2 + 1", Infix);

    cout << "Tree: " << endl;
    expressionTree.beautyPrint();

    cout << "Prefix : " << expressionTree.toExpression(Prefix) << endl;
    cout << "Infix  : " << expressionTree.toExpression(Infix) << endl;
    cout << "Suffix : " << expressionTree.toExpression(Suffix) << endl;

    return 0;
}