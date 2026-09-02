#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main(int argcount, char *args[]){
    cout << argcount << endl;
    string out = "echo";
    string argString = "";
    for (int i = 1; i < argcount; ++i){
        argString += "-";
        argString += args[i];
        argString += " "; 
    }

    out += " " +  argString;
    system(out.c_str());
    return 0;
}

//Notes this will work for -args but for file names and other args with out a - it will append a dash
/* I need to use a different char than -, since that won't work with *args, to specifiy how arguments should be treated
similar to how ffmpeg works with LAME*/ 