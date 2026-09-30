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
    public:
        string index;
        string value;

        ArgValue(){
            this->index = "";
            this->value = "";
        }

        ArgValue(string index, string value){
            this->index = index;
            this->value = value;
        }

        void printOut(){
            cout << "Value: " << value << "  Index: " << index << endl;
        }
};

// Function to add a child to a node
void addChild(Node* parent, Node* child) {
    parent->children.push_back(child);
    child->parent = parent;
    /*cout << parent->data << " : ";
    cout << child->data;*/
}

bool wildcardMatching(string txt, string pat){
    int n = txt.size();
    int m = pat.size();
    int i = 0, j = 0, startIndex = -1, match = 0;
    while (i < n) {
        if (j < m && (pat[j] == '?' || pat[j] == txt[i])) {          
            i++;
            j++;
        }
        
        else if (j < m && pat[j] == '*') {
            startIndex = j;
            match = i;
            j++;
        }
      
        else if (startIndex != -1) {
            j = startIndex + 1;
            match++;
            i = match;
        }
        
        else {
            return false;
        }
    }
    while (j < m && pat[j] == '*') {
        j++;
    }
    return j == m;
}

void readDirectory(fs::path workingPath, string filter, bool IgnoreHidden){
    Node* root = new Node(workingPath.string());
    Node* currentDir = root;
    Node* prevDir = nullptr;

    int totalFiles = 0;
    int prevDepth = 0;
    for(auto iterEntry = fs::recursive_directory_iterator(workingPath); iterEntry != fs::recursive_directory_iterator(); ++iterEntry ) {
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
        if (!wildcardMatching(filenameStr, filter) && !isDir){
            continue;
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
        if (!isDir)
            totalFiles++;
    }
    cout << "Total Files to Be Affected: " << totalFiles << endl;
}

ArgValue* parseValueFromArg(string argument){
    string index = argument.substr(0,5);
    string value = argument.substr(6,argument.size());
    return new ArgValue(index, value);
}

string findAndReplace(string prime, string filter, string replacement){
    int index = prime.find(filter);
    int size = filter.size();

    prime.replace(index, size, replacement);
    return prime;
}

string formatCommandString(string prime, vector<ArgValue*> nameFilters, vector<ArgValue*> pathFilters){
    string out = prime;
    for (ArgValue* temp : pathFilters){
            temp->printOut();
        }
    for (ArgValue* temp : nameFilters){
        temp->printOut();
    }
    cout << "\n\n";


    string nameFormat;

    //this is sloppy but for somereason I can't get vector<>.insert() to work so I can't count on the vector to be sorted;
    for (ArgValue* name : nameFilters){
                 
    }
    for (ArgValue* name : nameFilters){
        //can't get vector<>.erase() to work either so if name is NAME0 ignore it
        if (name->index.compare("NAME0") == 0)
            continue;
        //format out
    }
    for (ArgValue* name : nameFilters){
        //can't get vector<>.erase() to work either so if name is NAME0 ignore it
        if (name->index.compare("NAME0") == 0)
            continue;
        cout << "here: ";
        name->printOut();
        out = findAndReplace(out, name->index, name->value);
    }
    
    return out;
}

void StringSplitter(string& before, string& after, string original, string wildcard, bool include){
        if (original.find(wildcard) != string::npos){
            int index = original.find(wildcard);
            before = original.substr(0, index);
            if (include)
                after = original.substr(index, original.size());
            else
                after = original.substr(index + 1, original.size());
        }
    }

string wildcardHandling(string alpha, string beta){
    int wildAlpha, dotBeta;
    string beforeWildAlpha = "";
    string afterWildAlpha = "";
    string betaName = beta;
    string betaExtention;
    
    StringSplitter(beforeWildAlpha, afterWildAlpha, alpha, "*", false);
    StringSplitter(betaName, betaExtention, beta, ".", true);

    string out = beforeWildAlpha + betaName + afterWildAlpha +  betaExtention;
    return out;
}






int main(int argc, char* argv[]) {
    try {
        vector<char> options;
        vector<ArgValue*> nameFilters;
        vector<ArgValue*> pathFilters;
        string filter = "*";

        //Sort Arguments
        /*Display all args for testing purposes for (int i = 0; i < argc; ++i){
            string arg(argv[i]);
            cout << argv[i] << endl;
        }
        cout << "\n\n";*/


        int iterator = 1;
        string arg(argv[iterator]);
        //read options. If options move iterator otherwise first arg is command string
        if (arg.at(0) == '-'){
            //read options
            iterator++;
        }

        string commandString(argv[iterator]); iterator++;

        //check to see if path specifiyed
        arg = argv[iterator];
        arg = arg.substr(0,4);
        fs::path workingPath = fs::current_path();

        //if the next argument isn't a PATHx or NAMEx argument it must be the directory
        //if not the workingPath is the current path
        if (!(arg.compare("PATH") == 0) && !(arg.compare("NAME") == 0)){
            arg = argv[iterator];
            workingPath = arg; iterator++;
        }

        //Sort Args between names and paths
        for (int i = iterator; i < argc; ++i){
            string arg(argv[i]);
            if (arg.substr(0,4).compare("PATH") == 0){
                ArgValue* path = parseValueFromArg(arg);
                int i = stoi(path->index.substr(4, path->index.size()));
                pathFilters.push_back(path);
            }
            if (arg.substr(0,4).compare("NAME") == 0){
                ArgValue* name = parseValueFromArg(arg);
                if (name->index.compare("NAME0") == 0){
                    filter = name->value;
                    continue; 
                }  
                int i = stoi(name->index.substr(4, name->index.size()));
                nameFilters.push_back(name);
                //nameFilters.insert(i, name);
            }
        }

        /*for (ArgValue* temp : pathFilters){
            temp->printOut();
        }
        for (ArgValue* temp : nameFilters){
            temp->printOut();
        }*/

        //cout << formatCommandString(commandString, nameFilters, pathFilters) << endl;
        //const fs::path workingPath{ argc >= 2 ? argv[1] : fs::current_path() };
        readDirectory(workingPath, filter, true);
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



//Remember the replace() function for strings