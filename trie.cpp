#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false;
    }
};

class Trie {
private:
    TrieNode* root;

    bool hasChildren(TrieNode* node) {
        for (auto& it : node->children) {
            if (it.second != nullptr) {
                return true;
            }
        }
        return false;
    }

    bool removeHelper(TrieNode* node, string& key, int level, int len) {
    if (node == nullptr) {
        return false;
    }

    if (level == len) {
        if (node->isEndOfWord) {
            node->isEndOfWord = false;
            return !hasChildren(node);
        }
        return false;
    }

    char ch = key[level];
    bool dno = false;
    if (removeHelper(node->children[ch], key, level + 1, len)) {
        node->children.erase(ch);
        dno = !hasChildren(node);
    }
    return dno;
}

    void dfsTraversal(TrieNode* node, string prefix) {
        if (node == nullptr)
            return;

        if (node->isEndOfWord) {
            cout << prefix << endl;
        }

        for (auto& it : node->children) {
            if (it.second != nullptr) {
                dfsTraversal(it.second, prefix + it.first);
            }
        }
    }


public:
    Trie() {
        root = new TrieNode();
    }

    void insert(string word) {
        TrieNode* current = root;
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }
        current->isEndOfWord = true;
    }

    bool search(string word) {
        TrieNode* current = root;
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }
        return current != nullptr && current->isEndOfWord;
    }

    bool startsWith(string prefix) {
        TrieNode* current = root;
        for (char c : prefix) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c];
        }
        return current != nullptr;
    }

    bool endsWith(string word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (node->children.find(ch) == node->children.end()) {
                return false;
            }
            node = node->children[ch];
        }
        return node->isEndOfWord;
    }

    bool empty() {
        return !hasChildren(root);
    }

    void remove(string key) {
    int len = key.length();
    if (len > 0) {
        if (removeHelper(root, key, 0, len) && !hasChildren(root)) {
            delete root;
            root = new TrieNode();
        }
    }
}

    void traverse() {
        dfsTraversal(root, "");
    }

};

int main() {
    Trie trie;

    trie.insert("juli");
    cout << (trie.search("juli") ? "Sí" : "No") << endl;
    trie.remove("juli");
    cout << (trie.search("juli") ? "Sí" : "No") << endl;

    cout << (trie.empty() ? "Trie vacío" : "Trie no vacío") << endl;

    trie.insert("juli");
    cout << (trie.empty() ? "Trie vacío" : "Trie no vacío") << endl;

    trie.insert("uchu");
    trie.insert("ari");
    trie.insert("carlos");
    trie.insert("jimena");

    cout << "Palabras en el trie:" << endl;
    trie.traverse();

    Trie trie2;

    trie2.insert("hello");
    trie2.insert("world");
    trie2.insert("hell");
    trie2.insert("helloworld");
    trie2.insert("hellow");

    cout << "startsWith 'hell': " << (trie2.startsWith("hell") ? "Sí" : "No") << endl;
    cout << "startsWith 'world': " << (trie2.startsWith("world") ? "Sí" : "No") << endl;
    cout << "startsWith 'low': " << (trie2.startsWith("low") ? "Sí" : "No") << endl;


    cout << "endsWith 'low': " << (trie2.endsWith("low") ? "Sí" : "No") << endl;
    cout << "endsWith 'world': " << (trie2.endsWith("world") ? "Sí" : "No") << endl;
    cout << "endsWith 'hellow': " << (trie2.endsWith("hellow") ? "Sí" : "No") << endl;



    return 0;
}

