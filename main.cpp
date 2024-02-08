
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <map>
#include <sstream>

using namespace std;

struct DocumentItem {
    string documentName;
    int count;

    DocumentItem(const string& name, int c) : documentName(name), count(c) {}
};

struct WordItem {
    string word;
    map<string, int> documents; // Map of document names to counts
};


// Our AVLsearchTree class
template <class Key, class Value>
class AVLSearchTree {
private:
    struct Node {
        Key key;
        Value value;
        Node *left, *right;
        int height;

        Node(Key k, Value v) : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    }; // Main struct

    Node* root;

    // Utility functions
    int height(Node *N) const {
        if (N == nullptr) return 0;
        return N->height;
    }

    int getBalanceFactor(Node *N) const {
        if (N == nullptr) return 0;
        return height(N->left) - height(N->right);
    }
    Node* minValueNode(Node* node);
    Node* deleteNode(Node* root, Key key);
    Node* rightRotate(Node *y);
    Node* leftRotate(Node *x);
    Node* insert(Node* node, Key key, Value value);
    Node* search(Node* node, Key key) const;
    void destroyTree(Node* node);

    // Tree traversal helpers
    void inOrderTraversal(Node* node) const;
    void preOrderTraversal(Node* node) const;
    void postOrderTraversal(Node* node) const;

public:
    void deleteKey(const Key& key);
    AVLSearchTree() : root(nullptr) {}
    ~AVLSearchTree();

    void insert(Key key, Value value) {
        root = insert(root, key, value);
    }

    Value search(Key key) const {
        Node* result = search(root, key);
        if (result == nullptr) return nullptr;
        return result->value;
    }

    // Public traversal methods
    void inOrder() const {
        inOrderTraversal(root);
    }

    void preOrder() const {
        preOrderTraversal(root);
    }

    void postOrder() const {
        postOrderTraversal(root);
    }

};

// Returns the minimum value node, in this case the left most node
template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::minValueNode(Node* node) {
    Node* current = node;
    while (current->left != nullptr)
        current = current->left;
    return current;
}

// Helper function to delete a node from our tree
template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::deleteNode(Node* root, Key key) {
    // Step 1: Perform standard BST delete
    if (root == nullptr)
        return root;

    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if (key < root->key)
        root->left = deleteNode(root->left, key);

    // If the key to be deleted is greater than the root's key,
    // then it lies in right subtree
    else if (key > root->key)
        root->right = deleteNode(root->right, key);

    // If key is same as root's key, then this is the node to be deleted
    else {
        // Node with only one child or no child
        if ((root->left == nullptr) || (root->right == nullptr)) {
            Node* temp = root->left ? root->left : root->right;

            // No child case
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else // One child case
                *root = *temp; // Copy the contents of the non-empty child
            
            delete temp;
        } else {
            // Node with two children: Get the inorder successor (smallest in the right subtree)
            Node* temp = minValueNode(root->right);

            // Copy the inorder successor's data to this node
            root->key = temp->key;
            root->value = temp->value;

            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // If the tree had only one node then return
    if (root == nullptr)
        return root;

    // Step 2: Update the height of the current node
    root->height = 1 + max(height(root->left), height(root->right));

    // Step 3: Get the balance factor of this node
    int balance = getBalanceFactor(root);

    // Balance the tree
    // Left Left Case
    if (balance > 1 && getBalanceFactor(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalanceFactor(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Main function to delete a node from our tree by using the above helper function
template <class Key, class Value>
void AVLSearchTree<Key, Value>::deleteKey(const Key& key) {
    root = deleteNode(root, key);
}



// Rotations and balancing
template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Main function to insert a node into the tree
template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::insert(Node* node, Key key, Value value) {
    if (node == nullptr) {
        return new Node(key, value);
    }

    if (key < node->key) {
        node->left = insert(node->left, key, value);
    } else if (key > node->key) {
        node->right = insert(node->right, key, value);
    } else {
        // Duplicate keys not allowed, or update the existing node's value
        return node;
    }

    // Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // Balance the node
    int balance = getBalanceFactor(node);

    // Left Left Case
    if (balance > 1 && key < node->left->key) {
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && key > node->right->key) {
        return leftRotate(node);
    }

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// To traverse the tree and look for a certain node in the tree
template <class Key, class Value>
typename AVLSearchTree<Key, Value>::Node* AVLSearchTree<Key, Value>::search(Node* node, Key key) const {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return search(node->left, key);
    } else {
        return search(node->right, key);
    }
}

// Destructor and helper
template <class Key, class Value>
AVLSearchTree<Key, Value>::~AVLSearchTree() {
    destroyTree(root);
}

template <class Key, class Value>
void AVLSearchTree<Key, Value>::destroyTree(Node* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Tree traversals
template <class Key, class Value>
void AVLSearchTree<Key, Value>::inOrderTraversal(Node* node) const {
    if (node != nullptr) {
        inOrderTraversal(node->left);
        cout << node->key << " ";
        inOrderTraversal(node->right);
    }
}

template <class Key, class Value>
void AVLSearchTree<Key, Value>::preOrderTraversal(Node* node) const {
    if (node != nullptr) {
        cout << node->key << " ";
        preOrderTraversal(node->left);
        preOrderTraversal(node->right);
    }
}

template <class Key, class Value>
void AVLSearchTree<Key, Value>::postOrderTraversal(Node* node) const {
    if (node != nullptr) {
        postOrderTraversal(node->left);
        postOrderTraversal(node->right);
        cout << node->key << " ";
    }
}


// A standard function to convert our string to lowercase
string toLowerCase(const string& str) {
    string lowerCaseStr = str;
    transform(lowerCaseStr.begin(), lowerCaseStr.end(), lowerCaseStr.begin(), ::tolower);
    return lowerCaseStr;
}

// To filter our query from all unnecessary punctuation and numbers
string filterAlpha(const string& str) {
    string filteredStr;
    for (char ch : str) {
        if (isalpha(ch)) {
            filteredStr += ch;
        }
    }
    return filteredStr;
}


void updateDocumentCount(WordItem* wordItem, const string& documentName) {
    auto& documents = wordItem->documents;
    documents[documentName]++;
}

// Function to read our file and insert the words one by one to our tree in a correct order by using other helper functions
void preprocessDocument(const string& documentName, AVLSearchTree<string, WordItem*>& myTree) {
    
    ifstream file(documentName);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << documentName << endl;
        return;
    }

    string word;
    while (file >> word) {
        word = toLowerCase(word);
        word = filterAlpha(word);

        if (!word.empty()) {
            WordItem* wordItem = myTree.search(word);
            if (wordItem == nullptr) {
                // Create a new WordItem if it doesn't exist
                wordItem = new WordItem;
                wordItem->word = word;
                myTree.insert(word, wordItem);
            }
            // Update the document count in the WordItem
            updateDocumentCount(wordItem, documentName);
        }
    }
    file.close();
}

// Function to display the output results
void displayResults(const map<string, map<string, int>>& results) {
    for (const auto& docPair : results) {
        cout << "in Document " << docPair.first << ", ";
        bool firstWord = true;
        for (const auto& wordCountPair : docPair.second) {
            if (!firstWord) {
                cout << ", ";
            }
            cout << wordCountPair.first << " found " << wordCountPair.second << " times";
            firstWord = false;
        }
        cout << "." << endl;
    }

    // If no documents contain the query
    if (results.empty()) {
        cout << "No document contains the given query" << endl;
    }
}

// Function to process the query and call the display results function
void processQuery(const string& query, const AVLSearchTree<string, WordItem*>& myTree) {
    istringstream iss(query);
    vector<string> words((istream_iterator<string>(iss)), istream_iterator<string>());

    map<string, map<string, int>> allResults; // Map of document names to word counts for all words in the query

    for (const string& word : words) {
        string lowerWord = toLowerCase(word);
        WordItem* wordItem = myTree.search(lowerWord);

        if (wordItem != nullptr) {
            for (const auto& docPair : wordItem->documents) {
                allResults[docPair.first][lowerWord] = docPair.second;
            }
        }
    }

    displayResults(allResults);
}

void handleRemoveQuery(const std::string& query, AVLSearchTree<std::string, WordItem*>& myTree) {
    istringstream iss(query);
    string command, wordToRemove;

    // Extract the first word from the query
    iss >> command;
    command = toLowerCase(command);

    // Check if the command is "remove"
    if (command == "remove") {
        // Extract the word to remove
        if (iss >> wordToRemove) {
            // Convert the word to lowercase before deleting
            wordToRemove = toLowerCase(wordToRemove);

            // Check if the word exists and remove it
            if (myTree.search(wordToRemove) != nullptr) {
                myTree.deleteKey(wordToRemove);
                cout << wordToRemove << " has been REMOVED." << endl;
            }
        }
    }
}

// Main code starts here...
int main() {
    AVLSearchTree<string, WordItem*> myTree; // our AVLSearchTree is created
    
        int numOfFiles;
        cout << "Enter number of input files: ";
        cin >> numOfFiles;
    
        cin.ignore(); // To ignore the newline after reading numOfFiles
    
        vector<string> fileNames(numOfFiles);
        for (int i = 0; i < numOfFiles; ++i) {
            cout << "Enter " << (i + 1) << ". file name: ";
            getline(cin, fileNames[i]);
            preprocessDocument(fileNames[i], myTree);
        }
    
        string query;
        cout << "Enter queried words in one line: ";
        getline(cin, query);
        query = toLowerCase(query);
    
    while (query.find("endofinput") == -1){
        if (query.find("remove") != -1){
            handleRemoveQuery(query, myTree); // removes the word if 'remove' is found in our query
        } else {
            processQuery(query, myTree); } // otherwise processes and displays the results
        cout << "Enter queried words in one line: ";
        getline(cin, query);
        query = toLowerCase(query);
    }
        return 0;
}


