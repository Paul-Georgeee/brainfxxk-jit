#include <IR.hpp>
#include <assert.h>
#include <iostream>
#include <string>
#include <stack>
using namespace std;

static void err(string s, int line, int col)
{
    cerr << "At line " << line << " col " << col << ": " << s << endl;
    exit(-1);
}

void gen_IRs(std::vector<IR> &ir, std::ifstream &fin)
{
    string tmp;
    int line = 0;
    stack<pair<int, int>> s;
    stack<int> match;
    while(getline(fin, tmp))
    {
        line++;
        int len = tmp.length();
        for(int i = 0; i < len; ++i)
        {
            int cnt = 0;
            switch (tmp[i])
            {
            case '+': case '-':
                do{
                    cnt = tmp[i] == '+' ? cnt + 1 : cnt - 1;
                    ++i;
                }while(i < len && (tmp[i] == '+' || tmp[i] == '-'));
                if(cnt > 0)
                    ir.push_back(IR(AddVal, cnt));
                else if(cnt < 0)
                    ir.push_back(IR(SubVal, -cnt));
                --i;
                break;
            case '>': case '<':
                do{
                    cnt = tmp[i] == '>' ? cnt + 1 : cnt - 1;
                    ++i;
                }while(i < len && (tmp[i] == '>' || tmp[i] == '<'));
                if(cnt > 0)
                    ir.push_back(IR(AddPtr, cnt));
                else if(cnt < 0)
                    ir.push_back(IR(SubPtr, -cnt));
                --i;
                break;
            case ',':
                ir.push_back(IR(Read));
                break;
            case '.':
                ir.push_back(IR(Write));
                break;
            case '[':
                s.push(make_pair(line, i + 1));
                match.push(ir.size());
                ir.push_back(IR(Jz));
                break;
            case ']':
                if(s.empty())
                    err("Unexpected ]", line, i + 1);
                s.pop();
                ir.push_back(IR(Jnz, match.top() + 1));
                ir[match.top()].op = ir.size();
                match.pop();
                break;
            case '\n': case ' ':
                break;
            default:
                //err("Unexpected symbol " + string(1, tmp[i]), line, i + 1);
                break;
            }
        }
    }
    if(!s.empty())
        err("Miss match for [", s.top().first, s.top().second);
#ifdef DEBUG
    print_IRs(ir);
#endif

}

void print_IRs(std::vector<IR> &ir)
{   
    
    ofstream o;
    o.open("bfcode.txt");
    for(auto &i: ir)
    {
        switch (i.kind)
        {
        case AddVal:
            o << "AddVal " << i.op << endl;
            break;
        case SubVal:
            o << "SubVal " << i.op << endl;
            break;
        case AddPtr:
            o << "AddPtr " << i.op << endl;
            break;
        case SubPtr:
            o << "SubPtr " << i.op << endl;
            break;
        case Read:
            o << "Read" << endl;
            break;
        case Write:
            o << "Write" << endl;
            break;
        case Jz:
            o << "Jz" << endl;
            break;
        case Jnz:
            o << "Jnz" << endl;
            break;
        
        default:
            break;
        }
    }
    o.close();
}
