//
// Created by damir on 23.04.2023.
//
#include <vector>
#include <fstream>
#include <map>
#include <list>

using namespace std;

struct adjNode {
    int destNode;
    int edgeLen;
};

struct graph {
    int numVertices;
    map<int, list<adjNode*>> adjacentListMap;
};

struct fibNode {
    int degree;
    fibNode *parent;
    fibNode *child;
    fibNode *left;
    fibNode *right;
    bool mark;
    bool isInfinity;
    int key;
    int nodeIndex;
};

struct fibHeap {
    fibNode *minNode;
    int numNodes;
    fibHeap()
    {
        numNodes = 0;
        minNode = nullptr;
    }
};

class FibHeap {
    fibHeap* heap = new fibHeap;
    
    void insert(fibNode *new_node, int key) {
        fibNode *min_node = heap->minNode;
        new_node->key = key;
        new_node->degree = 0;
        new_node->parent = nullptr;
        new_node->child = nullptr;
        new_node->left = new_node;
        new_node->right = new_node;
        new_node->mark = false;
        new_node->isInfinity = false;
        if (min_node != nullptr) {
            fibNode* min_left_temp = min_node->left;
            min_node->left = new_node;
            new_node->right = min_node;
            new_node->left = min_left_temp;
            min_left_temp->right = new_node;
        }

        if (min_node == nullptr || min_node->key > new_node->key) {
            heap->minNode = new_node;
        }

        heap->numNodes = heap->numNodes + 1;
    }

    void toRoot(fibNode *new_node) {
        fibNode *min_node = heap->minNode;
        new_node->parent = nullptr;
        new_node->mark = false;

        if (min_node != nullptr) {
            fibNode* min_left_temp = min_node->left;
            min_node->left = new_node;
            new_node->right = min_node;
            new_node->left = min_left_temp;
            min_left_temp->right = new_node;

            if (min_node->key > new_node->key) {
                heap->minNode = new_node;
            }
        } else {
            heap->minNode = new_node;
            new_node->right = new_node;
            new_node->left = new_node;
        }
    }

    fibHeap *unionHeaps(fibHeap *fib_heap_first, fibHeap *fib_heap_second) {
        fibNode *first_heap_min_node = fib_heap_first->minNode;
        fibNode *second_heap_min_node = fib_heap_second->minNode;

        if (fib_heap_second->minNode == nullptr) {
            heap->minNode = first_heap_min_node;
        } else if (fib_heap_first->minNode == nullptr) {
            heap->minNode = second_heap_min_node;
        } else {
            fibNode *first_heap_min_node_left_temp =
                    first_heap_min_node->left;
            fibNode *second_heap_min_node_left_temp =
                    second_heap_min_node->left;
            first_heap_min_node->left = second_heap_min_node_left_temp;
            second_heap_min_node->left = first_heap_min_node_left_temp;
            first_heap_min_node_left_temp->right = second_heap_min_node;
            second_heap_min_node_left_temp->right = first_heap_min_node;

            if (second_heap_min_node->key < first_heap_min_node->key) {
                heap->minNode = second_heap_min_node;
            } else {
                heap->minNode = first_heap_min_node;
            }
        }

        heap->numNodes = fib_heap_first->numNodes + fib_heap_second->numNodes;
        return heap;
    }

    void addChild(fibNode *parent_node, fibNode *new_child_node) {
        if (parent_node->degree == 0) {
            parent_node->child = new_child_node;
            new_child_node->right = new_child_node;
            new_child_node->left = new_child_node;
            new_child_node->parent = parent_node;
        } else {
            fibNode* first_child = parent_node->child;
            fibNode* first_child_left_temp = first_child->left;
            first_child->left = new_child_node;
            new_child_node->right = first_child;
            new_child_node->left = first_child_left_temp;
            first_child_left_temp->right = new_child_node;
        }
        new_child_node->parent = parent_node;
        parent_node->degree = parent_node->degree + 1;
    }

    void removeFromRoot(fibNode *node) {
        if (node->right != node) {
            node->right->left = node->left;
            node->left->right = node->right;
        }

        if (node->parent != nullptr) {
            int parent_degree = node->parent->degree;
            if (parent_degree == 1) {
                node->parent->child = nullptr;
            } else {
                node->parent->child = node->right;
            }
            node->parent->degree = node->parent->degree - 1;
        }
    }

    void link(fibNode *high_node, fibNode *low_node) {
        removeFromRoot(high_node);
        addChild(low_node, high_node);
        high_node->mark = false;
    }

    void consolidate() {
        int node_degree;
        int count = 0, root_count = 0;
        if (heap->numNodes > 1) {
            int degree_table_size = heap->numNodes;
            vector<fibNode*> degree_table;
            fibNode* current_node = heap->minNode;
            fibNode* start_node = heap->minNode;
            fibNode *existing_node_degree_array, *current_consolidating_node;

            fibNode* temp_node = heap->minNode;
            fibNode* iterating_node = heap->minNode;

            do {
                root_count++;
                iterating_node = iterating_node->right;
            } while (iterating_node != temp_node);

            while (count < root_count) {
                current_consolidating_node = current_node;
                current_node = current_node->right;
                node_degree = current_consolidating_node->degree;
                while (true) {
                    while (node_degree >= degree_table.size()) {
                        degree_table.push_back(nullptr); 				}
                    if (degree_table[node_degree] == nullptr) {
                        degree_table[node_degree] = current_consolidating_node;
                        break;
                    } else {
                        existing_node_degree_array = degree_table[node_degree];
                        if (current_consolidating_node->key > existing_node_degree_array->key) {
                            fibNode * temp_node = current_consolidating_node;
                            current_consolidating_node = existing_node_degree_array;
                            existing_node_degree_array = temp_node;
                        }
                        if (existing_node_degree_array == current_consolidating_node) break;
                        link(existing_node_degree_array,
                                current_consolidating_node);
                        degree_table[node_degree] = nullptr;
                        node_degree++;
                    }
                }
                count++;
            }

            heap->minNode = nullptr;
            for (int i = 0; i < degree_table.size(); i++) {
                if (degree_table[i] != nullptr) {
                    toRoot(degree_table[i]);
                }
            }
        }
    }

    fibNode *extractMin() {
        fibNode *min_node = heap->minNode;

        if (min_node != nullptr) {
            int degree = min_node->degree;
            fibNode *current_child = min_node->child;
            fibNode *removed_child;
            int count = 0;
            while (count < degree) {
                removed_child = current_child;
                current_child = current_child->right;
                toRoot(removed_child);
                count++;
            }

            removeFromRoot(min_node);

            heap->numNodes = heap->numNodes - 1;

            if (heap->numNodes == 0) {
                heap->minNode = nullptr;
            } else {
                heap->minNode = min_node->right;
                fibNode *min_node_left_temp = min_node->left;
                heap->minNode->left = min_node_left_temp;
                min_node_left_temp->right = heap->minNode;
                consolidate();
            }
        }

        return min_node;
    }

    void cut(fibNode *node) {
        removeFromRoot(node);
        toRoot(node);
    }

    void cascCut(fibNode *node) {
        fibNode *parent_node = node->parent;
        if (parent_node != NULL) {
            if (node->mark == false) {
                node->mark = true;
            } else {
                cut(node);
                cascCut(parent_node);
            }
        }

    }

    void decreaseKey(fibNode *node_inst, int new_key) {
        int old_key = node_inst->key;

        if (new_key > old_key) {
            return;
        }

        node_inst->key = new_key;
        if (node_inst->parent != nullptr) {
            if (node_inst->key < node_inst->parent->key) {
                fibNode *parent_node = node_inst->parent;
                cut(node_inst);
                cascCut(parent_node);
            }
        }

        if (node_inst->key < heap->minNode->key) {
            heap->minNode = node_inst;
        }
    }

    int getMinUnmarkedDist(graph* graph_obj, fibNode **node_array, bool *marked) {
        int min_distance = INT_MAX;
        fibNode *min_node = extractMin();
        int min_node_index;

        if (min_node != NULL) {
            min_node_index = min_node->nodeIndex;
            marked[min_node_index] = true;
            list<adjNode*> adj_list_for_node = graph_obj->adjacentListMap[min_node_index];
            for (list<adjNode*>::iterator it = adj_list_for_node.begin();
                 it != adj_list_for_node.end(); ++it) {
                int adj_node_index = (*it)->destNode;

                if (marked[adj_node_index] == false) {
                    if (node_array[adj_node_index]->key > node_array[min_node_index]->key + (*it)->edgeLen) {
                        decreaseKey(node_array[adj_node_index], node_array[min_node_index]->key + (*it)->edgeLen);
                    }
                }
            }
        }

        return min_node_index;
    }

public:
    fibNode** dijkstra(graph* graph_instance, int src) {
        int count_marked = 0;
        fibNode **node_array = new fibNode*[graph_instance->numVertices];
        bool *marked = new bool[graph_instance->numVertices];

        for (int i = 0; i < graph_instance->numVertices; i++) {
            marked[i] = false;
            node_array[i] = new fibNode;
            if (i == src) {
                insert(node_array[i], 0);
            } else {
                insert(node_array[i], INT_MAX);
            }
            node_array[i]->nodeIndex = i;
        }

        while (count_marked < graph_instance->numVertices) {
            getMinUnmarkedDist(graph_instance, node_array, marked);
            count_marked++;
        }

        return node_array;
    }
};