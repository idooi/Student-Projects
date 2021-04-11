//
// Created by IdoSarel on 12/11/20.
//


#include "compsrc.h"
#include "bp.hpp"

#define MAX_LOCAL 50
#define L_I "0"
#define R_I "1"
#define SIZE_I "2"
#define SET_META_SIZE 3
#define PTR "i64*"
using namespace std;

vector<Scope> symTable;
stack<int> offStack;
int t;
CodeBuffer& buffer = CodeBuffer::instance();
string stackPtr;
int stackLen = MAX_LOCAL;

Entry insertInCurrent (string name, string type, string var){
    int currOff = offStack.top();
    int newOff = currOff + 1;
    offStack.pop();
    offStack.push(newOff);
    Entry e = Entry(name, type, currOff, var);
    Scope* scope = &symTable.back();
    scope->entries.push_back(e);
    return e;
}

Entry insertOffset (string name, string type, int offset, string var){
   // buffer.emit(" im inserting "+name +type + to_string(offset) + " " +var);
    Entry e = Entry(name, type, offset, var);
    Scope* scope = &symTable.back();
    scope->entries.push_back(e);
    return e;
}

void insertArgs(vector<FormalDecl> args){
      int offset = -1;
      for (int i = 0; i < args.size() ; i++){
         // buffer.emit(" hey :" +args[i].var);
          insertOffset(args[i].lex, args[i].type, offset, args[i].var);
          offset--;
      }
    }


bool isVar(Entry e){
      return (e.type.find(";") == string::npos);
  }

void pt(){
    string type = "";
    vector<Entry> entries = symTable.back().entries;
    for (int i = 0; i < entries.size(); i++){
        vector<string> args;
        string funcType;
        type = entries[i].type;
        if (!isVar(entries[i])){//a function
            args = getArgsList(entries[i].name);
            funcType = args[args.size()-1];
            args.pop_back();
            //args = reverse(args);
            for(int i = 0; i< args.size(); i++){
                                  //                          << args[i] << " " ;
                                                           }
                        //                                   cout<< endl;
            type = makeFunctionType(funcType, args);
           // cout<< "funcType " << funcType << "type: " <<type << endl; 
        }
       // printID(entries[i].name, entries[i].offset, type);
    }
  }
  
Scope newScope(){
    Scope newScp = Scope();
    if (&newScp == nullptr || &symTable == nullptr) printf("");
    symTable.push_back(newScp);
    int newOffset = offStack.top();
    offStack.push(newOffset);
    return newScp;
}

void closeScope(){
    endScope();
  //  pt();
    symTable.pop_back();
    offStack.pop();
}



bool Defined(string name, bool var){
    for (auto symi = symTable.begin(); symi != symTable.end(); ++symi){
        for (auto enti = symi->entries.begin(); enti != symi->entries.end(); ++enti){
            if (enti->name == name && (var == isVar(*enti))){
                return true;
            }
        }
    }
    return false;
}

bool nameDefined(string name){
    for (auto symi = symTable.begin(); symi != symTable.end(); ++symi){
        for (auto enti = symi->entries.begin(); enti != symi->entries.end(); ++enti){
            if (enti->name == name){
                return true;
            }
        }
    }
    return false;
}


Entry * getSymEntry(string name){
    for (auto symi = symTable.begin(); symi != symTable.end(); ++symi){
        for (auto enti = symi->entries.begin(); enti != symi->entries.end(); ++enti){
            if (enti->name == name){
                return &*enti;
            }
        }
    }
    return nullptr;
}

void updateSymType(string name, string newType){
    for (auto symi = symTable.begin(); symi != symTable.end(); ++symi){
        for (auto enti = symi->entries.begin(); enti != symi->entries.end(); ++enti){
            if (enti->name == name){
                enti->type = newType;
            }
        }
    }
}

string getFunctype(string fName){
      Entry * e = getSymEntry(fName);
      string type = e->type;
      int lastCom = type.find_last_of(",");
      return  (lastCom != string::npos ? 
              type.substr(lastCom + 1, type.length() - 1 - lastCom - 1):
              type.substr(0, type.length() - 1)
              );
}

vector<string> getArgsList(string name){
    Entry * e = getSymEntry(name);
    vector<string> args = vector<string>();
    string argsStr = e->type;
    argsStr = argsStr.substr(0, argsStr.length() - 1);
    string curr = "";
    for (int i = 0; i < argsStr.length(); i++){
        if(argsStr[i] == ','){
            args.push_back(curr);
            curr = "";
        }else{
            curr += argsStr[i];
        }
    }
    args.push_back(curr);
    return args;
}


bool argsCheck(string fName, vector<Exp> args) {
     vector<string> realArgs = (getArgsList(fName));
  //   cout<< "real size: "<< realArgs.size() << " size:" << args.size()<< endl; 
    if (realArgs.size() - 1 != args.size()){
        return false;
    }
    for (int i = 0; i < realArgs.size() - 1 ; ++i) {
  //  cout<< "in args loop num "<< i << endl;
      //      cout<< "real arg: "<< realArgs[i] << " arg:" << args[i].type << endl;
        if (realArgs[i].substr(0,3) == "SET" && args[i].type.substr(0,3) =="SET") continue;//TODO piazza i think TODO
        if (!(realArgs[i] == "INT" && args[i].type == "BYTE") && realArgs[i] != args[i].type) {//stupid piazza shit
            return false;
        }
    }
    return true;
}

void mainCheck(){
    if (!(Defined("main", false) && getFunctype("main") == "VOID" && getArgsList("main").size() == 1)){
      errorMainMissing();
      exit(0);
    }
}

bool setCheck(string start, string end){
    int a = stoi(start);
    int b = stoi(end)  ;
    return(!(a < 0 || b > 255));  
}

bool voidCheck(string fName){
    vector<string> realArgs = getArgsList(fName);
    return (realArgs[realArgs.size() - 1] == "VOID");
}

bool specialDup(string t1, string t2){
      return ((t1 == "INT"  && t2 == "BYTE") || 
              (t1 == "BYTE" && t2 == "INT"));
            }


string freshVar(){
    t++;
    return "%v_" + to_string(t - 1);
}

string freshStr(){
    t++;
    return "str_" + to_string(t - 1);
}

string freshSet(){
    t++;
    return "%set_" + to_string(t - 1);
}

string newLabel(){
    t++;
    return "label_t_" + to_string(t - 1);
}


string llvmType(string type){
    //buffer.emit("m type is " + type);
    return (type == "VOID"                   ? "void" :
           (type.find("SET") != string::npos ? "i32" :
           (type == "STRING"                 ? "i8*"  : "i32")));
}
string llvmOp(string op){
    return (op == "+"  ? "add" :
           (op == "-"  ? "sub" :
           (op == "*"  ? "mul" :
           (op == "/"  ? "div" :
           (op == "<"  ? "lt"  :
           (op == "<=" ? "le"  :
           (op == ">"  ? "gt"  :
           (op == ">=" ? "ge"  :
           (op == "==" ? "eq"  :
           (op == "!=" ? "ne"  :
           "WTF"))))))))));
}

string llvmVar(string id){
    return "%" + id + "_ptr";
}

string setLen(string setL, string setR){
    return to_string(stoi(setR) - stoi(setL) + 1 + SET_META_SIZE);
}

string llvmSetType(string setL, string setR){
    return "[" + setLen(setL, setR) + " x i32]";
}

string llvmSetType(string len){
    return "[" + len + " x i32]";
}


string llvmPhi(string type,string label1, string value1, string label2, string value2){
    return "phi " + type + " ["+ value1+ ", %" + label1 + "], [" + value2 + ", %" + label2 + "]";
}

string llvmBrCondP(string cond, string label1, string label2){
    return "br i1 " + cond + ", label %" + label1 + ", label %"+ label2;
}

string llvmBrCond(string cond, string label1, string label2){
    return "br i1 " + cond + ", label " + label1 + ", label "+ label2;
}
int llvmBr(){
    return buffer.emit("br label @");
}

string llvmGEP(string llvmName, string type, string len, string i){
    string setType = llvmSetType(len);
    return  "getelementptr " + \
    setType + ", " + setType + "* " + llvmName + ", i32 0, i32 " + i ;
   // "i32, i32* " + llvmName + ", i32 0, i32 " + i ;
}
string llvmGEP(string llvmName ,string type, int len, int i){
    string setType = llvmSetType(to_string(len));
    return  "getelementptr " + \
    setType + ", " + setType + "* " + llvmName + ", i32 0, i32 " + to_string(i) ;
}
string llvmGEPStr(string llvmName , int len, int i){
    return  "getelementptr "  \
    "[" + to_string(len) + " x i8], [" + to_string(len)+ " x i8]* " + llvmName + ", i32 0, i32 " + to_string(i);
}

string llvmString(string str) {
    string fresh = freshStr();
    string strGlob = "@." + fresh;
    str = str.substr(1, str.length() - 2);//get the quotations out
    string strVar = freshVar();
   // buffer.emit("my str is [" +str + "]");
    buffer.emitGlobal(strGlob  + " = constant [" + to_string(str.length() + 1)+" x i8] c\"" + str + "\\00\"");
    buffer.emit(strVar+" = " + llvmGEPStr(strGlob, str.length() + 1, 0));
    return strVar;
}
string llvmStrPtr(string str){
    string fresh = freshStr();
    string strGlob = "@." + fresh;
    str = str.substr(1, str.length() - 2);//get the quotations out
    string strVar = freshVar();
    buffer.emitGlobal(strVar+" = " + llvmGEPStr(strGlob, str.length() + 2, 0));
    return strVar;
}

string llvmGetBoolVar(Exp* exp){
    string newVar = freshVar();
    string true_label  = buffer.genLabel();
    int trueLoc = buffer.emit("br label @");
    string false_label = buffer.genLabel();
    int falseLoc = buffer.emit("br label @");
    buffer.bpatch(exp->trueList , true_label );
    buffer.bpatch(exp->falseList, false_label);
    string phiLabel = buffer.genLabel();
    buffer.bpatch(buffer.makelist({trueLoc,  FIRST}), phiLabel);
    buffer.bpatch(buffer.makelist({falseLoc, FIRST}), phiLabel);
    buffer.emit(newVar + " = " + llvmPhi("i32", true_label, "1", false_label, "0"));
    return newVar;
}

void llvmInitArr(string name, string type, string length){
    //cout <<"INITArr"<<endl;
    string llvmName = name;
    string arrType = "[" + length + " x " + type + "]";
    //buffer.emit(llvmName + " = alloca " + arrType);
    string label_entry  = newLabel(),
           label_inloop = newLabel(),
           label_out    = newLabel();
    string llvmName_i   = name + "_i"  ,
           llvmName_cmp = name + "_cmp",
           llvmName_ni  = name + "_ni" ,
           llvmName_e   = name + "_e"  ;
    buffer.emit(";InitArr");
    buffer.emit(\
    "br label %"+ label_entry+"\n" +\
    label_entry + ":\n" + \
    "br label %"+ label_inloop+"\n" +\
    label_inloop + ":\n" +\
    llvmName_i + " = " + llvmPhi("i32",label_entry, "0",label_inloop,llvmName_ni) + "\n" + \
         llvmName_e +" = " + llvmGEP(llvmName, arrType, length, llvmName_i) +"\n"+\
         "store " + type + " 0,  i32* " + llvmName_e + "\n"+\
         llvmName_ni + " = add i32 " + llvmName_i + ", 1\n"+\
    llvmName_cmp + " = icmp slt i32 " + llvmName_ni + ", " + length +"\n"+\
    llvmBrCondP(llvmName_cmp, label_inloop,label_out)+"\n" + \
    label_out + ":");
}


void printArr(string set, string len){
    string setType = llvmSetType(len);
    string exe = +"_"+to_string(++t);
    string setEPtr = set +exe+ "_e";
    string setEPtr2 = set + exe+"_e2";
    string setE = freshVar();
    string llvmName_i = set + exe +"_i";
    string label_entry = newLabel(),
            label_inloop = newLabel(),
            label_out = newLabel();
    string  llvmName_cmp = set +exe+ "_cmp",
            llvmName_ni = set + exe+"_ni",
            llvmName_e = set + exe+"_e";
    buffer.emit(";printSet");
    buffer.emit(\
    "br label %"+ label_entry+"\n" +\
    label_entry + ":\n" + \
    "br label %"+ label_inloop+"\n" +\
    label_inloop + ":\n" +\
    llvmName_i + " = " + llvmPhi("i32", label_entry, "0", label_inloop, llvmName_ni) + "\n" + \
          llvmName_e +" = " + llvmGEP(set, "i32", len, llvmName_i) +"\n"+\
          setE + " = load i32, i32* " + setEPtr + "\n"+\
          "call void @printi(i32 " + setE +")\n" +\
          llvmName_ni + " = add i32 " + llvmName_i + ", 1\n" + \
    llvmName_cmp + " = icmp slt i32 " + llvmName_ni + ", " + len +"\n"+\
    llvmBrCondP(llvmName_cmp, label_inloop,label_out)+"\n" + \
    label_out + ":\n" + \
     "call void @printi(i32 -15)");
    buffer.emit(";EndPrintSet");
}

string copySet(Exp* setNode){
    //cout<<"copy llvminset" + setNode->setL +  " " + setNode->setR<<endl;
    string newSet = llvmRawSet(setNode->setL, setNode->setR);
    string len = setLen(setNode->setL, setNode->setR);
    string setType = llvmSetType(len);
    string set = setNode->var;
    string exe = +"_"+to_string(++t);
    string setEPtr = set +exe+ "_e";
    string setEPtr2 = set + exe+"_e2";
    string setE = freshVar();
    string llvmName_i = set + exe +"_i";
    string label_entry = newLabel(),
            label_inloop = newLabel(),
            label_out = newLabel();
    string  llvmName_cmp = set +exe+ "_cmp",
            llvmName_ni = set + exe+"_ni",
            llvmName_e = set + exe+"_e";
    buffer.emit(";copySet");
    buffer.emit(\
    "br label %"+ label_entry+"\n" +\
    label_entry + ":\n" + \
    "br label %"+ label_inloop+"\n" +\
    label_inloop + ":\n" +\
    llvmName_i + " = " + llvmPhi("i32", label_entry, "0", label_inloop, llvmName_ni) + "\n" + \
          llvmName_e +" = " + llvmGEP(set, "i32", len, llvmName_i) +"\n"+\
          setE + " = load i32, i32* " + setEPtr + "\n"+\
          setEPtr2 +" = " + llvmGEP(newSet, "i32", len, llvmName_i) +"\n"+\
          "store i32 " + setE + ", i32* " + setEPtr2 + "\n" +\
          llvmName_ni + " = add i32 " + llvmName_i + ", 1\n" + \
    llvmName_cmp + " = icmp slt i32 " + llvmName_ni + ", " + len +"\n"+\
    llvmBrCondP(llvmName_cmp, label_inloop,label_out)+"\n" + \
    label_out + ":");
    //printArr(set, len);
//   / buffer.emit("call void @printi(i32 -70)");
    //printArr(newSet, len);
    return newSet;
}

void llvmdivByZero(string var){
    buffer.emit("call void (i32) @divByZero(i32 " + var + ")");
}

void llvmWrite(string name, string index, string value, string length){
    string llvmName = name;
    string arrType = llvmSetType(length);
    string v_e = freshVar();
    buffer.emit(";llvmWrite");
    buffer.emit(\
    v_e + " = " + llvmGEP(llvmName, "i32", length, index)+"\n"+\
    "store i32 " + value + ", i32* " + v_e);
}

string getSetType(string l, string r){
    return "SET[" + l + "-" + r+ "]";
}

string getSetL(string type){
    int dash = type.find("-");
    return (type.substr(4, dash - 4));
}

string getSetR(string type){
    int dash = type.find("-");
    return (type.substr(dash + 1, type.length() - dash - 2));
}

string llvmSetL(string setPtr, string len, string setL){
    string setLptr = freshVar();
    return
    setLptr + " = " + llvmGEP(setPtr, "i32", stoi(len), stoi(L_I)) + "\n"+ \
    setL + " = load i32, i32* " + setLptr+ "\n";
}
string llvmSetR(string setPtr, string len, string setR){
    string setRptr = freshVar();
    return
    setRptr + " = " + llvmGEP(setPtr, "i32", stoi(len), stoi(R_I)) + "\n"+\
    setR + " = load i32, i32* " + setRptr+ "\n";
}

string llvmCast(string var, string type, string newType){
    return "bitcast " + type + " " + var + " to " + newType;
}

string llvmCastStackPtr(string ptr, string var){
    return var + " = " + llvmCast(ptr, "i32*", "i32**");
}

string llvmInttoptr(string var, string type, string newType){
    return "inttoptr " +type + " " + var + " to " + newType;
}
string llvmPtrtoint(string var, string type, string newType){
    return "ptrtoint " +type + " " + var + " to " + newType;
}

string llvmSetToInt(string set, string setL, string setR){
    string setType = llvmSetType(setL, setR);
    string casted = freshVar();
    buffer.emit(casted + " = " + llvmPtrtoint(set, setType + "*", "i32"));
    return casted;
}

string llvmIntToSet(string integer, string setL, string setR){
    string setType = llvmSetType(setL, setR);
    string set = freshSet();
    buffer.emit(set + " = " + llvmInttoptr(integer, "i32", setType + "*"));
    return set;
}


void llvmLoadSet(string set, string setL, string setR, string ptr){
    string setType = llvmSetType(setL, setR);
    //string casted = freshVar();
    string temp = freshVar();
    buffer.emit(
    temp + " = load i32 , i32* " + ptr + "\n"+ \
    set  + " = " + llvmInttoptr(temp, "i32", setType + "*"));
}
//void llvmLoadSet(string set, string setL, string setR, string ptr){
//    string setType = llvmSetType(setL, setR);
//    string casted = freshVar();
//    string temp = freshVar();
//    buffer.emit(llvmCastStackPtr(ptr, casted)+"\n" + \
//    temp + " = load i32*, i32** " + casted + "\n"+ \
//    set  + " = " + llvmCast(temp, "i32*", setType + "*"));
//}


void llvmStoreSet(string newSet, string setL, string setR, string ptr){
    string setType = llvmSetType(setL, setR);
    //string castedPtr = freshVar();
    string castedSet = freshVar();
    buffer.emit(\
    castedSet + " = " + llvmPtrtoint(newSet, setType + "*", "i32") + "\n" +\
    "store i32 " + castedSet + ", i32* " + ptr);
}

//void llvmStoreSet(string newSet, string setL, string setR, string ptr){
//    string setType = llvmSetType(setL, setR);
//    string castedPtr = freshVar();
//    string castedSet = freshVar();
//    buffer.emit(\
//    llvmCastStackPtr(ptr, castedPtr) + "\n" +\
//    castedSet + " = " + llvmCast(newSet, setType + "*", "i32*") + "\n" +\
//    "store i32* " + castedSet + ", i32** " + castedPtr);
//}
string llvmRawSet(string setL, string setR) {
    //cout<<"llvmset const1 " + setL +  " " + setR<<endl;
    string len = setLen(setL, setR);
    string temp = freshVar(), temp2 = freshVar(), temp3 = freshVar();
    string set = freshSet();
    string setType  = llvmSetType(len);
    buffer.emit(";llvmset");
    buffer.emit(temp + " = call i8* @malloc(i32 " + len + ")\n" + \
     temp2 + " = " + llvmPtrtoint(temp, "i8*", "i32") + "\n" +\
     set  + " = " + llvmInttoptr(temp2, "i32", setType + "*"));
    //buffer.emit(set + " = " +llvmCast(temp, "i8*",setType + "*"));
    return set;
}

string llvmSet(string setL, string setR) {
    //cout<<"llvmset const1 " + setL +  " " + setR<<endl;
    string len = setLen(setL, setR);
    string set = llvmRawSet(setL,setR);

    llvmInitArr(set, "i32", len);

    //buffer.emit(setL + " = add i32 " + setL + ", 0\n" + \
     //setL + " = add i32 " + setR + ", 0");
    llvmWrite(set, L_I, setL, len);
    llvmWrite(set, R_I, setR, len);
    //llvmWrite(set, SIZE_I, len, len);
    //cout<<"going to return set" <<endl;
    //printArr(set, len);
    return set;
}

void llvmOutOfRange(Exp* setNode, string var, string op){
    string error  = "@.range";
    error += (op == "-" ? "Minus" : (op == "+" ? "Plus" : "In"));
    string errLen = to_string(op == "in" ? 31 : 30);
    string llvmName = setNode->var;
    string label_goodL = newLabel()+"_goodL";
    string label_good  = newLabel()+"_good";
    string label_shit  = newLabel()+"_shit";
    string len = setLen(setNode->setL, setNode->setR);
    string setL = freshVar(),
            setR = freshVar(),
            setLptr = freshVar(),
            setRptr = freshVar();
    string cmpL = freshVar(), cmpR = freshVar();
    buffer.emit(";llvmoutofRange");
    buffer.emit(\
    llvmSetL(llvmName, len, setL) + \
    llvmSetR(llvmName, len, setR) + \
    cmpL + " = icmp sle i32 " + setL + ", " + var+"\n"+\
    llvmBrCondP(cmpL, label_goodL, label_shit) + "\n"+\
    label_shit +":\n"+\
    "call void (i8*) @print(i8* getelementptr (["+ errLen + " x i8], ["+ errLen + " x i8]* " + error + ", i32 0, i32 0))\n"+\
    "call void (i32) @exit(i32 0)\n"+\
    "br label %"+ label_goodL+"\n" +\
    label_goodL + ":\n" +\
    cmpR + " = icmp sge i32 " + setR + ", " + var+"\n"+\
    llvmBrCondP(cmpR, label_good, label_shit) + "\n"+\
    label_good + ":");
}

string llvmSetCast(Exp* setNode){
    string set = setNode->var;
    string sizePtr = freshVar();
    string size = freshVar();
    string len = setLen(setNode->setL, setNode->setR);
    buffer.emit(sizePtr + " = " + llvmGEP(set, "?", len, SIZE_I));
    buffer.emit( size+ " = load i32, i32* " + sizePtr);
    return size;
}

void llvmSizeInc(string set, string len, string op){
    string ptr = freshVar();
    string varOld = freshVar();
    string varNew = freshVar();
    string delta = (op == "+" ? "1" : "-1");

    buffer.emit(ptr + " = " + llvmGEP(set, "?", len, SIZE_I));
    buffer.emit(varOld + " = load i32, i32* " + ptr);
    buffer.emit(varNew + " = add i32 " + varOld + ", " + delta);
    buffer.emit("store i32 " + varNew + ", i32* " + ptr);
}



string llvmOpSet(Exp* setNode, string op, string element){
    //cout<<"llvmopset" + setNode->setL +  " " + setNode->setR<<endl;
    buffer.emit(";opset");
    string newSet = copySet(setNode);
    string index = freshVar();
    buffer.emit(index + " = sub i32 " + element  + ", " + to_string(stoi(setNode->setL) - SET_META_SIZE));
    string len = setLen(setNode->setL, setNode->setR);
    string setType =llvmSetType(len);
    string gepString = llvmGEP(newSet, "?", len, index);

    string cmp = freshVar();
    string in  = freshVar();
    string label_size_change = newLabel() + "_size_change";
    string label_f    = newLabel();
    string value    = (op == "-" ? "0" : (op == "+" ? "1" : "WTF"));
    string cmpValue = (op == "-" ? "1" : (op == "+" ? "0" : "WTF"));
    string ptr = freshVar();
    string storeString = "store i32 " + value + " , i32* " + ptr;
    buffer.emit(ptr + " = " + gepString + "\n" + \
    in + " = load i32, i32* " + ptr + "\n" +\
    cmp + " = icmp eq i32 " + cmpValue + ", "+ in +"\n"+\
    llvmBrCondP(cmp, label_size_change, label_f)+ "\n"+\
    label_size_change+ ":");
    llvmSizeInc(newSet, len, op);
    buffer.emit("br label %" + label_f +"\n" +\
    label_f + ":\n"+\
    storeString);
    //printArr(setNode->var, len);
  //  buffer.emit("call void @printi(i32 -70)");
    //printArr(newSet, len);
    return newSet;
}


string llvmInSet(string element, Exp* setNode){
    string index = freshVar();
    buffer.emit(index + " = sub i32 " + element  + ", " + to_string(stoi(setNode->setL) - SET_META_SIZE));
    //cout<<"llvminset" + setNode->setL +  " " + setNode->setR<<endl;
    string len = setLen(setNode->setL, setNode->setR);
    string setType =llvmSetType(len);
    string gepString = llvmGEP(setNode->var, "?", len, index);
    string present = freshVar();
    string ptr = freshVar(), icmp = freshVar();
    buffer.emit(ptr + " = " + gepString  + "\n" + \
    present + " = load i32, i32* " + ptr + "\n" + \
    icmp + " = icmp eq i32 1, " + present );
    //buffer.emit("call void @printi(i32 -7000)");
    //printArr(setNode->var, len);
    return icmp;
}

/*void initVars(string func){
    Scope* currScope = &symTable.back();
    vector<Entry> entries = currScope->entries;
    Entry  *e;
    for (int i = 0 ; i < entries.size(); i++){
        e = &entries[i];
        string type = "i32";
        string ptr = llvmVar(e->name);
        if (e->type.find("SET") != string::npos) {//set
            int length = stoi(getSetL(e->type)) - stoi(getSetR(e->type)) + 1;
            length += 2;// for setL, setR
            llvmInitArr(ptr, type, to_string(length));
            llvmWrite(ptr, to_string(0), getSetL(e->type), to_string(length));
            llvmWrite(ptr, to_string(1), getSetR(e->type), to_string(length));
        }
        else {
            buffer.emit(ptr + " = alloca i32");
        }
    }
}*/

string llvmDivByZeroFunc(){
    string label_shit = "label_shit";
    string label_good = "label_good";
    return \
    "define void @divByZero(i32) {\n"\
    "%cond = icmp eq i32 0, %0\n"+\
    llvmBrCondP("%cond", label_shit, label_good)+ "\n"+\
    label_shit+ ":\n" +\
    "call void (i8*) @print(i8* getelementptr ([23 x i8], [23 x i8]* @.divError, i32 0, i32 0))\n"+\
    "call void (i32) @exit(i32 0)\n"+\
    "br label %"+label_good +"\n"+\
    label_good+ ":\n" +\
    "ret void\n"+\
    "}";
}


/*void llvmInsertToSet(string name, string var, int setR, int setL){
    string index = freshVar();
    buffer.emit(index + " = sub i32 " + var + ", " + to_string(setL));
    int length = setR - setL + 1 + 2;
    llvmWrite(name, index, to_string(1), to_string(length));
}*/

void llvmFuncDecl(string type, string name, vector<FormalDecl>& args){
    string argsDecl = "";
    if (args.size() > 0) {
        for (int i = 0; i < args.size() - 1; i++) {
            argsDecl += llvmType(args[i].type) + ", ";
        }
            argsDecl += llvmType(args[args.size() - 1].type);
    }
    stackPtr = "%" + name + "_Stack";
    stackLen = args.size() + MAX_LOCAL;
    buffer.emit("define "+ llvmType(type) + " @" + name + "(" + argsDecl + "){");
    buffer.emit(stackPtr + " = alloca " + "[" + to_string(args.size() + MAX_LOCAL) + " x i32]");
    string currStackPtr, argReg;
    //insert arguments to stack and puts them in new regs:
    for (int i = 0; i < args.size(); i++){
        argReg = "%" + to_string(i);
        //args[i].var = fre;//argReg;
        currStackPtr = freshVar();
        buffer.emit(currStackPtr + " = " +llvmGEP(stackPtr, "i32", stackLen, i +MAX_LOCAL));
        buffer.emit("store i32 " + argReg + ", i32* " + currStackPtr);
        //buffer.emit(argReg + " = load i32, i32* " + currStackPtr);
    }
}

string llvmIntToByte(string intVar){
    string truncated = freshVar();
    string var       = freshVar();
    buffer.emit(truncated + " = trunc i32 " + intVar + " to i8");
    buffer.emit(var + " = zext i8 " + truncated + " to i32");
    return var;
}

string llvmVarPtr(int offset){
    offset = ( offset < 0 ? - (offset) - 1 + MAX_LOCAL : offset);
    string ptr = freshVar();
    buffer.emit(ptr +" = " +llvmGEP(stackPtr ,"i32", stackLen ,offset));
    return  ptr;
}

string llvmCall(string name, string type, vector<Exp> args){
    string argsDecl = "";
    string argsUsage ="";
    if (args.size() > 0) {
        for (int i = 0; i < args.size() - 1 ; i++){
            argsDecl  += llvmType(args[i].type) + ", ";
            argsUsage += llvmType(args[i].type) + " " + args[i].var + ", ";
        }
            argsDecl  += llvmType(args[args.size() - 1].type);
            argsUsage += llvmType(args[args.size() - 1].type)+" " + args[args.size() - 1].var;
    }
    return "call " + llvmType(type)+(llvmType(type) == "void" ? "" : "(" + argsDecl+ ")")+ " @" + name + "(" +argsUsage +")";
}

bool isGlobal(string name){
    vector<Entry> entries = symTable[0].entries;
    for(int i = 0; i< entries.size(); i++){
        if (entries[i].name == name){
            return true;
        }
    }
    return false;
}

void init(){
    symTable = vector<Scope>();
    Scope newScope = Scope();
    symTable.push_back(newScope);
    offStack = stack<int>();
    offStack.push(0);
    //now stupid print funcs
    insertOffset("print", "STRING,VOID;", 0, "");
    insertOffset("printi","INT,VOID;", 0, "");
    //declares:
    buffer.emitGlobal(\
    "declare i32  @printf(i8*, ...)\n" \
    "declare void @exit(i32)\n" \
    "declare i8*  @malloc(i32)");
    buffer.emitGlobal("@.int_specifier   = global [4 x i8] c\"%d\\0A\\00\"");
    buffer.emitGlobal("@.str_specifier   = global [4 x i8] c\"%s\\0A\\00\"");
    buffer.emitGlobal("@.divError   = global [23 x i8] c\"Error division by zero\\00\"");
    buffer.emitGlobal("@.rangeMinus = global [30 x i8] c\"Error out of set range. Op: -\\00\"");
    buffer.emitGlobal("@.rangePlus  = global [30 x i8] c\"Error out of set range. Op: +\\00\"");
    buffer.emitGlobal("@.rangeIn    = global [31 x i8] c\"Error out of set range. Op: in\\00\"");
    //definitions:
    buffer.emitGlobal(\
    "define void @printi(i32) {\n"\
    "    call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)\n"\
    "    ret void\n"\
    "}");
    buffer.emitGlobal(\
    "define void @print(i8*) {\n"\
    "    call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)\n"\
    "    ret void\n"\
    "}");
    //stupid divide by zero
    buffer.emitGlobal(llvmDivByZeroFunc());
}

void finish(){
    //cout<< "da GLOBAL:"<<endl;
    buffer.printGlobalBuffer();
    //cout<< "da GOOD:"<<endl;
    buffer.printCodeBuffer();
}