#include <bits/stdc++.h>
using namespace std;

class HuffmanCoding {
private:
    struct HeapNode {
        char ch;
        int freq;
        HeapNode* left;
        HeapNode* right;

        HeapNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    };

    struct Compare {
        bool operator()(HeapNode* l, HeapNode* r) {
            return l->freq > r->freq;
        }
    };

    unordered_map<char, string> codes;
    unordered_map<string, char> reverse_mapping;
    priority_queue<HeapNode*, vector<HeapNode*>, Compare> heap;

    void make_frequency_dict(const string& text, unordered_map<char, int>& frequency) {
        for (char ch : text) {
            frequency[ch]++;
        }
    }

    void make_heap(const unordered_map<char, int>& frequency) {
        for (auto& pair : frequency) {
            heap.push(new HeapNode(pair.first, pair.second));
        }
    }

    void merge_nodes() {
        while (heap.size() > 1) {
            HeapNode* node1 = heap.top(); heap.pop();
            HeapNode* node2 = heap.top(); heap.pop();

            HeapNode* merged = new HeapNode('\0', node1->freq + node2->freq);
            merged->left = node1;
            merged->right = node2;

            heap.push(merged);
        }
    }

    void make_codes_helper(HeapNode* root, string current_code) {
        if (!root) return;

        if (root->ch != '\0') {
            codes[root->ch] = current_code;
            reverse_mapping[current_code] = root->ch;
            return;
        }

        make_codes_helper(root->left, current_code + "0");
        make_codes_helper(root->right, current_code + "1");
    }

    void make_codes() {
        HeapNode* root = heap.top();
        make_codes_helper(root, "");
    }

    string get_encoded_text(const string& text) {
        string encoded = "";
        for (char ch : text) {
            encoded += codes[ch];
        }
        return encoded;
    }

    string pad_encoded_text(string encoded_text) {
        int extra_padding = 8 - encoded_text.size() % 8;
        for (int i = 0; i < extra_padding; i++) {
            encoded_text += "0";
        }
        bitset<8> padding_bits(extra_padding);
        return padding_bits.to_string() + encoded_text;
    }

    vector<unsigned char> get_byte_array(const string& padded_encoded_text) {
        vector<unsigned char> byte_array;
        for (size_t i = 0; i < padded_encoded_text.size(); i += 8) {
            bitset<8> byte_bits(padded_encoded_text.substr(i, 8));
            byte_array.push_back((unsigned char)byte_bits.to_ulong());
        }
        return byte_array;
    }

    string remove_padding(const string& padded_text) {
        int padding = bitset<8>(padded_text.substr(0, 8)).to_ulong();
        return padded_text.substr(8, padded_text.size() - 8 - padding);
    }

    string decode_text(const string& encoded_text) {
        string current_code = "", decoded = "";
        for (char bit : encoded_text) {
            current_code += bit;
            if (reverse_mapping.find(current_code) != reverse_mapping.end()) {
                decoded += reverse_mapping[current_code];
                current_code = "";
            }
        }
        return decoded;
    }

public:
    void compress(const string& input_path) {
        ifstream infile(input_path);
        if (!infile) {
            cout << "File not found!\n";
            return;
        }

        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        unordered_map<char, int> frequency;
        make_frequency_dict(text, frequency);
        make_heap(frequency);
        merge_nodes();
        make_codes();

        string encoded = get_encoded_text(text);
        string padded = pad_encoded_text(encoded);
        vector<unsigned char> bytes = get_byte_array(padded);

        string output_path = input_path.substr(0, input_path.find_last_of('.')) + ".bin";
        ofstream outfile(output_path, ios::binary);
        for (unsigned char b : bytes) {
            outfile.put(b);
        }
        outfile.close();

        cout << "Compressed\n";
    }

    void decompress(const string& input_path, const string& output_path) {
        ifstream infile(input_path, ios::binary);
        if (!infile) {
            cout << "File not found!\n";
            return;
        }

        string bit_string = "", encoded_text;
        char byte;
        while (infile.get(byte)) {
            bitset<8> bits((unsigned char)byte);
            bit_string += bits.to_string();
        }
        infile.close();

        encoded_text = remove_padding(bit_string);
        string decoded = decode_text(encoded_text);

        ofstream outfile(output_path);
        outfile << decoded;
        outfile.close();

        cout << "Decompressed\n";
    }
};

int main() {
    HuffmanCoding hc;
    hc.compress("input.txt"); // Compress input.txt to input.bin
    hc.decompress("input.bin", "output.txt"); // Decompress input.bin to output.txt
    return 0;
}
