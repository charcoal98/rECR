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
    Node* parent;
    bool directoryflag;
    vector<Node*> children;

    Node(string path) {
        data = path;
        this->parent = nullptr;
        directoryflag = false;
    }

    Node(string path, bool isDir) {
        data = path;
        this->parent = nullptr;
        directoryflag = isDir;
    }
};

class ArgValue {
    string index;
    string value;

    ArgValue(string index, string name){
        this->index = index;
        this->value = value;
    }
};

// Function to add a child to a node
void addChild(Node* parent, Node* child) {
    parent->children.push_back(child);
    child->parent = parent;
    /*cout << parent->data << " : ";
    cout << child->data;*/
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

void readDirectory(fs::path filepathT, string Filter, bool IgnoreHidden){
    Node* root = new Node(filepathT.string());
    Node* currentDir = root;
    Node* prevDir = nullptr;
    int totalFiles = 0;
    int prevDepth = 0;
    for(auto iterEntry = fs::recursive_directory_iterator(filepathT); iterEntry != fs::recursive_directory_iterator(); ++iterEntry ) {
        const string filenameStr = iterEntry->path().filename().string();
        //skip if dot file
        if(IgnoreHidden){
            if(filenameStr.at(0) == '.'){
                continue;
            }
        }

        Node* file;
        bool isDir = false;
        //check if directory for tree
        if (iterEntry->is_directory()) {
            isDir = true;
        }

        //build tree
        if (prevDepth > iterEntry.depth()){//Going up
            file = new Node(filenameStr, isDir);
            currentDir = currentDir->parent;
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

        //cout << "(file: " << file->data << "),(prevDir: "<<prevDir->data<<"),(currentDir: "<<currentDir->data<<")";

        cout << setw(iterEntry.depth()*3) << "";
        if (iterEntry->is_directory()) {
            cout << "dir:  " << filenameStr;
        }
        else if (iterEntry->is_regular_file()) {
            cout << "file: " << filenameStr;
        }
        else
            cout << "??    " << filenameStr;
        cout << endl;
        prevDepth = iterEntry.depth();
        totalFiles++;
    }
    cout << "Total Files to Be Affected: " << totalFiles << endl;
}

int main(int argc, char* argv[]) {
    try {
        vector<char> options;
        vector<ArgValue> nameFilters;
        vector<ArgValue> pathFilters;

        //Sort Arguments
        for (int i = 0; i < argc; ++i){
            string arg(argv[i]);
            cout << argv[i] << endl;
        }
        cout << "\n\n";


        int iterator = 1;
        string arg(argv[iterator]);
        //read options. If options move iterator otherwise first arg is command string
        if (arg.at(0) == '-'){
            //read options
            iterator++;
        }

        //Sort Args between names and paths
        string commandString(argv[iterator]); iterator++;
        for (int i = iterator; i < argc; ++i){
            string arg(argv[i]);
            if (arg.substr(0,4).compare("PATH") == 0){
                cout << argv[i] << " < Path\n"; ///START HERE
            }
            if (arg.substr(0,4).compare("NAME") == 0){
                cout << argv[i] << " < Name\n";
            }
        }
        /*const fs::path filepathT{ argc >= 2 ? argv[1] : fs::current_path() };
        readDirectory(filepathT, "",true);*/
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