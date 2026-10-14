#include <chrono>
#include <filesystem>  
#include <iostream>
#include <iomanip>
#include <vector>
#include <bits/stdc++.h>
//https://xkcd.com/1513/ I'm sorry this can definatly be cleaned up
using namespace std;
namespace fs = filesystem;

/*class Node {
    public:
        string name;
        string path;
        Node* parent;
        bool directoryflag;
        vector<Node*> children;

        Node(string name, string path) {
            this->name = name;
            this->path = path;
            this->parent = nullptr;
            directoryflag = false;
        }

        Node(string name, string path, bool isDir) {
            this->name = name;
            this->path = path;
            this->parent = nullptr;
            directoryflag = isDir;
        }

        void PrintAll(){
            cout << "parent: "<<parent<<"| isDir: "<<directoryflag<<"| path: "<<path<<"| name: "<<name<<endl;
        }
};

// Function to add a child to a node
void addChild(Node* parent, Node* child) {
    parent->children.push_back(child);
    child->parent = parent;

}//*/

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

bool wildcardMatching(string alpha, string filter){
    int alphaSize = alpha.size();
    int filterSize = filter.size();
    int i = 0, j = 0, startIndex = -1, match = 0;
    while (i < alphaSize) {
        if (j < filterSize && (filter[j] == '?' || filter[j] == alpha[i])) {          
            i++;
            j++;
        } else if (j < filterSize && filter[j] == '*') {
            startIndex = j;
            match = i;
            j++;
        } else if (startIndex != -1) {
            j = startIndex + 1;
            match++;
            i = match;
        } else {
            return false;
        }
    }
    while (j < filterSize && filter[j] == '*') {
        j++;
    }
    return j == filterSize;
}

void readDirectory(fs::path workingPath, string filter, bool IgnoreHidden){
    /*Node* root = new Node("",workingPath.string());
    Node* currentDir = root;
    Node* prevDir = nullptr;//*/

    int totalFiles = 0;
    int totalDir = 0;
    int prevDepth = 0;
    for(auto entry = fs::recursive_directory_iterator(workingPath); entry != fs::recursive_directory_iterator(); ++entry ) {
        const string filename = entry->path().filename().string();
        string filepath = entry->path().string();
        //edit name out of path
        filepath = filepath.substr(0,filepath.size() - filename.size());

        //skip if dot file
        if(IgnoreHidden){
            if(filename.at(0) == '.'){
                continue;
            }
        }

        //Node* file;
        bool isDir = false;
        //check if directory for tree
        if (entry->is_directory()) {
            isDir = true;
            totalDir++;
        }
        if (!wildcardMatching(filename, filter) && !isDir){
            continue;
        }
        //build tree left over from before I just went throught the fs twice, Might revisit does have some efficency benifits
        /*if (prevDepth > entry.depth()){//Going up
            file = new Node(filename, filepath, isDir);
            currentDir = currentDir->parent;
            addChild(currentDir, file);
        } else if (prevDepth < entry.depth()){//Going down
            file = new Node(filename, filepath, isDir);
            addChild(prevDir, file);
            currentDir = prevDir;
        }else{//pure horizontal
            file = new Node(filename, filepath, isDir);
            addChild(currentDir, file);
        }
        prevDir = file;
        file->PrintAll();//*/

        cout << setw(entry.depth()*3) << "";
        if (entry->is_directory()) {
            cout << "dir:  " << filename;
        }
        else if (entry->is_regular_file()) {
            cout << "file: " << filename;
        }
        else
            cout << "??    " << filename;
        cout << endl;
        prevDepth = entry.depth();
        if (!isDir)
            totalFiles++;
    }
    cout << "Total Directories to Be Affected: " << totalDir << endl;
    cout << "Total Files to Be Affected: " << totalFiles << endl;
}



ArgValue* parseValueFromArg(string argument){
    string index = argument.substr(0,5);
    index = "FILE." + index;
    string value = argument.substr(6,argument.size());
    return new ArgValue(index, value);
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

string findAndReplace(string prime, string filter, string replacement){
    int index = prime.find(filter);
    int size = filter.size();
    if (index == string::npos){
        return prime;
    }
    prime.replace(index, size, replacement);

    //testing prints
    /*cout << "prime: " << prime << "| filter: " << filter<<endl;
    cout << "index: " << index << "| size: " << size<<"| replacement: "<<replacement<<endl;//*/
    return prime;
}

string handleSpaces(string alpha){
    string temp = "";
    string out = alpha;
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, " ", "☺");//ascii smilly face doesn't break c++ (SWIFT emoji vars arn't so special) I'm using it since It probably won't appear in the string
    }
    temp = "";
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, "☺", "\\ ");
    }
    return out;
}

string formatCommandString(string prime, vector<ArgValue*> nameFilters, 
    vector<ArgValue*> pathFilters, string filepath, string filename){////HERE
    string out = prime;
    string temp = "";
    //replace all instances of FILE.PATH0 in commandString
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, "FILE.PATH0", filepath);
    }
    
    temp = "";
    //replace all instances of FILE.NAME0 in commandString
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, "FILE.NAME0", filename);
    }
    

    for (ArgValue* path : pathFilters){
        string outCheck = findAndReplace(out, path->index, path->value + filepath);
        if (out == outCheck){
            throw runtime_error(path->index + " is not specified in the command string");
        }
        out = outCheck;
    }
    for (ArgValue* name : nameFilters){
        string outCheck = findAndReplace(out, name->index, wildcardHandling(name->value, filename));
        if (out == outCheck){
            throw runtime_error(name->index + " is not specified in the command string");
        }
        out = outCheck;
    }

    return out;
}

void executeCommand(fs::path workingPath, string filter, bool IgnoreHidden, 
    string command, vector<ArgValue*> nameFilters, vector<ArgValue*> pathFilters, bool runOnDir){
    string commandString;

    for(auto entry = fs::recursive_directory_iterator(workingPath); 
    entry != fs::recursive_directory_iterator(); ++entry ) {
        const string filename = entry->path().filename().string();
        string filepath = entry->path().string();

        //edit name out of path
        filepath = filepath.substr(0,filepath.size() - filename.size()-1);

        //skip if dot file
        if(IgnoreHidden){
            if(filename.at(0) == '.'){
                continue;
            }
        }

        //Node* file;
        bool isDir = false;
        //check if directory for tree
        if (entry->is_directory()) {
            isDir = true;
        }
        if (!wildcardMatching(filename, filter) && !isDir){
            continue;
        }
        if (!(!runOnDir && isDir)){
            commandString = formatCommandString(command, nameFilters, pathFilters,
                 handleSpaces(filepath), handleSpaces(filename));
            
            //exit if commandstring has an error
            if (commandString == "ERROR")
                return;
            cout << "\nrunning command: "<<commandString << "\n";
            system(commandString.c_str());
            cout << endl;
        }

    }
}





int main(int argc, char* argv[]) {
    try {
        vector<char> options;
        vector<ArgValue*> nameFilters;
        vector<ArgValue*> pathFilters;
        string filter = "*";
        bool warn = true;
        bool ignoreHidden = true;
        bool runOnDir = false;
        fs::path workingPath = fs::current_path();
        if(argc == 1){
            throw runtime_error("No Arguments or Command String");
        }
        //Sort Arguments
        //Display all args for testing purposes 
        /*for (int i = 0; i < argc; ++i){
            string arg(argv[i]);
            cout << argv[i] << endl;
        }
        cout << "\n\n";//*/


        int iterator = 1;
        string arg(argv[iterator]);
        //read options. If options move iterator otherwise first arg is command string
        if (arg.at(0) == '-'){
            if (!(arg.find("f") == string::npos))
                warn = false;
            
            if (!(arg.find("a") == string::npos))
                ignoreHidden = false;
            
            if (!(arg.find("d") == string::npos))
                runOnDir = true;

            iterator++;
        }

        string commandString(argv[iterator]); iterator++;
        //after the commandString no arguments are 100% required in all 
        //cases so we skip some code to avoid a seg fault
        if (!(iterator == argc)){
            //check to see if path specifiyed
            arg = argv[iterator];
            arg = arg.substr(0,4);

            //if the next argument isn't a PATHx or NAMEx or empty argument it must be the directory
            //if not the workingPath is the current path
            if (!(arg == "PATH") && !(arg == "NAME")){
                arg = argv[iterator];
                workingPath = arg; iterator++;
            }

            //Sort Args between names and paths
            for (int i = iterator; i < argc; ++i){
                string arg(argv[i]);
                if (arg.substr(0,4) == "PATH"){
                    ArgValue* path = parseValueFromArg(arg);
                    pathFilters.push_back(path);
                }
                if (arg.substr(0,4) == "NAME"){
                    ArgValue* name = parseValueFromArg(arg);
                    if (name->index == "FILE.NAME0"){
                        filter = name->value;
                        continue; 
                    }  
                    nameFilters.push_back(name);
                }
            }

            //debug printout vectors
            /*cout << "PATHS:\n";
            for (ArgValue* temp : pathFilters){
                temp->printOut();
            }
            cout << "NAMES:\n";
            for (ArgValue* temp : nameFilters){
                temp->printOut();
            }//*/
        }
        if (warn){
            cout << "WARNING YOU ARE RESPONSIBLE FOR KNOWING HOW THE COMMAND THAT YOU WANT TO RUN RECURSIVLY WORKS!!!\nFiles & Directories to be affected: \n";
            readDirectory(workingPath, filter, ignoreHidden);
            
            cout << "Would you like to run the command on all these files? y/n: ";
            string yN;
            cin >> yN;
            cout << "\n";
            transform(yN.begin(), yN.end(), yN.begin(), ::tolower);
            if (!(yN == "y")){
                cout << "Command Aborted" << endl;
                return -1;
            }
        }
        executeCommand(workingPath, filter, ignoreHidden, commandString, nameFilters, pathFilters, runOnDir);
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
