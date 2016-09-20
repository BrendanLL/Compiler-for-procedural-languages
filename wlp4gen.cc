#include <vector>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <utility>
using namespace std;


struct node{
  //int index;
  int childnum;
  string token1;
  string token2;
  vector<node> child;  
};

string leaves[]={"BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"};
set<string>leaf(leaves,leaves+35);
map<string,vector<string> > func;//
map<string,pair<vector<string>, map<string,string> > > allt;//

int count_child(string str){
    int child = 0 ;
    string exp;
    istringstream ss(str);
    ss>>str;
    if(leaf.count(str) > 0)return 0;
    while(ss>>str){
        child +=1;
    }
    return child;
}

string head_child(string str){
    istringstream ss(str);
    ss>>str;
    return str;
}

string rest_child(string str){
    string child=str;
    istringstream ss(str);
    ss>>str;
    child.erase(0,str.length()+1);
    return child;
}

void readin(node &n){
    if(n.childnum==0)return;//no child
    for(int i=0;i<n.childnum;i++){   
      string str;
      getline(cin,str);
      node newchild;
      //newchild.index = n.index+i+1;
      newchild.childnum = count_child(str);
      newchild.token1 =head_child(str);
      newchild.token2 =rest_child(str);
      readin(newchild);
      n.child.push_back(newchild);
    } 
}

string intorstar(string s){
    if(s=="INT")return "int";
    else return "int*";
}

string returntype(node n,map<string,string>&stable){
    string a = "int";
    string b = "int";
    string c = "well-typed";
    //cout << n.token1 << " "<<n.token2<<endl;
    if(n.token1 == "factor"){
        if(n.token2 == "ID"){
            a =stable[n.child[0].token2];
            return a;
        }if(n.token2 == "NUM"){
            return "int";
        }if(n.token2 == "NULL"){
            return "int*";
        }if(n.token2 == "LPAREN expr RPAREN"){
            a=returntype(n.child[1],stable);
            return a;
        }if(n.token2 == "AMP lvalue"){
            a = returntype(n.child[1],stable);
            if(a=="int"){
                return "int*";
            }else{
                string err = "ERROR: &int* is not vaild";
                throw(err); 
            }
        }if(n.token2 == "STAR factor"){
             a = returntype(n.child[1],stable);
            if(a=="int*"){
                return "int";
            }else{
                string err = "ERROR: * int is not vaild";
                throw(err); 
            }
        }if(n.token2 == "NEW INT LBRACK expr RBRACK"){
            a = returntype(n.child[3],stable);
            if(a=="int"){
                return "int*";
            }else{
                string err = "ERROR: new int[int*] is not vaild";
                throw(err); 
            }
        }
        if(n.token2 == "ID LPAREN RPAREN"){
            string id = n.child[0].token2;
            if(func[id].size()!=0){
                throw("ERROR: num of argument in "+ id +" is not match"); 
            }
            return "int";
        }if(n.token2 == "ID LPAREN arglist RPAREN"){            
            string id = n.child[0].token2;
            int length = func[id].size();            
            node tem =  n.child[2];
            if(length == 0){
                throw("ERROR: num of argument in "+ id +" is not match");
            }
            for(int i = 0;i<length;i++){     
                a = returntype(tem.child[0],stable);
                if(func[id][i] != a){
                     throw("ERROR: type of argument in "+ id +" is not match"); 
                }
                if(tem.token2=="expr COMMA arglist" && func[id].size()==i+1 ){
                    throw("ERROR: num of argument in "+ id +" is not match(+)"); 
                }
                if(tem.token2=="expr"){
                    if(func[id].size()!=i+1){
                    throw("ERROR: number of arguments in "+ id +" is not match"); 
                    }else{
                      break;
                    }
                }
                tem = tem.child[2]; 
            }
            return "int";
        }
    }
    if(n.token1 == "expr"){
        if(n.token2 == "term"){
            a = returntype(n.child[0],stable);
            return a;
        }else if(n.token2 == "expr PLUS term"){//expr PLUS/minus term 
           a = returntype(n.child[0],stable);
           b = returntype(n.child[2],stable);
           //cout<<a<<" ? "<<b<<endl;
            if(a == "int" && b=="int"){
                return "int";
            }else if((a == "int" && b=="int*")||(a == "int*" && b=="int")){
                return "int*";
            }else {
                string err = "ERROR: int* + int* is not vaild";
                throw(err); 
            }
        }else{//expr minus term 
            a = returntype(n.child[0],stable);
            b = returntype(n.child[2],stable);
            if(a == "int" &&b=="int"){
                return "int";
            }else if(a == "int*" && b=="int"){
                return "int*";
            }else if(a == "int*" && b=="int*"){
                return "int";
            }else{
                string err = "ERROR: int - int* is not vaild";
                throw(err); 
            }
        }
    }
    if(n.token1 == "term"){
        if(n.token2 == "factor"){
            a = returntype(n.child[0],stable);
            return a;
        }else{// term STAR/SLASH/PCT factor 
           a = returntype(n.child[0],stable);
            b = returntype(n.child[2],stable);
            if(a=="int" && b=="int"){
                return "int";
            }else{
                string err = "ERROR: int* for */ ./ / % is not vaild";
                throw(err); 
            }
        }
    }
    if(n.token1 == "lvalue"){
        if(n.token2 == "ID"){
            return stable[n.child[0].token2];
        }if(n.token2 == "STAR factor"){
            a = returntype(n.child[1],stable);
            if(a=="int*"){
                return "int";
            }else{
                string err = "ERROR: *int* is not vaild";
                throw(err); 
            }
        }if(n.token2 == "LPAREN lvalue RPAREN"){
            a = returntype(n.child[1],stable);
            return a;
        }
    }
    // Type Correctness Rules 
     if(n.token1=="test"){ //[Comparisons]
        a = returntype(n.child[0],stable);
        b = returntype(n.child[2],stable);
        if(a != b ){
            string err = "ERROR: Comparison for different type";
            throw(err); 
        }return "well-typed";
    }if(n.token1=="statements"){  //[Control flow]
        if(n.childnum == 0)return "well-typed";
        else{ //statements → statements statement 
            a = returntype(n.child[0],stable);
            b = returntype(n.child[1],stable);
            if(a=="well-typed" && b=="well-typed")return "well-typed";
            else{
                string err = "ERROR: Comparison for different type";
                throw(err);
            }
        }
    }if(n.token1=="statement"){
        if(n.token2=="WHILE LPAREN test RPAREN LBRACE statements RBRACE"){
            a = returntype(n.child[2],stable);
            b = returntype(n.child[5],stable);
            if(a=="well-typed" && b=="well-typed")return "well-typed";
            else{
                string err = "ERROR: not well-typed while control flow";
                throw(err);
            }
        }
        if(n.token2=="IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE"){
            a = returntype(n.child[2],stable);
            b = returntype(n.child[5],stable);
            c = returntype(n.child[9],stable);
            if(a=="well-typed" && b=="well-typed" && c=="well-typed")return "well-typed";
            else{
                string err = "ERROR: not well-typed if control flow";
                throw(err);
            }
        }
        if(n.token2=="DELETE LBRACK RBRACK expr SEMI"){
            a = returntype(n.child[3],stable);
            if(a=="int*") return "well-typed";
            else{
                string err = "ERROR: not well-typed deallocation";
                throw(err);
            }
        }
        if(n.token2=="PRINTLN LPAREN expr RPAREN SEMI"){
            a = returntype(n.child[2],stable);
            if(a=="int") return "well-typed";
            else{
                string err = "ERROR: not well-typed Printing";
                throw(err);
            }
        }
        if(n.token2=="lvalue BECOMES expr SEMI"){
            a = returntype(n.child[0],stable);
            b = returntype(n.child[2],stable);
            if(a == b)return "well-typed";
            else{
                string err = "ERROR: not well-typed Assignment";
                throw(err);
            }
        }
    }
    if(n.token1=="dcls"){
        if(n.token2=="")return "well-typed";
        if(n.token2=="dcls dcl BECOMES NUM SEMI"){
            a = returntype(n.child[0],stable);
            b = intorstar(n.child[1].child[0].token2);
            if(a=="well-typed" && b == "int") return "well-typed";
            else{
                string err = "ERROR: not well-typed Decl'ns(int)";
                throw(err);
            }
        }
        if(n.token2=="dcls dcl BECOMES NULL SEMI"){
            a = returntype(n.child[0],stable);
            b = intorstar(n.child[1].child[0].token2);
            if(a=="well-typed" && b == "int*") return "well-typed";
            else{
                string err = "ERROR: not well-typed Decl'ns(int*)";
                throw(err);
            }
        }
    }
    if(n.token1=="procedure"){//INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE 
        a = returntype(n.child[6],stable);
        b = returntype(n.child[7],stable);
        c = returntype(n.child[9],stable);
        if(a=="well-typed" && b == "well-typed" && c=="int") return "well-typed";
        else{
            string err = "ERROR: not well-typed Procedure";
            throw(err);
        }
    }
    if(n.token1=="main"){//INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE 
        a = intorstar(n.child[5].child[0].token2);
        b =  returntype(n.child[8],stable);
        c =  returntype(n.child[9],stable);
        string d = returntype(n.child[11],stable);
        //cout << a + " " + b+" "+c+" "+d <<endl;
        if(a=="int" && b == "well-typed" && c=="well-typed" && d=="int") return "well-typed";
        else{
            string err = "ERROR: not well-typed main";
            throw(err);
        }
    }
}



void editstable(node &n,map<string,string>&stable){
    if(stable.count("foo")>0) cout<<n.token1<<" "<<n.token2 <<endl;
    string id;
    string type;
    //cout << n.token1<<" "<<n.token2<<endl;
    if(n.token1 == "main"){
        if(stable.count("wain")>0) throw("ERROR: duplicate main function");
        stable["wain"]="int";
        string dcl1 = intorstar(n.child[3].child[0].token2) ;
        string dcl2 = intorstar(n.child[5].child[0].token2) ;
        vector<string> para;
        para.push_back(dcl1);para.push_back(dcl2);
        allt["wain"].first = para;
       // cerr << "wain "<< dcl1 <<" "<<dcl2<<endl; 
    }
    if(n.token1 == "procedure"){
        id = n.child[1].token2;
        if(func.count(id)){
            string errs = "ERROR: Duplicate declaration of function " + id;
            throw(errs);
        }
        // cerr << id;
        vector<string> inarg;
        node tem = n.child[3];
        if(tem.token2 ==  "paramlist"){
            tem = tem.child[0];
            for(;;){
                inarg.push_back(intorstar(tem.child[0].child[0].token2));
                if(tem.token2=="dcl COMMA paramlist"){
                    tem = tem.child[2];
                }else{//dcl ->type ID                    
                    break;
                }
            }
        }
        func[id]=inarg;
        /*for(int i=0;i<inarg.size();i++){
             cerr << " " << inarg[i];
        }        
        cerr << endl; */
        map<string,string> newstable;
        for(int i = 2;i<n.child.size();i++){
            editstable(n.child[i],newstable);
        } 
        //cerr << endl; 
        //returntype(n,newstable);
        allt[id].first=inarg;
        allt[id].second=newstable;
        return;
    }
    if(n.token1 == "dcl"){
        type = intorstar(n.child[0].token2);
        id = n.child[1].token2;
        if(stable.count(id)>0) throw("ERROR: duplicate " + id +" define");
        stable[id]=type; 
       // cerr << id <<" " <<type <<endl; 
    }
    if(n.token1 == "ID"){
        //cout << n.token2 <<endl;
        if(stable.count(n.token2)>0){}
        else{throw("ERROR: " + n.token2 +" not defined");}
    }    
    if(n.token1 == "factor" && (n.token2=="ID LPAREN arglist RPAREN" || n.token2=="ID LPAREN RPAREN")){
        id = n.child[0].token2;
        if(stable.count(id)>0){
            throw("ERROR: variable "+id+" is not a function");
            return;
        }//wtf?
        if(func.count(id)==0){
            throw("ERROR: function " + id +" not defined");
        }else{            
            for(int i = 1;i<n.child.size();i++){
                editstable(n.child[i],stable);
            }                       
        }
       // returntype(n,stable);
        return;
    }
    
    for(int i = 0;i<n.child.size();i++){
        //cout<<n.token2<<endl;
        editstable(n.child[i],stable);
    }  

   // if(n.token1=="term" || n.token1=="expr" || n.token1=="factor" || n.token1=="lvalue" || n.token1=="test" || n.token1=="statement" || n.token1=="statements" || n.token1=="dcls"  || n.token1=="main" ){
   //     //if(n.token1=="procedure")cout << "procedure"<<endl;
   //     returntype(n,stable);
  //  }
    allt["wain"].second=stable;
}



map<string,int> vtable;
int var = 1;
int loopY = 0;
int ifX = 0;
void push(int i){
    cout<< "sw $"<< i <<",-4($30)" <<endl;
    cout<< "sub $30,$30,$4" <<endl;
}

void pop(int i){
    cout<< "lw $"<< i <<",0($30)" <<endl;
    cout<< "add $30,$30,$4" <<endl;
}

void code(node &n){
      //cout << n.token1 << " " <<n.token2<<endl;
      if(n.token1 == "start")code(n.child[1]);
      else if(n.token1 == "procedures")code(n.child[0]);
      else if(n.token1 == "test"){
          code(n.child[0]);
          push(3);
          code(n.child[2]);
          pop(5);
          if(n.token2=="expr LT expr"){              
              cout<<"slt $3,$5,$3"<<endl;
          }
          else if(n.token2=="expr GT expr"){              
              cout<<"slt $3,$3,$5"<<endl;
          }
          else if(n.token2=="expr LE expr"){              
              cout<<"slt $3,$3,$5"<<endl;
              cout<<"sub $3,$11,$3"<<endl;
          }
          else if(n.token2=="expr GE expr"){              
              cout<<"slt $3,$5,$3"<<endl;
              cout<<"sub $3,$11,$3"<<endl;
          }
          else if(n.token2=="expr NE expr"){              
              cout<<"slt $6,$5,$3"<<endl;
              cout<<"slt $7,$3,$5"<<endl;
              cout<<"add $3,$6,$7"<<endl;
          }
          else if(n.token2=="expr EQ expr"){              
              cout<<"slt $6,$5,$3"<<endl;
              cout<<"slt $7,$3,$5"<<endl;
              cout<<"add $3,$6,$7"<<endl;
              cout<<"sub $3,$11,$3"<<endl;
          }
      }
      else if(n.token1 == "statements"){
          if(n.token2 == ""){return;}
          else{
              code(n.child[0]);
              code(n.child[1]);
          }
      }
      else if(n.token1 == "lvalue"){
          if(n.token2 == "ID"){
              cout<<"sw $3,"<< (1-vtable[n.child[0].token2])*4<<"($29)"<<endl;      
        }
          else if (n.token2 == "LPAREN lvalue RPAREN"){
              code(n.child[1]);
          }
      }
      else if(n.token1 == "statement"){
          if(n.token2 == "PRINTLN LPAREN expr RPAREN SEMI"){
              code(n.child[2]);
              cout<<"add $1,$3,$0"<<endl;
              cout<<"lis $5"<<endl;
              cout<<".word print"<<endl;
              cout<<"jalr $5"<<endl;
              cout<<"lw $1,0($29)"<<endl;
            }else if(n.token2 == "lvalue BECOMES expr SEMI"){
                code(n.child[2]);
                code(n.child[0]);
            }else if(n.token2 == "WHILE LPAREN test RPAREN LBRACE statements RBRACE"){
                int y = loopY;
                loopY++;
                cout<<"Loop"<<y<<":"<<endl;
                code(n.child[2]);
                cout<<"beq $3,$0,endLoop"<<y<<endl;
                code(n.child[5]);
                cout<<"beq $0,$0,Loop"<<y<<endl;
                cout<<"endLoop"<<y<<":"<<endl;                
            }    
            else if(n.token2 == "IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE"){
                int x = ifX;
                ifX++;
                code(n.child[2]);
                cout<<"beq $3,$0,else"<<x<<endl;
                code(n.child[5]);
                cout<<"beq $0,$0,endif"<<x<<endl;
                cout<<"else"<<x<<":"<<endl;
                code(n.child[9]);
                cout<<"endif"<<x<<":"<<endl;                
            }
      }
      else if(n.token1 == "dcls"){
            if(n.token2 == "")return;
            else if(n.token2 == "dcls dcl BECOMES NUM SEMI"){
                code(n.child[0]);
                cout<<"lis $3"<<endl;
                cout<<".word "<<n.child[3].token2 <<endl;
                code(n.child[1]);
            }   
        }
      else if(n.token1 == "dcl"){
          string dcl = n.child[1].token2;
          vtable[dcl] = var++;   //code(n.child[3]);//dcl1
          //cout<<dcl<<" dcldcl "<<var-1<<endl;
          if(var <= 3){
              cout<<"sw $"<<vtable[dcl]<< ",-4($30)"<<endl;//1 0 29
          }else{
              cout<<"sw $3,-4($30)"<<endl;
          }          
          cout<< "sub $30,$30,$4" <<endl;          
      }
      else if(n.token1 == "expr"){
          if(n.token2=="term"){
            code(n.child[0]);//term
          }else {
              code(n.child[0]);//expr
              push(3);
              code(n.child[2]);
              pop(5);
              if(n.token2=="expr PLUS term"){
                cout<<"add $3,$5,$3"<<endl;
              }else if(n.token2=="expr MINUS term"){;
                cout<<"sub $3,$5,$3"<<endl;
            }          
         }
      }
      else if(n.token1 == "term"){
          if(n.token2 == "factor"){
              code(n.child[0]);//factor
          }else{
              code(n.child[0]);//expr
              push(3);
              code(n.child[2]);
              pop(5);
              if(n.token2 == "term STAR factor"){
                cout<<"mult $5,$3"<<endl;
                cout<<"mflo $3"<<endl;
              }else if(n.token2 == "term SLASH factor"){
                cout<<"div $5,$3"<<endl;
                cout<<"mflo $3"<<endl;
              }else if(n.token2 == "term PCT factor"){
                cout<<"div $5,$3"<<endl;
                cout<<"mfhi $3"<<endl;
              }          
          }
      }
      else if(n.token1 == "factor"){
          if(n.token2 == "LPAREN expr RPAREN"){
              code(n.child[1]);
          }
          else if(n.token2 == "ID"){
            string s = n.child[0].token2;
            cout<<"lw $3,"<<(1-vtable[n.child[0].token2])*4<<"($29)"<<endl;
         }
         else if(n.token2 == "NUM"){
            cout<< "lis $3"<<endl;
            cout<<".word "<<n.child[0].token2<<endl;
            return;
         }
      }
      else if(n.token1 == "main"){
          code(n.child[3]);//dcl1
          code(n.child[5]);//dcl2
          code(n.child[8]);//dcls
          code(n.child[9]);//statements          
          //cout<<"lw $2,-4($29)"<<endl;
          //return
          code(n.child[11]);//expr
          //cout << "add $30,$29,$4"<<endl;                      
          cout <<"add $30,$29,$4"<<endl;        
          cout<<"lw $31,4($29)"<<endl;
          //cout<<"add $30,$30,$4"<<endl;
          cout << "jr $31" <<endl;          
      }
}

int main(){ 
    string str;
    getline(cin,str);
    node top;
    top.childnum = count_child(str);
    top.token1 =head_child(str);
    top.token2 =rest_child(str);
    readin(top);
    map<string,string>stable;
    try{
        editstable(top,stable);        
    }catch(string err){
        cerr << err << endl;
    }
    cout<<".import print"<<endl;
    cout << "lis $4" <<endl;
    cout << ".word 4" <<endl;
    cout << "lis $11" <<endl;
    cout << ".word 1" <<endl;    
    cout<<"sw $31,-4($30)"<<endl;
    cout<< "sub $30,$30,$4" <<endl;
    cout <<"sub $29,$30,$4"<<endl;
    code(top);
}
