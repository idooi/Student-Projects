//
// Created by IdoSarel on 12/11/20.
//

#ifndef OUTPUT_CPP_YYSTYPE_H
#define OUTPUT_CPP_YYSTYPE_H

#include "hw3_output.hpp"
#include "bp.hpp"
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <algorithm>

using namespace output;
using namespace std;

#define YYSTYPE Node*

typedef vector<pair<int,BranchLabelIndex>> AddressList;

typedef struct Node Node;

class Entry{
public:
    string name;
    string type;
    int offset;
    string var;
    Entry(string name, string type, int offset, string var):
            name(name), type(type), offset(offset), var(var)
    {}
    Entry(const Entry &p2) {
        this->name = p2.name;
        this->type = p2.type;
        this->offset = p2.offset;
        this->var    = p2.var;
    }
    Entry& operator=(Entry p2)
    {
        this->name = p2.name;
        this->type = p2.type;
        this->offset = p2.offset;
        this->var    = p2.var;
        return *this;
    }
};
class Scope{
public:
    vector<Entry> entries;
    Scope():entries(vector<Entry>()){}
    Scope(const Scope &p2) {this->entries = p2.entries; }
    Scope& operator=(Scope p2)
    {
        this->entries = p2.entries;
        return *this;
    }
    ~Scope(){
        /*for (auto i  = entries.begin(); i != entries.end(); ++i){
            delete &i;

        }*/
        //  printf("destroy");
    };
};



void init();

Scope newScope();
void closeScope();

bool Defined(string name, bool var);
bool nameDefined(string name);
void updateSymType(string name, string newType);

void mainCheck();

struct Node{
public:
    string  lex;
    string type;
    string  var;
    int   value;
    int*    set;
    string setL, setR;
    AddressList  nextList;
    AddressList  trueList;
    AddressList falseList;
    Node()= default;
    Node(char* text):lex(string(text)), type("my TYPEE!!"){}
    Node(string name, string type):
            lex(name), type(type), var("my VARR"),
            nextList(AddressList()),
            trueList(AddressList()),
            falseList(AddressList()){}
    Node(string name, string type, string var):
            lex(name), type(type), var(var),
            nextList(AddressList()),
            trueList(AddressList()),
            falseList(AddressList()){}
    Node(string name, string type, string var, int value):
            lex(name), type(type), var(var), value(value),
            nextList(AddressList()),
            trueList(AddressList()),
            falseList(AddressList()){}
    virtual ~Node() = default;
};

Entry insertInCurrent (string name, string type, string var);
Entry insertOffset(string name, string type, int offset, string var);

Entry* getSymEntry(string sym);


class Program: public Node{
    Program(): Node(){}
   
};


class Exp: public Node{
public:
    Exp(string name, string type,string var):Node(name, type, var){}
    Exp(const Exp &p2):Node(p2.lex, p2.type, p2.var){
        this->setL      = p2.setL;
        this->setR      = p2.setR;
        this->trueList  = p2.trueList;
        this->falseList = p2.falseList;
        this->nextList  = p2.nextList;
    }
    Exp& operator=(Exp p2)
    {
        this->lex       = p2.lex ;
        this->type      = p2.type;
        this->var       = p2.var ;
        this->setL      = p2.setL;
        this->setR      = p2.setR;
        this->trueList  = p2.trueList;
        this->falseList = p2.falseList;
        this->nextList  = p2.nextList;
        return *this;
    }
};
class ExpList: public Node{
public:
    vector<Exp> args;
    ExpList(vector<Exp> args):Node(),args(args){}
    ExpList(const ExpList &p2):Node(){this->args = p2.args;}
};

class FormalDecl: public Node{
    public:
    FormalDecl(string name, string type, string var):Node(name, type, var){}
    FormalDecl(const FormalDecl &p2):Node(p2.lex, p2.type, p2.var){}
};

class FormalsList: public Node{
public:
    vector<FormalDecl> args;
    FormalsList(vector<FormalDecl> args):Node(),args(args){}
    FormalsList(const FormalsList &p2):Node(){this->args = p2.args;}
};

class Formals: public FormalsList{
public:
    Formals(vector<FormalDecl> args):FormalsList(args){}
    Formals(const Formals &p2):FormalsList(p2.args){}
};


void insertArgs(vector<FormalDecl> args);

vector<string> getArgsList(string name);

string getFunctype(string fName);

void pt();

bool setCheck(string start, string end);

bool voidCheck(string fName);

bool specialDup(string t1, string t2);

bool argsCheck(string fName, vector<Exp> args);

string freshVar();

string llvmType(string type);

string llvmOp(string op);

void setError(string op);

string llvmVar(string id);

string llvmPhi(string type,string label1, string value1, string label2, string value2);

string llvmString(string str);

string llvmStrPtr(string str);

string llvmGetBoolVar(Exp* exp);

string llvmBrCond(string cond, string label1, string label2);

string llvmBrCondP(string cond, string label1, string label2);

int llvmBr();

string llvmGEP(string llvmName ,string type, int len,int i);

string llvmVarPtr(int offset);

void llvmInitArr(string name, string type, string length);

void llvmdivByZero(string var);

string getSetType(string l, string r);

string getSetL(string type);

string getSetR(string type);

string llvmSetType(string setL, string setR);

string llvmSetType(string len);

string setLen(string setL, string setR);

string freshSet();

string llvmSetToInt(string set, string setL, string setR);

string llvmIntToSet(string integer, string setL, string setR);

void llvmLoadSet(string set, string setL, string setR, string stackPtr);

void llvmStoreSet(string newSet, string setL, string setR, string ptr);

string llvmRawSet(string setL, string setR);

string llvmSet(string setL, string setR);

string llvmInsertToSet(Exp* setNode, string op, int element);

string llvmSetCast(Exp* setNode);

string copySet(Exp* setNode);

string llvmOpSet(Exp* setNode, string op, string element);

string llvmInSet(string element, Exp* setNode);

void initVars(string func);

string llvmDivByZeroFunc();

void llvmOutOfRange(Exp* setNode, string var, string op);

void llvmFuncDecl(string type, string name, vector<FormalDecl>& args);

string llvmIntToByte(string intVar);

string llvmCall(string name, string type, vector<Exp> args);

void finish();

#endif //OUTPUT_CPP_YYSTYPE_H
