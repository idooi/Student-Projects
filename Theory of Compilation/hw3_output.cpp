#include <iostream>
#include "hw3_output.hpp"
#include <sstream>

using namespace std;

void output::endScope(){
    //cout << "---end scope---" << endl;
}

void output::printID(const string& id, int offset, const string& type) {
    cout << id << " " << type <<  " " << offset <<  endl;
}

string typeListToString(const std::vector<string>& argTypes) {
    stringstream res;
    res << "(";
    for(int i = 0; i < argTypes.size(); ++i) {
        res << argTypes[i];
        if (i + 1 < argTypes.size())
            res << ",";
    }
    res << ")";
    return res.str();
}

string valueListsToString(const std::vector<string>& values) {
    stringstream res;
    res << "{";
    for(int i = 0; i < values.size(); ++i) {
        res << values[i];
        if (i + 1 < values.size())
            res << ",";
    }
    res << "}";
    return res.str();
}

string output::makeFunctionType(const string& retType, std::vector<string>& argTypes) {
    stringstream res;
    res << typeListToString(argTypes) << "->" << retType;
    return res.str();
}

void output::errorLex(int lineno){
    cout << "line " << lineno << ":" << " lexical error" << endl;
}

void output::errorSyn(int lineno){
    cout << "line " << lineno << ":" << " syntax error" << endl;
}

void output::errorUndef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " variable " << id << " is not defined" << endl;
}

void output::errorDef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " identifier " << id << " is already defined" << endl;
}

void output::errorUndefFunc(int lineno, const string& id) {
    cout << "line " << lineno << ":" << " function " << id << " is not defined" << endl;
}

void output::errorMismatch(int lineno){
    cout << "line " << lineno << ":" << " type mismatch" << endl;
}

void output::errorPrototypeMismatch(int lineno, const string& id, std::vector<string>& argTypes) {
    cout << "line " << lineno << ": prototype mismatch, function " << id << " expects arguments " << typeListToString(argTypes) << endl;
}

void output::errorUnexpectedBreak(int lineno) {
    cout << "line " << lineno << ":" << " unexpected break statement" << endl;
}

void output::errorUnexpectedContinue(int lineno) {
    cout << "line " << lineno << ":" << " unexpected continue statement" << endl;	
}

void output::errorMainMissing() {
    cout << "Program has no 'void main()' function" << endl;
}

void output::errorByteTooLarge(int lineno, const string& value) {
    cout << "line " << lineno << ": byte value " << value << " out of range" << endl;
}

void output::errorSetTooLarge(int lineno, const string& start, const string& end) {
    cout << "line " << lineno << ": set range " << start << " to " << end << " is invalid" << endl;
}
const string output::rules[] = {
	"Program -> Funcs", // 1
	"Funcs -> epsilon",
	"Funcs -> FuncDecl Funcs",
	"FuncDecl -> RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE",
	"RetType -> Type", // 5
	"RetType ->  VOID",
	"Formals -> epsilon",
	"Formals -> FormalsList",
	"FormalsList -> FormalDecl",
	"FormalsList -> FormalDecl COMMA FormalsList", // 10
	"FormalDecl -> Type ID",
	"Statements -> Statement",
	"Statements -> Statements Statement",
	"Statement -> LBRACE Statements RBRACE",
	"Statement -> Type ID SC", // 15
	"Statement -> Type ID ASSIGN Exp SC",
	"Statement -> ID ASSIGN Exp SC",
	"Statement -> Call SC",
	"Statement -> RETURN SC",
	"Statement -> RETURN Exp SC", // 20
	"Statement -> IF LPAREN Exp RPAREN Statement",
	"Statement -> IF LPAREN Exp RPAREN Statement ELSE Statement",
	"Statement -> WHILE LPAREN Exp RPAREN Statement",
	"Statement -> BREAK SC",
	"Statement -> CONTINUE SC", // 25
	"Call -> ID LPAREN ExpList RPAREN",
	"Call -> ID LPAREN RPAREN",
	"ExpList -> Exp",
	"ExpList -> Exp COMMA ExpList",
	"Type -> INT", // 30
	"Type -> BYTE",
	"Type -> BOOL",
	"Type -> SET LBRACKET NUM DOTS NUM RBRACKET",
	"Exp -> LPAREN Exp RPAREN",
	"Exp -> Exp BINOP Exp", // 35
	"Exp -> ID",
	"Exp -> Call",
	"Exp -> NUM",
	"Exp -> NUM B",
	"Exp -> STRING", // 40
	"Exp -> TRUE",
	"Exp -> FALSE",
	"Exp -> NOT Exp",
	"Exp -> Exp AND Exp",
	"Exp -> Exp OR Exp", // 45
	"Exp -> Exp RELOP Exp",
	"Exp -> LPAREN Type RPAREN Exp"
};
void output::printProductionRule(int ruleno) {
	 //cout << ruleno << ": " << output::rules[ruleno-1] << endl;
}

