#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <list>
using namespace std;


set<string> terms;
set<string> nonterms;
string start;

vector<string> finput(1,"BOF BOF");
vector<int> state;
vector<string> iin;
vector<string> action;
vector<string> outt; 
vector<string> rule;
vector<string> lr1;
int error = 0;
struct Tree {
    string rule;
    list<Tree*> children;

    ~Tree() {
        for(list<Tree*>::iterator it=children.begin(); it != children.end(); it++) {  // delete all subtrees
            delete (*it);
        }
    }
};

void popper(stack<Tree *> &myStack, list<string> &rhs, string rule) {
    Tree *n = new Tree();
    n->rule = rule;
    for(list<string>::iterator it=rhs.begin(); it != rhs.end(); it++){
        Tree *tmp = myStack.top();
        n->children.push_front(tmp);
        myStack.pop();
    }
    myStack.push(n);
}

Tree* lrdo() {
    stack<Tree*> myStack;
    string l; // lhs symbol
	int i =0;
    do {
		//cout<<"doing?"<<endl;
        string f;
        f = lr1[i];// getline(cin,f);
		i ++;
        list<string> r; // rhs symbols
        istringstream iss(f);
        iss >> l; // lhs symbol
        string s;
        while(iss >> s) {
            if(nonterms.count(s) > 0) r.push_back(s); // only non-terminals
        }
        popper(myStack, r, f); // reduce rule
    } while(start != l);
    return myStack.top();
}

int counterforfinput =  0;
void traverse(Tree *t, int d) {
    //for(int i = 0; i < finput.size(); i++) cout << "finout: "<< finput[i]<<endl;    
	string s = t->rule;
    cout << s << endl; // print root
	int spaces = 1;
	for(int x = 0;x<s.length();x++){
		if( s[x] == ' ' )spaces++;
	}istringstream ss(s); ss >> s;
	list<Tree*>::iterator it=(t->children).begin();
	while(ss>>s){
		//if(it == (t->children).end())break;
		if(terms.count(s) > 0) {
			cout<<finput[counterforfinput]<<endl;
			counterforfinput++;
		}else{traverse(*it, d+1);it++;}
	}
	//for(list<Tree*>::iterator it=(t->children).begin(); it != (t->children).end(); it++) {  // print all subtrees
	//			traverse(*it, d+1);
	//}
}


void skipLine(istream &in) {
	string s;
	getline(in, s);
}

void readsyms(istream &in,set<string> &t) {
    int n;
    string temp;
    getline(in,temp);
    istringstream iss(temp);
    iss >> n;
    if (iss.fail()) return;
    for(int i = 0; i < n; i++) {
        getline(in,temp);
        t.insert(temp);
    }
}

void readrule(istream &in) {
	int n;
	string s;
	in >> n; skipLine(in);
	//cout << "rule number: " << n << endl;
	for (int i = 0; i < n; i++) {
		getline(in, s);
		//cout << s << endl;
		rule.push_back(s);
	}
}

void readin(istream &in) {
	int line_n;
	in >> line_n; skipLine(in);
	//cout << line_n << endl;
	int pos;
	string s;
	string s1;
	string s2;
	string s3;
	for (int i = 0; i < line_n; i++) {
		getline(in, s);
		istringstream ss(s);
		ss >> pos; ss >> s1;
		ss >> s2;  ss >> s3;
		state.push_back(pos);
		iin.push_back(s1);
		action.push_back(s2);
		outt.push_back(s3);
	}
}

void printout(istream &in) {
	string s;	
	stack<int> pass_state;
	vector<string> output;
	int length = state.size();
	string current_read;  //up
	getline(in,s);
    istringstream iss(s);
    iss >> current_read; //in >> current_read;
	finput.push_back(s);
	int current_state = 23;//up
	string main_reading = current_read;
	int reading_pos = 1;//up
	int reading = 0; //check if one chara is finish reading
	while (true) {  //read last line one by one		
		for (int i = 0; i < length; i++) {
			if ( reading == 2 and in.eof()){ reading = 4;}
			else if (reading == 2) { reading = 0; }//prevent to shift twice
			//cout << "matching " << current_state << " " << current_read;
			//cout << " with " << state[i] << " " << iin[i] <<endl;			
			//cout << "reading " << reading << endl;
			if (state[i] == current_state) {
				if (iin[i] == current_read) {
			//		cout << "matching " << current_state << " " << current_read;
			//		cout << " with " << state[i] << " " << iin[i] << endl;
			//	    cout << "matched return " << action[i] << " " << outt[i] << endl;
					
					if (action[i] == "reduce") {
						int num;
						istringstream sss(outt[i]);
						sss >> num;
						output.push_back(rule[num]);
						istringstream ssss(rule[num]);
						ssss >> s;
						main_reading = current_read;
						current_read = s;
						while (!ssss.eof()) {
							ssss >> s;
							current_state = pass_state.top();
							pass_state.pop();
						}
						reading_pos--;
						reading=1;//keep checking reduce		
						//cout << "reduce: "<< rule[num] << endl;
						lr1.push_back(rule[num]);
					}
					else {//shift
						pass_state.push(current_state);
						int num;
						istringstream sss(outt[i]);
						sss >> num;
						current_state = num;
						reading_pos++;
						//cout << "when shift  reading is " << reading << endl;
						//if(in.eof()){cout << reading <<endl;}
						if (reading == 1) { 
							reading = 2;//use main reading to continue 
						}else if(reading == 4){
							//cout << "4 then 5" <<endl;
							reading = 5; // eof 
						}else if(in.eof()){
							//cout << "eof then 4" <<endl;
							reading = 4; // eof 
						}else { 
							reading = 3; //use new chara 
						}
					}
					//cout << "reading " << reading << endl;
					break;
				}//if match(rule find)
			}//if state			
		}//for loop
		//cout << " reading " << reading<<" "<<current_read << endl;
		if (in.eof() and reading == 5) {
				lr1.push_back(rule[0]);
				return;		
		}else if (reading == 0) { //error(rule not find)
			cerr << "ERROR at " << reading_pos << endl;
			error = 1;
			return;//rule not find
		} else if (reading == 1) {//continue reading
			
		}
		else if (reading == 2) {  //check no more reduce 
			current_read = main_reading;
			//reading = 3;
		}else {                   //readin next chara
			getline(in,s);			
			finput.push_back(s);
            iss.str(s);
            iss >> current_read; //in >> current_read;
			//cout << reading << " reading next " << current_read <<endl;
			if (in.eof()) {  //check no more reduce 
				current_read = "EOF";
				main_reading = current_read;
				//finput.push_back("EOF EOF");
				finput[finput.size()-1]="EOF EOF";
				reading = 0;
			}else{
				/*getline(in,s);			
				finput.push_back(s);
				iss.str(s);
				iss >> current_read; //in >> current_read;*/
				main_reading = current_read;
				reading = 0;	
			}
		}
	//	if(in.fail()) cout << "reach eof at reading "<< current_read  << endl; 
		//cout << "current_read " << current_read  << endl;	
		//cout << endl;
	}

}

int main() {
	ifstream is;
	is.open("WLP4.lr1",ifstream::in);
	readsyms(is,terms); // read terminals
	readsyms(is,nonterms); // read nonterminals
	getline(is,start); // read start symbol
	readrule(is);
	int state_n;
	int trans_n;
	is >> state_n;
	readin(is);
	printout(cin);
	if(error == 0){
		Tree *parsetree = lrdo();
		traverse(parsetree, 0); // write forward leftmost derivation
		delete parsetree;
	}
	is.close();
}
