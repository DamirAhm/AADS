//
// Created by damir on 23.04.2023.
//
#include <algorithm>

using namespace std;

const int maxnodes = 200000;
const int maxedges = 1000000;

class BinHeap{
    int edges;
    int last[maxnodes], head[maxedges], previous[maxedges], len[maxedges];
    int priorities[maxnodes], pred[maxnodes];

    int heap[maxnodes];
    int positionsById[maxnodes];
    int idsByPosition[maxnodes];
    int heapSize;
    
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
        swap(heap[i], heap[j]);
        swap(positionsById[i], positionsById[j]);
        swap(idsByPosition[positionsById[i]], idsByPosition[positionsById[j]]);
    }

    void moveUp(int pos) {
        while (pos > 0) {
            int parent = (pos - 1) >> 1;
            if (heap[pos] >= heap[parent]) {
                break;
            }
            hswap(pos, parent);
            pos = parent;
        }
    }

    void add(int id, int priority) {
        heap[heapSize] = priority;
        positionsById[heapSize] = id;
        idsByPosition[id] = heapSize;
        moveUp(heapSize++);
    }

    void increasePriority(int id, int priority) {
        int pos = idsByPosition[id];
        heap[pos] = priority;
        moveUp(pos);
    }

    void moveDown(int pos) {
        while (pos < (heapSize >> 1)) {
            int child = 2 * pos + 1;
            if (child + 1 < heapSize && heap[child + 1] < heap[child]) {
                ++child;
            }
            if (heap[pos] <= heap[child]) {
                break;
            }
            hswap(pos, child);
            pos = child;
        }
    }

    int removeMin() {
        int res = positionsById[0];
        int lastNode = heap[--heapSize];
        if (heapSize > 0) {
            heap[0] = lastNode;
            int id = positionsById[heapSize];
            idsByPosition[id] = 0;
            positionsById[0] = id;
            moveDown(0);
        }
        return res;
    }


    void dijkstra(int s) {
        fill(pred, pred + maxnodes, -1);
        fill(priorities, priorities + maxnodes, INT_MAX);
        priorities[s] = 0;
        heapSize = 0;
        add(s, 0);

        while (heapSize) {
            int u = removeMin();
            for (int e = last[u]; e >= 0; e = previous[e]) {
                int v = head[e];
                int newPriority = priorities[u] + len[e];
                if (priorities[v] > newPriority) {
                    if (priorities[v] == INT_MAX)
                        add(v, newPriority);
                    else
                        increasePriority(v, newPriority);
                    priorities[v] = newPriority;
                    pred[v] = u;
                }
            }
        }
    }
};