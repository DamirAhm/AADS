#include <iostream>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <sstream>
#include <queue>

using namespace std;

string RLE_Compress(string input)
{
    string result;

    for (size_t i = 0; i < input.length(); ++i)
    {
        int count = 1;
        while (i < input.length() - 1 && input[i] == input[i + 1])
        {
            ++count;
            ++i;
        }
        result += to_string(count) + input[i];
    }

    return result;
}

string LZ78_Compress(string input_str){
    map<string, int> dictionary;
    int dict_size = 256;
    string current_str = "", compressed_str = "";
    for(char& c : input_str){
        string new_str = current_str + c;
        if(dictionary.find(new_str) != dictionary.end()){
            current_str = new_str;
        }else{
            compressed_str += to_string(dictionary[current_str]) + c;
            dictionary[new_str] = dict_size++;
            current_str = "";
        }
    }
    if(current_str != ""){
        compressed_str += to_string(dictionary[current_str]);
    }
    return compressed_str;
}

string BWT_Compress(string input_str){
    string suffixes[input_str.size()];
    for(int i=0; i<input_str.size(); i++){
        suffixes[i] = input_str.substr(i) + input_str.substr(0, i);
    }
    sort(suffixes, suffixes+input_str.size());
    string compressed_str = "";
    for(int i=0; i<input_str.size(); i++){
        if(suffixes[i].back() == input_str[0]){
            compressed_str += to_string(i) + "$";
            break;
        }
    }
    for(int i=0; i<input_str.size(); i++){
        compressed_str += suffixes[i][input_str.size()-1];
    }
    return compressed_str;
}

string MTF_Compress(string input_str){
    string compressed_str = "";
    list<char> dictionary;
    for(int i=0; i<256; i++){
        dictionary.push_back(char(i));
    }
    for(char& c : input_str){
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
    Node* left;
    Node* right;
    Node(char c, int freq, Node* left = nullptr, Node* right = nullptr)
            : c(c), freq(freq), left(left), right(right) {}
};

struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

string AC_compress(const string& input) {
    // Build frequency table
    std::unordered_map<char, int> freqTable;
    for (char c: input) {
        freqTable[c]++;
    }

    // Build Huffman tree
    std::priority_queue<Node *, std::vector<Node *>, CompareNodes> minHeap;
    for (const auto &pair: freqTable) {
        minHeap.push(new Node(pair.first, pair.second));
    }
    while (minHeap.size() > 1) {
        Node *left = minHeap.top();
        minHeap.pop();
        Node *right = minHeap.top();
        minHeap.pop();
        Node *parent = new Node('$', left->freq + right->freq, left, right);
        minHeap.push(parent);
    }
    Node *root = minHeap.top();

    // Build code table
    std::unordered_map<char, std::string> codeTable;
    std::queue<std::pair<Node*, std::string>> nodeQueue;
    nodeQueue.push({ root, "" });
    while (!nodeQueue.empty()) {
        Node* node = nodeQueue.front().first;
        std::string code = nodeQueue.front().second;
        nodeQueue.pop();
        if (!node->left && !node->right) {
            codeTable[node->c] = code;
        } else {
            nodeQueue.push({ node->left, code + "0" });
            nodeQueue.push({ node->right, code + "1" });
        }
    }

    // Encode input using code table
    std::string encoded;
    for (char c : input) {
        encoded += codeTable[c];
    }

    // Add code table to encoded string
    std::string codeTableString;
    for (const auto& pair : codeTable) {
        codeTableString += pair.first;
        codeTableString += pair.second;
    }
    encoded = codeTableString + encoded;

    return encoded;
}

void encode(Node* root, string str, unordered_map<char, string>& huffmanCode) {
    if (root == nullptr) {
        return;
    }
    if (!root->left && !root->right) {
        huffmanCode[root->c] = str;
    }
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

string buildHuffmanTree(string text, unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;
    for (auto& pair : freq) {
        pq.push(new Node({ pair.first, pair.second, nullptr, nullptr }));
    }
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node({ '\0', sum, left, right }));
    }
    unordered_map<char, string> huffmanCode;
    encode(pq.top(), "", huffmanCode);

    string encoded = "";
    for (char& ch : text) {
        encoded += huffmanCode[ch];
    }

    return encoded;
}

string HA_Compress(string text) {
    unordered_map<char, int> freq;
    for (char& ch : text) {
        freq[ch]++;
    }
    return buildHuffmanTree(text, freq);
}

struct PPM_Context {
    int order;
    string suffix;
    array<int, 256> symbol_count;
};

template<typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for (InputIterator it = first; it != last; ++it) {
        init += *it;
    }
    return init;
}

string PPM_Compress(string input_str){
    const int MAX_ORDER = 6;
    stringstream compressed_str;
    vector<PPM_Context> contexts(256);
    for(int i=0; i<256; i++){
        contexts[i].order = 0;
        contexts[i].symbol_count.fill(1);
    }
    int order = 1;
    while(order <= 6){
        for(int i=0; i<input_str.size(); i++){
            int ctx_idx = input_str[i];
            PPM_Context& ctx = contexts[ctx_idx];
            if(ctx.order == 0){
                ctx.suffix = input_str.substr(i+1, order-1);
                ctx.suffix += '\0';
            }
            if(ctx.suffix == input_str.substr(i+1, order)){
                ctx.symbol_count[input_str[i+order]]++;
                if(ctx.order == 0){
                    ctx.order = order;
                }
            }
        }
        order++;
    }
    order = 6;
    int idx = 0;
    while(idx < input_str.size()){
        int ctx_idx = input_str[idx];
        PPM_Context& ctx = contexts[ctx_idx];
        while(ctx.order >= 0){
            int total_count = accumulate(ctx.symbol_count.begin(), ctx.symbol_count.end(), 0);
            int symbol = -1;
            int cum_count = 0;
            int count_sum = 0;
            for(int i=0; i<256; i++){
                count_sum += ctx.symbol_count[i];
                if(count_sum > cum_count && cum_count + ctx.symbol_count[i] <= total_count){
                    symbol = i;
                    cum_count += ctx.symbol_count[i];
                }
            }
            if(symbol == -1){
                ctx.order--;
                ctx.suffix.pop_back();
                continue;
            }
            compressed_str << char(symbol);
            idx += ctx.order+1;
            if(idx >= input_str.size()){
                break;
            }
            ctx_idx = input_str[idx];
            ctx = contexts[ctx_idx];
            if(ctx.order == 0){
                ctx.suffix = input_str.substr(idx+1, order-1);
                ctx.suffix += '\0';
            }
            if(ctx.suffix != input_str.substr(idx+1, order)){
                ctx.order--;
                ctx.suffix.pop_back();
            }else{
                ctx.symbol_count[symbol]++;
                if(ctx.order < 6){
                    ctx.order++;
                    ctx.suffix += char(symbol);
                }
            }
        }
        order = 6;
    }
    return compressed_str.str();
}

int main() {
    cout << RLE_Compress("AAABBCCCCDDEEEE");

    return 0;
}
