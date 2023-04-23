#include <iostream>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <sstream>
#include <queue>

using namespace std;

string RLE_Compress(string input) {
    string result;

    for (size_t i = 0; i < input.length(); ++i) {
        int count = 1;
        while (i < input.length() - 1 && input[i] == input[i + 1]) {
            ++count;
            ++i;
        }
        result += to_string(count) + input[i];
    }

    return result;
}

struct LZNode {
    int pos;
    char next;
};
list<LZNode> LZ78_Compress(string input) {
    string substring = "";
    map<string, int> dictionary = {};
    list<LZNode> compressed_data = {};

    for (int i = 0; i < input.length(); i++) {
        if (dictionary.find(substring + input[i]) != dictionary.end()) {
            substring += input[i];
        } else {
            compressed_data.push_back({dictionary[substring], input[i]});
            dictionary[substring + input[i]] = dictionary.size();
            substring = "";
        }
    }

    if (!substring.empty()) {
        char last_char = substring[substring.length() - 1];
        substring.pop_back();
        compressed_data.push_back({dictionary[substring], last_char});
    }

    return compressed_data;
}

string BWT_Compress(string input) {
    vector<string> rotations;
    string transformed_data = "";

    for (size_t i = 0; i < input.length(); ++i) {
        rotations.push_back(input.substr(i) + input.substr(0, i));
    }

    sort(rotations.begin(), rotations.end());

    for (const auto &rotation : rotations) {
        transformed_data += rotation.back();
    }

    return transformed_data;
}

string MTF_Compress(string input_str) {
    string compressed_str = "";
    list<char> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.push_back(char(i));
    }
    for (char &c: input_str) {
        auto it = find(dictionary.begin(), dictionary.end(), c);
        int idx = distance(dictionary.begin(), it);
        compressed_str += to_string(idx) + ",";
        dictionary.erase(it);
        dictionary.push_front(c);
    }
    compressed_str.pop_back();
    return compressed_str;
}

struct Node {
    char c;
    int freq;
    Node *left;
    Node *right;

    Node(char c, int freq, Node *left = nullptr, Node *right = nullptr)
            : c(c), freq(freq), left(left), right(right) {}
};

struct CompareNodes {
    bool operator()(Node *a, Node *b) {
        return a->freq > b->freq;
    }
};

void encode(Node *root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr) {
        return;
    }
    if (!root->left && !root->right) {
        huffmanCode[root->c] = str;
    }
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

string buildHuffmanTree(string text, unordered_map<char, int> &freq) {
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;
    for (auto &pair: freq) {
        pq.push(new Node({pair.first, pair.second, nullptr, nullptr}));
    }
    while (pq.size() != 1) {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node({'\0', sum, left, right}));
    }
    unordered_map<char, string> huffmanCode;
    encode(pq.top(), "", huffmanCode);

    string encoded = "";
    for (char &ch: text) {
        encoded += huffmanCode[ch];
    }

    string codeTableString;
    for (const auto &pair: huffmanCode) {
        codeTableString += pair.first;
        codeTableString += pair.second;
    }

    return codeTableString + encoded;
}

string HA_Compress(string text) {
    unordered_map<char, int> freq;
    for (char &ch: text) {
        freq[ch]++;
    }
    return buildHuffmanTree(text, freq);
}

string AC_Compress(string text) {
    std::map<char, int> freq_table;
    for (char c : text) {
        freq_table[c]++;
    }

    // Compute the cumulative frequency table
    std::map<char, double> cum_freq_table;
    double cum_freq = 0.0;
    for (auto it = freq_table.begin(); it != freq_table.end(); ++it) {
        cum_freq_table[it->first] = cum_freq;
        cum_freq += it->second;
    }

    // Normalize the cumulative frequency table
    double total_freq = cum_freq;
    for (auto it = cum_freq_table.begin(); it != cum_freq_table.end(); ++it) {
        it->second /= total_freq;
    }

    // Initialize the range
    double range_low = 0.0;
    double range_high = 1.0;

    // Encode each character in the input string
    for (char c : text) {
        double range_size = range_high - range_low;
        range_high = range_low + range_size * cum_freq_table[c] + range_size * freq_table[c] / total_freq;
        range_low = range_low + range_size * cum_freq_table[c];
    }

    return to_string((range_low + range_high) / 2);
}

string ppm_encode(const string& input) {
    map<string, int> freq;
    string output = "";

    for (int i = 0; i < input.length(); i++) {
        string context = "";
        int max_order = min(i, 5);
        for (int j = 1; j <= max_order; j++) {
            context = input.substr(i - j, j);
            if (freq.count(context)) {
                break;
            }
        }

        if (freq.count(context)) {
            char best_char = 0;
            int best_freq = 0;
            for (char c = 0; c < 127; c++) {
                string symbol = context + c;
                if (freq.count(symbol) && freq[symbol] > best_freq) {
                    best_char = c;
                    best_freq = freq[symbol];
                }
            }
            output += best_char;
            freq[context + best_char]++;
        } else {
            output += input[i];
            freq[string(1, input[i])]++;
        }
    }

    return output;
}

int main() {
    return 0;
}
