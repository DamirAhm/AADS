#include <iostream>
#include <map>
#include <list>
#include <climits>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

const int maxnodes = 200000;
const int maxedges = 1000000;

int edges;
int last[maxnodes], head[maxedges], previous[maxedges], len[maxedges];
int prio[maxnodes], pred[maxnodes];

int h[maxnodes];
int pos2Id[maxnodes];
int id2Pos[maxnodes];
int hsize;

vector<int> *shortest_order;
int *distance_to_dest;
int num_vertices;

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
        minNode = NULL;
	}
};

void insertFib(fibHeap *fib_heap_obj, fibNode *new_node, int key) {
	fibNode *min_node = fib_heap_obj->minNode;
	new_node->key = key;
	new_node->degree = 0;
	new_node->parent = NULL;
	new_node->child = NULL;
	new_node->left = new_node;
	new_node->right = new_node;
	new_node->mark = false;
	new_node->isInfinity = false;
	if (min_node != NULL) {
		fibNode* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;
	}

	if (min_node == NULL || min_node->key > new_node->key) {
		fib_heap_obj->minNode = new_node;
	}

	fib_heap_obj->numNodes = fib_heap_obj->numNodes + 1;
}

void toRootFib(fibHeap *fib_heap_obj, fibNode *new_node) {
	fibNode *min_node = fib_heap_obj->minNode;
	new_node->parent = NULL;
	new_node->mark = false;

	if (min_node != NULL) {
		fibNode* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;

		if (min_node->key > new_node->key) {
			fib_heap_obj->minNode = new_node;
		}
	} else {
		fib_heap_obj->minNode = new_node;
		new_node->right = new_node;
		new_node->left = new_node;
	}
}

fibHeap *makeFib() {
	fibHeap *new_fib_heap = new fibHeap;
	new_fib_heap->numNodes = 0;
	new_fib_heap->minNode = NULL;
	return new_fib_heap;
}


fibHeap *unionFib(fibHeap *fib_heap_first, fibHeap *fib_heap_second) {
	fibHeap *appended_fib_heap = makeFib();
	fibNode *first_heap_min_node = fib_heap_first->minNode;
	fibNode *second_heap_min_node = fib_heap_second->minNode;

	if (fib_heap_second->minNode == NULL) {
		appended_fib_heap->minNode = first_heap_min_node;
	} else if (fib_heap_first->minNode == NULL) {
		appended_fib_heap->minNode = second_heap_min_node;
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
			appended_fib_heap->minNode = second_heap_min_node;
		} else {
			appended_fib_heap->minNode = first_heap_min_node;
		}
	}

	appended_fib_heap->numNodes = fib_heap_first->numNodes + fib_heap_second->numNodes;
	return appended_fib_heap;
}

void addChildFib(fibNode *parent_node, fibNode *new_child_node) {
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


void removeFromRootFib(fibNode *node) {
    if (node->right != node) {
		node->right->left = node->left;
		node->left->right = node->right;
	}

	if (node->parent != NULL) {
		int parent_degree = node->parent->degree;
		if (parent_degree == 1) {
            node->parent->child = NULL;
		} else {
			node->parent->child = node->right;
		}
		node->parent->degree = node->parent->degree - 1;
	}
}

void linkFib(fibHeap *heap_inst, fibNode *high_node, fibNode *low_node) {
    removeFromRootFib(high_node);
    addChildFib(low_node, high_node);
	high_node->mark = false;
}

void consolidateFib(fibHeap *heap_inst) {
	int node_degree;
	int count = 0, root_count = 0;
	if (heap_inst->numNodes > 1) {
		int degree_table_size = heap_inst->numNodes;
		vector<fibNode*> degree_table;
		fibNode* current_node = heap_inst->minNode;
		fibNode* start_node = heap_inst->minNode;
		fibNode *existing_node_degree_array, *current_consolidating_node;

		fibNode* temp_node = heap_inst->minNode;
		fibNode* iterating_node = heap_inst->minNode;

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
					degree_table.push_back(NULL); 				}
				if (degree_table[node_degree] == NULL) {
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
                    linkFib(heap_inst, existing_node_degree_array,
                            current_consolidating_node);
                    degree_table[node_degree] = NULL;
					node_degree++;
				}
			}
			count++;
		}

        heap_inst->minNode = NULL;
		for (int i = 0; i < degree_table.size(); i++) {
			if (degree_table[i] != NULL) {
                toRootFib(heap_inst, degree_table[i]);
			}
		}
	}
}

fibNode *extractMinFib(fibHeap *heap_inst) {
	fibNode *min_node = heap_inst->minNode;
	ofstream myfile;

	if (min_node != NULL) {
		int degree = min_node->degree;
		fibNode *current_child = min_node->child;
		fibNode *removed_child;
		int count = 0;
        while (count < degree) {
			removed_child = current_child;
			current_child = current_child->right;
            toRootFib(heap_inst, removed_child);
			count++;
		}

        removeFromRootFib(min_node);

		heap_inst->numNodes = heap_inst->numNodes - 1;

		if (heap_inst->numNodes == 0) {
			heap_inst->minNode = NULL;
		} else {
			heap_inst->minNode = min_node->right;
			fibNode *min_node_left_temp = min_node->left;
			heap_inst->minNode->left = min_node_left_temp;
			min_node_left_temp->right = heap_inst->minNode;
            consolidateFib(heap_inst);
		}
	}
	return min_node;
}

int getMinDistUnmarked(graph* graph_obj, int *distance_to_dest, bool *marked) {
    int min_distance = INT_MAX;
    int min_node_index;

    for (int i = 0; i < graph_obj->numVertices; i++) {
        if ((!marked[i]) && (min_distance >= distance_to_dest[i])) {
            min_distance = distance_to_dest[i];
            min_node_index = i;
        }
    }

    marked[min_node_index] = true;

    list<adjNode*> adj_list_for_node =
            graph_obj->adjacentListMap[min_node_index];

    for (list<adjNode*>::iterator it = adj_list_for_node.begin();
         it != adj_list_for_node.end(); ++it) {
        int adj_node_index = (*it)->destNode;
        if (marked[adj_node_index] == false) {
            if (distance_to_dest[adj_node_index] > distance_to_dest[min_node_index] + (*it)->edgeLen) {
                distance_to_dest[adj_node_index] =
                        distance_to_dest[min_node_index] + (*it)->edgeLen;
                shortest_order[adj_node_index].push_back(min_node_index);
            }
        }
    }
    return min_node_index;
}

void cutFib(fibHeap *heap_inst, fibNode *node,
            fibNode *node_parent) {

    removeFromRootFib(node);
    toRootFib(heap_inst, node);
}

void cascCutFib(fibHeap *heap_inst, fibNode *node) {
	fibNode *parent_node = node->parent;
	if (parent_node != NULL) {
        if (node->mark == false) {
			node->mark = true;
		} else {
            cutFib(heap_inst, node, parent_node);
            cascCutFib(heap_inst, parent_node);
		}
	}

}

void decreaseKeyFib(fibHeap *heap_inst, fibNode *node_inst, int new_key) {
	int old_key = node_inst->key;

	if (new_key > old_key) {
		return;
	}

	node_inst->key = new_key;
	if (node_inst->parent != NULL) {
		if (node_inst->key < node_inst->parent->key) {
			fibNode *parent_node = node_inst->parent;
            cutFib(heap_inst, node_inst, node_inst->parent);
            cascCutFib(heap_inst, parent_node);
		}
	}

    if (node_inst->key < heap_inst->minNode->key) {
		heap_inst->minNode = node_inst;
	}

}

graph* createGraph(int graph_size) {
	graph *graph_inst = new graph;
	graph_inst->numVertices = graph_size;

	return graph_inst;
}


void addEdge(graph* graph_obj, int src, int dest, int edge_len) {
	adjNode *new_node = new adjNode;
	new_node->destNode = dest;
	new_node->edgeLen = edge_len;
	graph_obj->adjacentListMap[src].push_back(new_node);

	adjNode* new_node_2 = new adjNode;
	new_node_2->destNode = src;
	new_node_2->edgeLen = edge_len;
	graph_obj->adjacentListMap[dest].push_back(new_node_2);
}

int* dijkstra(graph* graph_instance, int src) {
	int count_marked = 0;
	distance_to_dest = new int[graph_instance->numVertices];
	bool *marked = new bool[graph_instance->numVertices];
	for (int i = 0; i < graph_instance->numVertices; i++) {
		distance_to_dest[i] = INT_MAX;
	}
    for (int i = 0; i < graph_instance->numVertices; i++) {
		marked[i] = false;
	}

	distance_to_dest[src] = 0;

	while (count_marked < graph_instance->numVertices) {
        getMinDistUnmarked(graph_instance, distance_to_dest, marked);
		count_marked++;
	}

	return distance_to_dest;
}

int getMinUnmarkedDistFib(graph* graph_obj, fibHeap *heap, fibNode **node_array, bool *marked) {
    int min_distance = INT_MAX;
    fibNode *min_node = extractMinFib(heap);
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
                    decreaseKeyFib(heap, node_array[adj_node_index], node_array[min_node_index]->key + (*it)->edgeLen);
                }
            }
        }
    }

    return min_node_index;
}

fibNode** dijkstraFib(graph* graph_instance, int src) {
	int count_marked = 0;
    fibNode **node_array = new fibNode*[graph_instance->numVertices];
	bool *marked = new bool[graph_instance->numVertices];
	fibHeap *heap_inst = new fibHeap;

	for (int i = 0; i < graph_instance->numVertices; i++) {
		marked[i] = false;
		node_array[i] = new fibNode;
		if (i == src) {
            insertFib(heap_inst, node_array[i], 0);
		} else {
            insertFib(heap_inst, node_array[i], INT_MAX);
		}
		node_array[i]->nodeIndex = i;
	}

	while (count_marked < graph_instance->numVertices) {
        getMinUnmarkedDistFib(graph_instance, heap_inst, node_array, marked);
		count_marked++;
	}

	return node_array;
}

graph* randomGraph(int vertices, int density) {
	int num_nodes = vertices;
	graph* my_graph = createGraph(num_nodes);
	map<pair<int, int>, int> check_map;
	long double num_edges = num_nodes * (num_nodes - 1) / 2;
	num_edges = num_edges * density / 100.0;
	long double count = 0;

	while (count < num_edges) {
		int start, end, weight;
		start = rand() % num_nodes;
		end = rand() % num_nodes;
		if (start > end) {
			int temp = start;
			start = end;
			end = temp;
		}
		pair<int, int> edge(start, end);
		weight = (rand() % 1000) + 1;

		if (check_map.find(edge) == check_map.end() and start != end) {
			check_map[edge] = 1;
            addEdge(my_graph, start, end, weight);
			count++;
		}
	}

	return my_graph;
}

void graphClear() {
    fill(last, last + maxnodes, -1);
    edges = 0;
}

void addEdge(int u, int v, int length) {
    head[edges] = v;
    len[edges] = length;
    previous[edges] = last[u];
    last[u] = edges++;
}

void hswap(int i, int j) {
    swap(h[i], h[j]);
    swap(pos2Id[i], pos2Id[j]);
    swap(id2Pos[pos2Id[i]], id2Pos[pos2Id[j]]);
}

void moveUp(int pos) {
    while (pos > 0) {
        int parent = (pos - 1) >> 1;
        if (h[pos] >= h[parent]) {
            break;
        }
        hswap(pos, parent);
        pos = parent;
    }
}

void add(int id, int prio) {
    h[hsize] = prio;
    pos2Id[hsize] = id;
    id2Pos[id] = hsize;
    moveUp(hsize++);
}

void increasePriority(int id, int prio) {
    int pos = id2Pos[id];
    h[pos] = prio;
    moveUp(pos);
}

void moveDown(int pos) {
    while (pos < (hsize >> 1)) {
        int child = 2 * pos + 1;
        if (child + 1 < hsize && h[child + 1] < h[child]) {
            ++child;
        }
        if (h[pos] <= h[child]) {
            break;
        }
        hswap(pos, child);
        pos = child;
    }
}

int removeMin() {
    int res = pos2Id[0];
    int lastNode = h[--hsize];
    if (hsize > 0) {
        h[0] = lastNode;
        int id = pos2Id[hsize];
        id2Pos[id] = 0;
        pos2Id[0] = id;
        moveDown(0);
    }
    return res;
}


void dijkstra(int s) {
    fill(pred, pred + maxnodes, -1);
    fill(prio, prio + maxnodes, INT_MAX);
    prio[s] = 0;
    hsize = 0;
    add(s, 0);

    while (hsize) {
        int u = removeMin();
        for (int e = last[u]; e >= 0; e = previous[e]) {
            int v = head[e];
            int nprio = prio[u] + len[e];
            if (prio[v] > nprio) {
                if (prio[v] == INT_MAX)
                    add(v, nprio);
                else
                    increasePriority(v, nprio);
                prio[v] = nprio;
                pred[v] = u;
            }
        }
    }
}

int main() {
	return 0;
}
