#include <chrono>
#include <filesystem>  
#include <iostream>
#include <iomanip>
#include <vector>
#include <bits/stdc++.h>
using namespace std;
namespace fs = filesystem;

//Left over Tree stuff from before I just went throught the fs twice, Might revisit it later does have some efficency benifits but also adds to the complexity of the program and uses more RAM.
//Still could be useful as an option down the line though.
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

//class to store the index and value of NAMEx and PATHx
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

//takes a string as an input such as "NAME1=x*" and splits it between the index "NAME1" and the value "x*" then returns then as an ArgValue* 
ArgValue* parseValueFromArg(string argument){
    string index = argument.substr(0,5);
    index = "FILE." + index;
    string value = argument.substr(6,argument.size());
    return new ArgValue(index, value);
}

//basic wildcardMatching just checks to see if a string, "alpha" matches the pattern specified in "filter"
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

//Read throught the Directory recersivly and print the affected files and directories
void readDirectory(fs::path workingPath, string filter, bool IgnoreHidden){
    /*Node* root = new Node("",workingPath.string()); left over from tree building. see above comment ↑
    Node* currentDir = root;
    Node* prevDir = nullptr;//*/

    int totalFiles = 0;// tracks the total files affected
    int totalDir = 0; // tracks the total dir affected
    //int prevDepth = 0; left over from tree building. see above comment ↑
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
        //check to see if the file matches the filtered pattern
        if (!wildcardMatching(filename, filter) && !isDir){
            continue;
        }
        //build tree left over see above comment ↑
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
        
        //Print the Directory Tree that will be affected
        cout << setw(entry.depth()*3) << "";//tabs based on depth
        if (entry->is_directory()) {
            cout << "dir:  " << filename;
        }
        else if (entry->is_regular_file()) {
            cout << "file: " << filename;
        }
        else
            cout << "??    " << filename;
        cout << endl;
        //prevDepth = entry.depth(); left over from tree building. see above comment ↑
        if (!isDir)
            totalFiles++;
    }
    cout << "Total Directories to Be Affected: " << totalDir << endl;
    cout << "Total Files to Be Affected: " << totalFiles << endl;
}





//splits a string into two around a specified character, hereby refered to as wildcard
void StringSplitter(string& before, string& after, string original, string wildcard, bool include){
        if (original.find(wildcard) != string::npos){
            int index = original.find(wildcard);
            before = original.substr(0, index);
            //if include wildcard specified include the wildcard else don't
            if (include)
                after = original.substr(index, original.size());
            else
                after = original.substr(index + 1, original.size());
        }
    }

//This is what combins NAME0 with what is specified in NAMEx
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

//does what it say. finds a pattern with in a string and replaces it with a different string
string findAndReplace(string alpha, string filter, string replacement){
    int index = alpha.find(filter);
    int size = filter.size();
    if (index == string::npos){
        return alpha;
    }
    alpha.replace(index, size, replacement);

    //testing prints. ignore
    /*cout << "alpha: " << alpha << "| filter: " << filter<<endl;
    cout << "index: " << index << "| size: " << size<<"| replacement: "<<replacement<<endl;//*/
    return alpha;
}


//Replaces all instances of spaces with the escape sequence for a space, "\ ", allowing it to be used in the command
string handleSpaces(string alpha){
    string temp = "";
    string out = alpha;
    //replaces spaces with a character that is very unlikly to show up in any user inputed string:☺
    //we do this because the escape sequence includes a space so it would just continue replacing the same space over and over again.
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, " ", "☺");//ascii smilly face doesn't break c++ (SWIFT emoji vars aren't so special now) I'm using it since It probably won't appear in the string
    }
    temp = "";
    //then replaces ☺ with the space escape sequence
    while(!(temp == out)){
        temp = out;
        out = findAndReplace(out, "☺", "\\ ");
    }
    return out;
}

//The main formater for the commandString this is run for every command execution.
string formatCommandString(string alpha, vector<ArgValue*> nameFilters, 
    vector<ArgValue*> pathFilters, string filepath, string filename){////HERE
    string out = alpha;
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
    
    //Replace FILE.PATHx in the commandString
    for (ArgValue* path : pathFilters){
        string outCheck = findAndReplace(out, path->index, path->value + filepath);
        if (out == outCheck){
            throw runtime_error(path->index + " is not specified in the command string");//throw an error if a PATHx is in the command string but not specified
        }
        out = outCheck;
    }
    //Replace FILE.NAMEx
    for (ArgValue* name : nameFilters){
        string outCheck = findAndReplace(out, name->index, /*merge NAMEx with NAME0*/wildcardHandling(name->value, filename));
        if (out == outCheck){
            throw runtime_error(name->index + " is not specified in the command string");//throw an error if a NAMEx is in the command string but not specified
        }
        out = outCheck;
    }

    return out;
}
//Executes the command this is run for every file 
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
        //check to see if file matches the filtered pattern
        if (!wildcardMatching(filename, filter) && !isDir){
            continue;
        }

        //if -d isn't used and it is a directory skip running the command
        if (!(!runOnDir && isDir)){

            commandString = formatCommandString(command, nameFilters, pathFilters,
                 handleSpaces(filepath), handleSpaces(filename));
            
            //print command that will run
            cout << "\nrunning command: "<<commandString << "\n";
            //run the command
            system(commandString.c_str());
        }

    }
}

int main(int argc, char* argv[]) {
    try {
        //vectors for the NAME and PATH filters
        vector<ArgValue*> nameFilters;
        vector<ArgValue*> pathFilters;
        string filter = "*"; //if NAME0 isn't specified default to a blank wildcard

        //booleans for the options
        bool warn = true; // show warning
        bool ignoreHidden = true; // ignore hidden files
        bool runOnDir = false; // run command on directories

        //set path to current path unless specified later
        fs::path workingPath = fs::current_path();
        if(argc == 1){
            throw runtime_error("No Arguments or Command String"); //throw an error if rECR is run with out any arguments
        }
        //Sort Arguments
        //Display all args for testing purposes 
        /*for (int i = 0; i < argc; ++i){
            string arg(argv[i]);
            cout << argv[i] << endl;
        }
        cout << "\n\n";//*/

        int iterator = 1; // iterator for the argument. Order is important here
        string arg(argv[iterator]);
        //read options. If there are options move iterator otherwise first arg is command string
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
        //cases so we skip some code to avoid a seg fault if there are no more args
        if (!(iterator == argc)){
            //check to see if path specified
            arg = argv[iterator];
            arg = arg.substr(0,4);

            //if the next argument isn't a PATHx or NAMEx it must be the directory
            //if not the workingPath is the current path
            if (!(arg == "PATH") && !(arg == "NAME")){
                arg = argv[iterator];
                workingPath = arg; iterator++;
            }

            //Sort Args between names and paths and push them into their respective vectors
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

            //debug printout vectors ignore 
            /*cout << "PATHS:\n";
            for (ArgValue* temp : pathFilters){
                temp->printOut();
            }
            cout << "NAMES:\n";
            for (ArgValue* temp : nameFilters){
                temp->printOut();
            }//*/
        }

        //Warning skip if option f
        if (warn){
            cout << "WARNING YOU ARE RESPONSIBLE FOR KNOWING HOW THE COMMAND THAT YOU WANT TO RUN RECURSIVLY WORKS!!!\nFiles & Directories to be affected: \n";
            readDirectory(workingPath, filter, ignoreHidden);
            cout << "Would you like to run the command on all these files? y/n: ";
            string yN;
            cin >> yN;//user input
            cout << "\n";
            transform(yN.begin(), yN.end(), yN.begin(), ::tolower);
            if (!(yN == "y")){//if anything but "y" treat it like a no for safety
                cout << "Command Aborted" << endl;
                return -1;//stop the program
            }
        }
        executeCommand(workingPath, filter, ignoreHidden, commandString, nameFilters, pathFilters, runOnDir);
    }//simple error handling for the filesystem
    catch (const fs::filesystem_error& err) {
        cerr << "filesystem error! " << err.what() << endl;
        if (!err.path1().empty())
            cerr << "path1: " << err.path1().string() << endl;
        if (!err.path2().empty())
            cerr << "path2: " << err.path2().string() << endl;
    }//error handling for anything else I thought might make an error
    catch (const exception& ex) {
        cerr << "general exception: " << ex.what() << endl;
    }
}
