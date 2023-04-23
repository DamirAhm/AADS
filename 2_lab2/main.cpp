#include <map>
#include <list>
#include <climits>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "FibHeap.cpp"
#include "BinHeap.cpp"

using namespace std;

vector<int> *shortest_order;
int *distance_to_dest;
int num_vertices;

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

graph* generateRandomGraph(int vertices, int density) {
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

int main() {
    graph* randomGraph = generateRandomGraph(num_vertices, .5);

    int src;
    cin >> src;

    int* res = dijkstra(randomGraph, src);

    FibHeap* fib = new FibHeap();
    fibNode** resFib = fib->dijkstra(randomGraph, src);

    BinHeap* bin = new BinHeap();
    auto resBin = bin->dijkstra(randomGraph, src);

	return 0;
}
