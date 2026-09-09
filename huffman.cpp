#include <bits/stdc++.h>
using namespace std;

/* ---------- Node ---------- */

struct Node {
    char ch;
    int freq;
    int id;

    Node *left, *right;

    Node(char c, int f, int i) {
        ch = c;
        freq = f;
        id = i;
        left = right = NULL;
    }
};


/* ---------- Compare ---------- */

struct Compare {
    bool operator()(Node* a, Node* b) {

        // Smaller frequency gets higher priority
        if (a->freq != b->freq)
            return a->freq > b->freq;

        // If frequencies are same,
        // smaller id gets higher priority
        return a->id > b->id;
    }
};


map<char, string> codes;
Node* root = NULL;

int nodeId = 0;


/* ---------- Build Codes ---------- */

void buildCodes(Node* node, string str) {

    if (!node)
        return;

    // Leaf node
    if (!node->left && !node->right) {

        // Single character case
        if (str == "")
            str = "0";

        codes[node->ch] = str;

        return;
    }

    buildCodes(node->left, str + "0");
    buildCodes(node->right, str + "1");
}


/* ---------- Build Tree ---------- */

Node* buildTree(map<char, int>& freq) {

    priority_queue<Node*, vector<Node*>, Compare> pq;

    nodeId = 0;

    // Create leaf nodes
    for (auto p : freq) {

        pq.push(
            new Node(
                p.first,
                p.second,
                nodeId++
            )
        );
    }

    // Empty file
    if (pq.empty())
        return NULL;

    // Build Huffman tree
    while (pq.size() > 1) {

        Node* left = pq.top();
        pq.pop();

        Node* right = pq.top();
        pq.pop();

        Node* parent = new Node(
            '\0',
            left->freq + right->freq,
            nodeId++
        );

        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}


/* ---------- Compress ---------- */

void compressFile(string input, string output) {

    // Binary mode is important on Windows
    ifstream in(input, ios::binary);
    ofstream out(output, ios::binary);

    if (!in || !out) {
        cout << "File error\n";
        return;
    }


    /* ---------- Read input ---------- */

    string text;
    char ch;

    while (in.get(ch))
        text += ch;

    in.close();


    /* ---------- Empty file ---------- */

    if (text.empty()) {

        out << 0 << '\n';
        out << 0 << '\n';

        out.close();

        return;
    }


    /* ---------- Frequency ---------- */

    map<char, int> freq;

    for (char c : text)
        freq[c]++;


    /* ---------- Build Tree ---------- */

    root = buildTree(freq);


    /* ---------- Build Codes ---------- */

    codes.clear();

    buildCodes(root, "");


    /* ---------- Save Frequency Table ---------- */

    out << freq.size() << '\n';

    for (auto p : freq) {

        // Store ASCII value instead of character
        // This safely handles space, newline, tab etc.
        out << (int)(unsigned char)p.first
            << " "
            << p.second
            << '\n';
    }


    /* ---------- Encode ---------- */

    string encoded;

    for (char c : text)
        encoded += codes[c];


    /* ---------- Save Bit Length ---------- */

    out << encoded.size() << '\n';


    /* ---------- Pack Bits ---------- */

    int val = 0;
    int bits = 0;

    for (char c : encoded) {

        val = (val << 1) + (c - '0');

        bits++;

        if (bits == 8) {

            out.put((char)val);

            val = 0;
            bits = 0;
        }
    }


    // Remaining bits
    if (bits > 0) {

        val = val << (8 - bits);

        out.put((char)val);
    }

    out.close();
}


/* ---------- Decompress ---------- */

void decompressFile(string input, string output) {

    ifstream in(input, ios::binary);
    ofstream out(output, ios::binary);

    if (!in || !out) {
        cout << "File error\n";
        return;
    }


    /* ---------- Read Frequency Table ---------- */

    int n;

    in >> n;


    /* ---------- Empty File ---------- */

    if (n == 0) {

        out.close();
        in.close();

        return;
    }


    map<char, int> freq;

    for (int i = 0; i < n; i++) {

        int ascii;
        int f;

        in >> ascii >> f;

        char c = (char)ascii;

        freq[c] = f;
    }


    /* ---------- Rebuild Tree ---------- */

    root = buildTree(freq);


    /* ---------- Read Bit Length ---------- */

    int len;

    in >> len;

    // Move past the newline after bit length
    in.ignore(numeric_limits<streamsize>::max(), '\n');


    /* ---------- Single Character Case ---------- */

    if (!root->left && !root->right) {

        for (int i = 0; i < freq[root->ch]; i++)
            out.put(root->ch);

        in.close();
        out.close();

        return;
    }


    /* ---------- Read Binary Data ---------- */

    string bits;

    char byte;

    while (in.get(byte)) {

        bitset<8> b(
            (unsigned char)byte
        );

        bits += b.to_string();
    }


    /* ---------- Remove Padding Bits ---------- */

    if (len < (int)bits.size())
        bits = bits.substr(0, len);


    /* ---------- Decode ---------- */

    Node* curr = root;

    for (char c : bits) {

        if (c == '0')
            curr = curr->left;
        else
            curr = curr->right;


        // Reached leaf
        if (!curr->left && !curr->right) {

            out.put(curr->ch);

            curr = root;
        }
    }


    in.close();
    out.close();
}


/* ---------- Main ---------- */

int main(int argc, char* argv[]) {

    if (argc != 4) {

        cout << "Usage:\n";

        cout << "huffman compress input output\n";

        cout << "huffman decompress input output\n";

        return 0;
    }


    string mode = argv[1];

    string input = argv[2];

    string output = argv[3];


    if (mode == "compress") {

        compressFile(input, output);

    }
    else if (mode == "decompress") {

        decompressFile(input, output);

    }
    else {

        cout << "Invalid mode\n";
    }


    return 0;
}