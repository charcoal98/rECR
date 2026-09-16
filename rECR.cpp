#include <chrono>
#include <filesystem>  
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;
namespace fs = filesystem;

class Node {
public:
    string data;
    Node* par;
    bool directoryflag;
    vector<Node*> children;

    Node(string path) {
        data = path;
        par = nullptr;
        directoryflag = false;
    }

    Node(string path, bool isDir) {
        data = path;
        par = nullptr;
        directoryflag = isDir;
    }
};

// Function to add a child to a node
void addChild(Node* parent, Node* child) {
    parent->children.push_back(child);
    child->par = parent;
    cout << parent->data << " : ";
    cout << child->data;
}

void printParents(Node* node, Node* parent) {
    if (parent == nullptr)
        cout << node->data << " -> NULL" << endl;
    else
        cout << node->data << " -> " << parent->data << endl;

    for (auto child : node->children)
        printParents(child, node);
}

// Function to print children of each node
void printChildren(Node* node) {
    cout << node->data << " -> ";
    for (auto child : node->children)
        cout << child->data << " ";
    cout << endl;

    for (auto child : node->children)
        printChildren(child);
}

// Function to print leaf nodes
void printLeafNodes(Node* node) {
    if (node->children.empty()) {
        cout << node->data << " ";
        return;
    }
    for (auto child : node->children)
        printLeafNodes(child);
}

// Function to print degrees of each node 
void printDegrees(Node* node, Node* parent) {
    int degree = node->children.size();
    if (parent != nullptr)  
        degree++;
    cout << node->data << " -> " << degree << endl;

    for (auto child : node->children)
        printDegrees(child, node);
}

int main(int argc, char* argv[]) {
    try {
        const fs::path filepathT{ argc >= 2 ? argv[1] : fs::current_path() };
        Node* root = new Node(filepathT.string());
        Node* currentDir = root;
        Node* prevDir = nullptr;
        int prevDepth = 0;

        for(auto iterEntry = fs::recursive_directory_iterator(filepathT); iterEntry != fs::recursive_directory_iterator(); ++iterEntry ) {
            const string filenameStr = iterEntry->path().filename().string();
            Node* file;
            bool isDir = false;
            if (iterEntry->is_directory()) {
                isDir = true;
            }
            if (prevDepth > iterEntry.depth()){//Going up
                file = new Node(filenameStr, isDir);
                currentDir = currentDir->par;
                addChild(currentDir, file);
            } else if (prevDepth < iterEntry.depth()){//Going down
                file = new Node(filenameStr, isDir);
                addChild(prevDir, file);
                currentDir = prevDir;
            }else{//pure horizontal
                file = new Node(filenameStr, isDir);
                addChild(currentDir, file);
            }
            prevDir = file;
            cout << "(file: " << file->data << "),(prevDir: "<<prevDir->data<<"),(currentDir: "<<currentDir->data<<")";
            /*cout << setw(iterEntry.depth()*3) << "";
            if (iterEntry->is_directory()) {
                cout << "dir:  " << filenameStr;
            }
            else if (iterEntry->is_regular_file()) {
                cout << "file: " << filenameStr;
            }
            else
                cout << "??    " << filenameStr;*/
            cout << endl;
            prevDepth = iterEntry.depth();
        }

        /*cout << "Parents of each node:" << endl;
        printParents(root, nullptr);

        cout << "Children of each node:" << endl;
        printChildren(root);

        cout << "Leaf nodes: ";
        printLeafNodes(root);
        cout << endl;

        cout << "Degrees of nodes:" << endl;
        printDegrees(root, nullptr);*/
    }
    catch (const fs::filesystem_error& err) {
        cerr << "filesystem error! " << err.what() << endl;
        if (!err.path1().empty())
            cerr << "path1: " << err.path1().string() << endl;
        if (!err.path2().empty())
            cerr << "path2: " << err.path2().string() << endl;
    }
    catch (const exception& ex) {
        cerr << "general exception: " << ex.what() << endl;
    }
}