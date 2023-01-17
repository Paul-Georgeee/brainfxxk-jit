#include <IR.hpp>
#include <string>
#include <iostream>

using namespace std;
void jit_run(vector<IR> &ir);
void interprete_run(vector<IR> &ir);

int main(int argc, const char *argv[])
{
    if(argc == 1)
        cout << "usega: jit [-i] filePath" << endl;
    else
    {
        const char *path = argc == 2 ? argv[1] : argv[2];
        ifstream fin;
        fin.open(path, ios::in);
        vector<IR> ir;
        if(!fin){
            cerr << "Open file error\n";
            exit(-1);
        }
        gen_IRs(ir, fin);

        if(argc == 2)
            jit_run(ir);
        else if(argc == 3 && string(argv[1]) == "-i")
            interprete_run(ir);
        else 
            cout << "usega: jit [-i] filePath" << endl;
        fin.close();
    }
}