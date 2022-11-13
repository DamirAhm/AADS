#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "MySorts.cpp"

using namespace std;

struct Generated {
    unsigned len;
    Arr avg_case;
    Arr best_case;
    Arr worst_case;
};  

vector<Generated> generate_data() {
    vector<Generated> generated;

    vector<unsigned> data_lens = { 2, 10, 20, 40, 60, 80, 100, 1000, 10000, 20000, 30000, 50000, 100000 };

    for (unsigned data_len : data_lens) {
        vector<int> best_case_data, worst_case_data, avg_case_data;

        for (unsigned j = 0; j < data_len; ++j) {
            if (j % 10000 == 0) {
                srand(time(0));
            }

            avg_case_data.push_back(rand());
        }

        for (int i = 0; i < data_len; ++i) {
            best_case_data.push_back(i);
            worst_case_data.push_back(int(data_len) - i);
        }

        generated.push_back({ data_len, avg_case_data, best_case_data, worst_case_data });
    }

    return generated;
}

long long get_working_time(void (*sortFunc)(std::vector<int>::iterator, std::vector<int>::iterator),
    std::vector<int> data) {
    auto start = std::chrono::high_resolution_clock::now();

    sortFunc(std::begin(data), std::end(data));

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    return duration.count();
}

int main()
{
    srand(time(0));

    auto generated = generate_data();

    for (auto sort : sorts) {
        cout << sort.name << ": " << endl;

        for (auto data : generated) {
            Arr cp1 = data.avg_case, cp2 = data.best_case, cp3 = data.worst_case;

            auto avg_case = get_working_time(sort.sort,cp1);
            auto best_case = get_working_time(sort.sort, cp2);
            auto worst_case = get_working_time(sort.sort, cp3);

            cout << data.len << "(" << avg_case << "/" << best_case << "/" << worst_case << ")" << " ";
            
            data.avg_case.clear();
            data.best_case.clear();
            data.worst_case.clear();
        }
        cout << endl;
    }
}
