#include <iostream>
#include <map>
#include <list>
#include <climits>
#include <stdio.h>
#include <memory>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

vector<int> *shortest_order;
int *distance_to_dest;
int num_vertices;


struct node {
	int index;
};


struct adj_node {
	int dest_node; 	
	int edge_len; 
};

struct graph {
	int num_vertices; 	
	map<int, list<adj_node*>> adjacent_list_map; 
};


struct fibonacci_node {
	int degree;                 	
	fibonacci_node *parent;     	
	fibonacci_node *child;      	
	fibonacci_node *left;       	
	fibonacci_node *right;      	
	bool mark;                  	
	bool is_infinity;
	int key;                    	
	int node_index;             
};

struct fibonacci_heap {
	fibonacci_node *min_node;
	int num_nodes;
	fibonacci_heap()
	{
	    num_nodes = 0;
	    min_node = NULL;
	}
};

void fib_heap_insert(fibonacci_heap *fib_heap_obj, fibonacci_node *new_node, int key) {
	fibonacci_node *min_node = fib_heap_obj->min_node;
	new_node->key = key;
	new_node->degree = 0;
	new_node->parent = NULL;
	new_node->child = NULL;
	new_node->left = new_node;
	new_node->right = new_node;
	new_node->mark = false;
	new_node->is_infinity = false;
	if (min_node != NULL) {
		fibonacci_node* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;
	}

	if (min_node == NULL || min_node->key > new_node->key) {
		fib_heap_obj->min_node = new_node;
	}

	fib_heap_obj->num_nodes = fib_heap_obj->num_nodes + 1;
}

void fib_heap_existing_to_root(fibonacci_heap *fib_heap_obj, fibonacci_node *new_node) {
	fibonacci_node *min_node = fib_heap_obj->min_node;
	new_node->parent = NULL; 	
	new_node->mark = false;   

	if (min_node != NULL) {
		fibonacci_node* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;

		if (min_node->key > new_node->key) {
			fib_heap_obj->min_node = new_node;
		}
	} else { 		
		fib_heap_obj->min_node = new_node;
		new_node->right = new_node;
		new_node->left = new_node;
	}
}

fibonacci_heap *fib_heap_make() {
	fibonacci_heap *new_fib_heap = new fibonacci_heap;
	new_fib_heap->num_nodes = 0;
	new_fib_heap->min_node = NULL;
	return new_fib_heap;
}


fibonacci_heap *fib_heap_union(fibonacci_heap *fib_heap_first, fibonacci_heap *fib_heap_second) {

	fibonacci_heap *appended_fib_heap = fib_heap_make();
	fibonacci_node *first_heap_min_node = fib_heap_first->min_node;
	fibonacci_node *second_heap_min_node = fib_heap_second->min_node;

	if (fib_heap_second->min_node == NULL) {
		appended_fib_heap->min_node = first_heap_min_node;
	} else if (fib_heap_first->min_node == NULL) {
		appended_fib_heap->min_node = second_heap_min_node;
	} else { 				
		fibonacci_node *first_heap_min_node_left_temp =
				first_heap_min_node->left;
		fibonacci_node *second_heap_min_node_left_temp =
				second_heap_min_node->left;
		first_heap_min_node->left = second_heap_min_node_left_temp;
		second_heap_min_node->left = first_heap_min_node_left_temp;
		first_heap_min_node_left_temp->right = second_heap_min_node;
		second_heap_min_node_left_temp->right = first_heap_min_node;

		if (second_heap_min_node->key < first_heap_min_node->key) {
			appended_fib_heap->min_node = second_heap_min_node;
		} else {
			appended_fib_heap->min_node = first_heap_min_node;
		}
	}

	appended_fib_heap->num_nodes = fib_heap_first->num_nodes
			+ fib_heap_second->num_nodes;
	return appended_fib_heap;
}

void fib_heap_add_child(fibonacci_node *parent_node, fibonacci_node *new_child_node) {
	if (parent_node->degree == 0) {
		parent_node->child = new_child_node;
		new_child_node->right = new_child_node;
		new_child_node->left = new_child_node;
		new_child_node->parent = parent_node;
	} else {
		fibonacci_node* first_child = parent_node->child;
		fibonacci_node* first_child_left_temp = first_child->left;
		first_child->left = new_child_node;
		new_child_node->right = first_child;
		new_child_node->left = first_child_left_temp;
		first_child_left_temp->right = new_child_node;
	}
	new_child_node->parent = parent_node;
	parent_node->degree = parent_node->degree + 1;
}


void fib_heap_remove_node_from_root(fibonacci_node *node) {
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

void fib_heap_link(fibonacci_heap *heap_inst, fibonacci_node *high_node, fibonacci_node *low_node) {
	fib_heap_remove_node_from_root(high_node);
	fib_heap_add_child(low_node, high_node);
	high_node->mark = false;
}

void fib_heap_consolidate(fibonacci_heap *heap_inst) {
	int node_degree;
	int count = 0, root_count = 0; 
	if (heap_inst->num_nodes > 1) { 		
		int degree_table_size = heap_inst->num_nodes;
		vector<fibonacci_node*> degree_table; 		
		fibonacci_node* current_node = heap_inst->min_node;
		fibonacci_node* start_node = heap_inst->min_node;
		fibonacci_node *existing_node_degree_array, *current_consolidating_node;

		fibonacci_node* temp_node = heap_inst->min_node;
		fibonacci_node* iterating_node = heap_inst->min_node;
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
						fibonacci_node * temp_node = current_consolidating_node;
						current_consolidating_node = existing_node_degree_array;
						existing_node_degree_array = temp_node;
					}
					if (existing_node_degree_array == current_consolidating_node) break;
                    fib_heap_link(heap_inst, existing_node_degree_array,
					current_consolidating_node);
                    degree_table[node_degree] = NULL;
					node_degree++;
				}
			}
			count++;
		}

        heap_inst->min_node = NULL;
		for (int i = 0; i < degree_table.size(); i++) {
			if (degree_table[i] != NULL) {
				fib_heap_existing_to_root(heap_inst, degree_table[i]);
			}
		}
	}
}

fibonacci_node *fib_heap_extract_min(fibonacci_heap *heap_inst) {

	fibonacci_node *min_node = heap_inst->min_node;
	ofstream myfile;
	myfile.open("order_fib.txt", std::ios_base::app);

    	myfile << "FIB -- MIN NODE " << min_node->node_index << endl;

	if (min_node != NULL) {
		int degree = min_node->degree;
		fibonacci_node *current_child = min_node->child;
		fibonacci_node *removed_child;
		int count = 0; 
        while (count < degree) {
			removed_child = current_child;
			current_child = current_child->right;
			fib_heap_existing_to_root(heap_inst, removed_child);
			count++;
		}

        fib_heap_remove_node_from_root(min_node);
		heap_inst->num_nodes = heap_inst->num_nodes - 1;
		if (heap_inst->num_nodes == 0) { 			heap_inst->min_node = NULL;
		} else { 			heap_inst->min_node = min_node->right; 						fibonacci_node *min_node_left_temp = min_node->left;
			heap_inst->min_node->left = min_node_left_temp;
			min_node_left_temp->right = heap_inst->min_node;
						fib_heap_consolidate(heap_inst);
		}
	}
	return min_node;
}


void fib_heap_cut(fibonacci_heap *heap_inst, fibonacci_node *node,
	fibonacci_node *node_parent) {

	fib_heap_remove_node_from_root(node);
	fib_heap_existing_to_root(heap_inst, node);

}

void fib_heap_cascading_cut(fibonacci_heap *heap_inst, fibonacci_node *node) {
	fibonacci_node *parent_node = node->parent;
	if (parent_node != NULL) {
        if (node->mark == false) {
			node->mark = true;
		} else {
		    fib_heap_cut(heap_inst, node, parent_node);
			fib_heap_cascading_cut(heap_inst, parent_node);
		}
	}

}

void fib_heap_decrease_key(fibonacci_heap *heap_inst, fibonacci_node *node_inst, int new_key) {
	int old_key = node_inst->key;

	if (new_key > old_key) {
		return;
	}

	node_inst->key = new_key;
	if (node_inst->parent != NULL) {
		if (node_inst->key < node_inst->parent->key) {
			fibonacci_node *parent_node = node_inst->parent;
			fib_heap_cut(heap_inst, node_inst, node_inst->parent);
			fib_heap_cascading_cut(heap_inst, parent_node);
		}
	}

    if (node_inst->key < heap_inst->min_node->key) {
		heap_inst->min_node = node_inst;
	}

}

graph* create_graph(int graph_size) {
	graph *graph_inst = new graph;
	graph_inst->num_vertices = graph_size;
	return graph_inst;
}


void add_edge(graph* graph_obj, int src, int dest, int edge_len) {
	adj_node *new_node = new adj_node;
	new_node->dest_node = dest;
	new_node->edge_len = edge_len;
	graph_obj->adjacent_list_map[src].push_back(new_node);

		adj_node* new_node_2 = new adj_node;
	new_node_2->dest_node = src;
	new_node_2->edge_len = edge_len;
	graph_obj->adjacent_list_map[dest].push_back(new_node_2);
}

int* dijkstra_normal(graph* graph_instance, int src) {
	int count_marked = 0;
	distance_to_dest = new int[graph_instance->num_vertices];
	bool *marked = new bool[graph_instance->num_vertices];
	for (int i = 0; i < graph_instance->num_vertices; i++) {
		distance_to_dest[i] = INT_MAX;
	}
    for (int i = 0; i < graph_instance->num_vertices; i++) {
		marked[i] = false;
	}

	distance_to_dest[src] = 0; 	
	
	while (count_marked < graph_instance->num_vertices) { 		
		get_min_distant_unmarked_node(graph_instance, distance_to_dest, marked);
		count_marked++;
	}

	return distance_to_dest;
}

fibonacci_node** dijkstra_fibanocci(graph* graph_instance, int src) {
	int count_marked = 0;
    fibonacci_node **node_array =
			new fibonacci_node*[graph_instance->num_vertices];
	bool *marked = new bool[graph_instance->num_vertices];
	fibonacci_heap *heap_inst = new fibonacci_heap;
    
	for (int i = 0; i < graph_instance->num_vertices; i++) {
		marked[i] = false;
		node_array[i] = new fibonacci_node;
		if (i == src) {
			fib_heap_insert(heap_inst, node_array[i], 0);
		} else {
			fib_heap_insert(heap_inst, node_array[i], INT_MAX);
		}
		node_array[i]->node_index = i;
	}

	while (count_marked < graph_instance->num_vertices) { 		
		get_min_distant_unmarked_node_fib_heap(graph_instance, heap_inst, node_array, marked);
		count_marked++;
	}


	return node_array;
}

int get_min_distant_unmarked_node(graph* graph_obj, int *distance_to_dest,
	bool *marked) {
    int min_distance = INT_MAX;
	int min_node_index;

	for (int i = 0; i < graph_obj->num_vertices; i++) {
		if ((!marked[i]) && (min_distance >= distance_to_dest[i])) {
			min_distance = distance_to_dest[i];
			min_node_index = i;
		}
	}
    ofstream myfile;
	myfile.open("order_normal.txt", std::ios_base::app);
	myfile << "NORMAL -- MIN NODE " << min_node_index << ":";

	if (shortest_order[min_node_index].size() < 0) {
		for (int i = 0; i < shortest_order[min_node_index].size(); i++) {
			myfile << shortest_order[min_node_index][i] << "-";
		}
	}

	myfile << endl;
	myfile.close();
		marked[min_node_index] = true;

		list<adj_node*> adj_list_for_node =
			graph_obj->adjacent_list_map[min_node_index];

	for (list<adj_node*>::iterator it = adj_list_for_node.begin();
			it != adj_list_for_node.end(); ++it) {
		int adj_node_index = (*it)->dest_node;
		if (marked[adj_node_index] == false) {
			if (distance_to_dest[adj_node_index]
					> distance_to_dest[min_node_index] + (*it)->edge_len) {
				distance_to_dest[adj_node_index] =
						distance_to_dest[min_node_index] + (*it)->edge_len;
				shortest_order[adj_node_index].push_back(min_node_index);
			}
		}
	}
	return min_node_index;
}


int get_min_distant_unmarked_node_fib_heap(graph* graph_obj,
	fibonacci_heap *heap, fibonacci_node **node_array, bool *marked) {

	int min_distance = INT_MAX;
	fibonacci_node *min_node = fib_heap_extract_min(heap);
	int min_node_index;

	if (min_node != NULL) {
		min_node_index = min_node->node_index;
		marked[min_node_index] = true;
		list<adj_node*> adj_list_for_node = graph_obj->adjacent_list_map[min_node_index];
		for (list<adj_node*>::iterator it = adj_list_for_node.begin();
				it != adj_list_for_node.end(); ++it) {
			int adj_node_index = (*it)->dest_node;

			if (marked[adj_node_index] == false) {
				if (node_array[adj_node_index]->key > node_array[min_node_index]->key + (*it)->edge_len) {
					fib_heap_decrease_key(heap, node_array[adj_node_index], node_array[min_node_index]->key + (*it)->edge_len);
				}
			}
		}
	}

	return min_node_index;
}

graph* generate_random_graph(int vertices, int density) {
	int num_nodes = vertices;
	graph* my_graph = create_graph(num_nodes);
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
			add_edge(my_graph, start, end, weight);
			count++;
		}
	}

	return my_graph;
}

int main() {
    int nodes, density, src_node, dij_start_index;
    nodes = 20;
    density = 40;
    dij_start_index = 2;

    graph* my_graph = generate_random_graph(nodes, density);
    shortest_order = new vector<int> [nodes];
;
    dijkstra_normal(my_graph, dij_start_index);
    dijkstra_fibanocci(my_graph, dij_start_index);

	return 0;
}