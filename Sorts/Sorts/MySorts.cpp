#include <vector>
#include <string>

using namespace std;

#pragma once
typedef typename vector<int> Arr;
typedef typename vector<int>::iterator Iter;

void insertion_sort(Iter begin, Iter end) {
    for (auto i = begin + 1; begin != end && i != end; ++i) {
        auto j = i;
        while (*j < *(j - 1)) {
            swap(*j, *(j - 1));
            if (j - 1 == begin)
                break;
            j = (j - 1);
        }
    }
}

void selection_sort(Iter begin, Iter end) {
    for (auto i = begin + 1; begin != end && i != end; ++i) {
        auto j = i;
        while (*j < *(j - 1)) {
            swap(*j, *(j - 1));
            if (j - 1 == begin)
                break;
            j = (j - 1);
        }
    }
}

void bubble_sort(Iter begin, Iter end) {
    for (auto i = begin; i != end; ++i) {
        bool swapped = false;

        for (auto j = begin; j != end - 1; ++j) {
            if (*j > *(j + 1)) {
                swap(*j, *(j + 1));
                swapped = true;
            }
        }

        if (!swapped) break;
    }
}

void merge_arrays(Iter l_begin, Iter mid, Iter end) {
    auto rbegin = mid;
    while (l_begin != mid && rbegin != end) {
        if (*l_begin > *rbegin)
            swap(*l_begin, *rbegin);
        ++l_begin;
    }
}

void merge_sort(Iter begin, Iter end) {
    if (begin == end || begin == end - 1)
        return;

    auto mid = begin + (end - begin) / 2;
    merge_sort(begin, mid);
    merge_sort(mid, end);
    merge_arrays(begin, mid, end);
}

void shell_sort(Iter begin, Iter end) {
    for (auto d = (end - begin) / 2; d != 0; d /= 2)
        for (auto i = begin + d; i != end; ++i)
            for (auto j = i; j - begin >= d && *j < *(j - d); j -= d)
                swap(*j, *(j - d));
}

Iter partition(Iter begin, Iter end) {
    auto pivot = end - 1;
    auto firsthigh = begin;
    for (auto i = begin; i != end - 1; ++i) {
        if (*i < *pivot) {
            swap(*firsthigh, *i);
            ++firsthigh;
        }
    }
    swap(*pivot, *firsthigh);
    return firsthigh;
}

void quick_sort(Iter begin, Iter end) {
    auto i = begin;
    auto j = end;
    auto mid = begin + (end - begin) / 2;

    while (i < end || j > begin) {
        while (*i < *mid)
            i++;
        while (*j > *mid)
            j--;

        if (i <= j) {
            swap(*i, *j);
            i++;
            j--;
        }
        else {
            if (i < end)
                quick_sort(i, end);
            if (j > begin)
                quick_sort(begin, j);
            return;
        }
    }
}

struct Sort {
    string name;
    void (*sort)(Iter begin, Iter end);
};

vector<Sort> sorts = {
    { "selection sort", selection_sort},
    { "insertion sort", insertion_sort },
    { "bubble sort", bubble_sort },
    { "shell sort", shell_sort },
    { "quick_sort", quick_sort },
    { "merge sort", merge_sort }
};