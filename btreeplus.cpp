#include <iostream>
using namespace std;

// Clase BPlusTreeNode
class BPlusTreeNode {
    int* keys;
    int t;
    BPlusTreeNode** children;
    int n;
    bool leaf;

    BPlusTreeNode* parent;

public:
    BPlusTreeNode(int _t, bool _leaf) : t(_t), leaf(_leaf), n(0), parent(nullptr) {
        keys = new int[2 * t - 1];
        children = new BPlusTreeNode*[2 * t];
    }

    ~BPlusTreeNode() {
        delete[] keys;
        delete[] children;
    }

    int getMinimumDegree() const {
        return t;
    }

    bool isLeaf() const {
        return leaf;
    }

    int getKey(int index) const {
        return keys[index];
    }

    void setKey(int index, int key) {
        keys[index] = key;
    }

    BPlusTreeNode* getChild(int index) const {
        return children[index];
    }

    void setChild(int index, BPlusTreeNode* child) {
        children[index] = child;
        if (child)
            child->parent = this;
    }

    BPlusTreeNode* getParent() const {
        return parent;
    }

    void setParent(BPlusTreeNode* p) {
        parent = p;
    }

    int getKeyCount() const {
        return n;
    }

    void setKeyCount(int count) {
        n = count;
    }

    void incrementKeyCount() {
        n++;
    }

    void decrementKeyCount() {
        n--;
    }

    void insertKey(int key) {
        int i = n - 1;
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = key;
        n++;
    }

    void removeKey(int keyIndex) {
        for (int i = keyIndex; i < n - 1; ++i) {
            keys[i] = keys[i + 1];
        }
        n--;
    }

    void insertChild(int index, BPlusTreeNode* child) {
        for (int i = n; i > index; --i) {
            children[i + 1] = children[i];
        }
        children[index + 1] = child;
        if (child)
            child->parent = this;
        n++;
    }

    void removeChild(int index) {
        for (int i = index; i < n - 1; ++i) {
            children[i] = children[i + 1];
        }
        n--;
    }

    int findKey(int key) const {
        int index = 0;
        while (index < n && keys[index] < key) {
            ++index;
        }
        return index;
    }

    void insertNonFull(int key);
    void splitChild(int i, BPlusTreeNode* y);
    void traverse();
    BPlusTreeNode* search(int key);
    void remove(int key);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    int getPred(int idx);
    int getSucc(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
};

void BPlusTreeNode::insertNonFull(int key) {
    int i = n - 1;

    if (leaf) {
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = key;
        n++;
    } else {
        while (i >= 0 && keys[i] > key)
            i--;

        if (children[i + 1]->n == 2 * t - 1) {
            splitChild(i + 1, children[i + 1]);

            if (keys[i + 1] < key)
                i++;
        }
        children[i + 1]->insertNonFull(key);
    }
}

void BPlusTreeNode::splitChild(int i, BPlusTreeNode* y) {
    BPlusTreeNode* z = new BPlusTreeNode(y->t, y->leaf);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->children[j] = y->children[j + t];
    }

    y->n = t - 1;

    for (int j = n; j >= i + 1; j--)
        children[j + 1] = children[j];

    children[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[t - 1];

    n++;
}

void BPlusTreeNode::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (!leaf)
            children[i]->traverse();
        cout << " " << keys[i];
    }

    if (!leaf)
        children[i]->traverse();
}

BPlusTreeNode* BPlusTreeNode::search(int key) {
    int i = 0;
    while (i < n && key > keys[i])
        i++;

    if (keys[i] == key)
        return this;

    if (leaf)
        return nullptr;

    return children[i]->search(key);
}

void BPlusTreeNode::remove(int key) {
    int idx = findKey(key);

    if (idx < n && keys[idx] == key) {
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    } else {
        if (leaf) {
            cout << "The key " << key << " is does not exist in the tree\n";
            return;
        }

        bool flag = (idx == n);

        if (children[idx]->n < t)
            fill(idx);

        if (flag && idx > n)
            children[idx - 1]->remove(key);
        else
            children[idx]->remove(key);
    }
}

void BPlusTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];
    n--;
}

void BPlusTreeNode::removeFromNonLeaf(int idx) {
    int key = keys[idx];

    if (children[idx]->n >= t) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    } else if (children[idx + 1]->n >= t) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    } else {
        merge(idx);
        children[idx]->remove(key);
    }
}

int BPlusTreeNode::getPred(int idx) {
    BPlusTreeNode* cur = children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->n];

    return cur->keys[cur->n - 1];
}

int BPlusTreeNode::getSucc(int idx) {
    BPlusTreeNode* cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];

    return cur->keys[0];
}

void BPlusTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->n >= t)
        borrowFromPrev(idx);
    else if (idx != n && children[idx + 1]->n >= t)
        borrowFromNext(idx);
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BPlusTreeNode::borrowFromPrev(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx - 1];

    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = keys[idx - 1];

    if (!child->leaf)
        child->children[0] = sibling->children[sibling->n];

    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

void BPlusTreeNode::borrowFromNext(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx + 1];

    child->keys[(child->n)] = keys[idx];

    if (!child->leaf)
        child->children[(child->n) + 1] = sibling->children[0];

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->n += 1;
    sibling->n -= 1;
}

void BPlusTreeNode::merge(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx + 1];

    child->keys[t - 1] = keys[idx];

    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->children[i + t] = sibling->children[i];
    }

    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    for (int i = idx + 2; i <= n; ++i)
        children[i - 1] = children[i];

    child->n += sibling->n + 1;
    n--;

    delete sibling;
}

// Clase BTreePlus
class BTreePlus {
    BPlusTreeNode* root;
    int t;

public:
    BTreePlus(int _t) : root(nullptr), t(_t) {}

    ~BTreePlus() {

    }

    void insert(int key) {
        if (!root) {
            root = new BPlusTreeNode(t, true);
            root->insertKey(key);
            return;
        }

        if (root->getKeyCount() == 2 * t - 1) {
            BPlusTreeNode* newRoot = new BPlusTreeNode(t, false);
            newRoot->setChild(0, root);
            newRoot->splitChild(0, root);
            int i = 0;
            if (newRoot->getKey(0) < key) {
                ++i;
            }
            newRoot->getChild(i)->insertNonFull(key);
            root = newRoot;
        } else {
            root->insertNonFull(key);
        }
    }

    void remove(int key) {
        if (!root) {
            cout << "The tree is empty\n";
            return;
        }

        root->remove(key);

        if (root->getKeyCount() == 0) {
            BPlusTreeNode* tmp = root;
            if (root->isLeaf()) {
                root = nullptr;
            } else {
                root = root->getChild(0);
            }
            delete tmp;
        }
    }

    void traverse() {
        if (root != nullptr) {
            root->traverse();
        }
    }

    BPlusTreeNode* search(int key) {
        return (root == nullptr) ? nullptr : root->search(key);
    }
};


int main() {
    // Crear un árbol B+ con un grado mínimo de 3
    BTreePlus bTreePlus(3);

    // Insertar claves
    bTreePlus.insert(10);
    bTreePlus.insert(20);
    bTreePlus.insert(5);
    bTreePlus.insert(6);
    bTreePlus.insert(12);
    bTreePlus.insert(30);
    bTreePlus.insert(7);
    bTreePlus.insert(17);

    cout << "Recorrido árbol B+ :\n";
    bTreePlus.traverse();
    cout << endl;

    // Buscar una clave
    int key = 6;
    (bTreePlus.search(key) != nullptr) ? cout << "Clave " << key << " está.\n" : cout << "Clave " << key << " nno está.\n";

    // Eliminar una clave
    key = 6;
    bTreePlus.remove(key);
    cout << "Recorrido después de eliminación " << key << ":\n";
    bTreePlus.traverse();
    cout << endl;

    return 0;
}
