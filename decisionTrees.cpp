#include<iostream>
#include<vector>
#include<cstdlib>
#include<bits/stdc++.h>
#include<queue>
#include<chrono>
#include<string>
#include<fstream>
#include<math.h>
#include<sstream>

using namespace std::chrono;
using namespace std;

#define MAX_SIZE 500

class Val{
    public:
        string tag;
        int counter;
        inline Val(string t, int c){ this->tag=t; this->counter=c;};
        inline Val(){this->tag=""; this->counter=0;};
        string getTag();
};

string Val::getTag(){
    string t = this->tag;
    return t;
};


class Example{
    public:
        string tag;
        vector<pair<int,int>> atr_val;
        int n_atributes;
        string clas;

        inline Example(string t = "", int n_a = 0){ this->tag=t; this->n_atributes=n_a; this->clas="";};
        void setClass(string c);
        void addAtribute(pair<int, int> a_v, int index);
};

void Example::setClass(string c){
    this->clas=c;
}

void Example::addAtribute(pair<int, int> a_v, int index){
    this->atr_val.push_back(a_v);
    this->n_atributes++;
}


class Atribute{
    public:
        string tag;
        vector<Val> values;
        int n_values;
        int entropy;
        vector<float> value_limits;
        Atribute(string tag);
        Atribute();
        void addValue(Val v);
        void setAtribute(string t);
};

void Atribute::setAtribute(string t){
    this->tag=t;
    this->n_values=0;
    this->entropy=0;
}

Atribute::Atribute(string t){
    this->tag=t;
    this->n_values=0;
    this->entropy=0;
}

Atribute::Atribute(){
    this->tag="";
    this->n_values=0;
    this->entropy=0;
}

void Atribute::addValue(Val v){
    this->values.push_back(v);
    this->n_values++;
}


class Node{
    public:
        int atr;
        int val;
        char type;              //a para atr, v para val e l para folhas
        string clas;
        vector<int> atrs_left;
        vector<int> exs;
        Node* parent;
        vector<Node*> children;

    Node(int atr, vector<int> atrs, vector<int> exs){
        this->atr = atr;
        this->atrs_left = atrs;
        this->exs = exs;
    }

    void addChild(Node* child);
    inline int getNChildren(){   return this->children.size();};
    inline Node getCHildAt(int i){ return (*this->children[i]);};
    inline vector<int> getExamples(){return this->exs;};

};

    void Node::addChild(Node* child){
        this->children.push_back(child);
    }

vector<Atribute> atrs;
vector<Example> examples;       //tentativa de tornar os vetores globais para serem usados como base de dados
vector<Example> tests;          //Exemplos do ficheiro teste, sem classes
vector<bool> discre;            //tem de ser global para construir arvores direito

int chooseHighestEntropy(vector<int> atribs, vector<int> exs);

Node rootNode(vector<int> atribs, vector<int> exs){
    int atr = chooseHighestEntropy(atribs, exs);
    remove(atribs.begin(), atribs.end(), atr);
    atribs.resize(atribs.size()-1);
    Node n = Node(atr, atribs, exs);
    n.type = 'a';
    return n;
}

bool atributeIsValue(Example e, int a, int v){
    if(e.atr_val[a].second == v){
        return true;
    }
    return false;
}

vector<int> filterExamples(vector<int> exs, int a, int v){
    Example e;
    vector<int> newVec;
    for(int i = 0; i < exs.size(); i++){
        e = examples[exs[i]];
        if(atributeIsValue(e, a, v)){
            newVec.push_back(exs[i]);
        }
    }
    return newVec;
}

void test_node(Node n);

void expandAtributeNode(Node* parent){
    Atribute atribute = atrs[(*parent).atr];
    int tatr = (*parent).atr;
    vector<int> filtered_exs;
    for(int i = 0; i < atribute.n_values; i++){
        filtered_exs = filterExamples((*parent).exs, (*parent).atr, i);
        //cout<<"EXAMPLE TEST: "<<endl;
        Node* n = new Node((*parent).atr, (*parent).atrs_left, filtered_exs);
        (*n).val = i;
        (*n).type = 'v';
        //test_node(*n);
        if((*n).exs.size() > 0){
            (*n).parent = parent;
            (*parent).addChild(n);
            //cout<<" Number of children: "<<(*parent).getNChildren()<<endl;
        }
    }
}

bool allSameClass(vector<int> exs);

void expandValueNode(Node* parent){
    vector<int> exampls;
    if((*parent).exs.size() == 0){
        return;
    }
    if(allSameClass((*parent).exs)){ //leaf node
        Node* n = new Node((*parent).atr, (*parent).atrs_left, (*parent).exs);
        (*n).type = 'l';
        (*n).val = -1;
        (*n).clas = examples[(*parent).exs[0]].clas;
        (*n).parent = parent;
        (*parent).addChild(n);
        //test_node(*n);
    }
    else{   //atribute node
        int new_atr = chooseHighestEntropy((*parent).atrs_left, (*parent).exs);
        vector<int> new_atrs_left = (*parent).atrs_left;
        remove(new_atrs_left.begin(), new_atrs_left.end(), new_atr);
        new_atrs_left.resize((*parent).atrs_left.size()-1);
        Node* n = new Node(new_atr, new_atrs_left, (*parent).exs);
        (*n).type = 'a';
        (*n).val = -1;
        (*n).parent = parent;
        (*parent).addChild(n);
        //test_node(*n);
    }
}

int ValueAlreadyRead(vector<Atribute> atributes, string value, int atr_index){
      for( int i=0; i<atributes[atr_index].n_values;i++){
          //strcmp is working on the opposite way so this is correct
          if(atributes[atr_index].values[i].tag.compare(value) == 0){
              return i;
          }

      }
      return -1;
}

void printAtributes(vector<Atribute> atributes){
    int n_a=atributes.size();
    for(int i=0; i<n_a; i++){
        cout<<atributes[i].tag<<endl;
        if(atributes[i].n_values>0){
            for(int j=0; j<atributes[i].n_values;j++){
                cout<<"\t"<<atributes[i].values[j].tag<< ": "<<atributes[i].values[j].counter<<endl;
         }
       }
    }
}

void printExamples(vector<Example> examples){
    int n_e=examples.size();
    cout<<"examples size: "<<n_e<<endl;
    for(int i=0;i<n_e;i++){
        cout<<examples[i].tag<<": "<<endl;
        int n_atrs=examples[i].n_atributes;
        cout<<"n atributos"<<n_atrs<<endl;
        for(int j=0;j<n_atrs;j++){
            cout<<examples[i].atr_val[j].first<<","<<examples[i].atr_val[j].second<<" class: "<<examples[i].clas<<" n_atrs: "<<n_atrs<<endl;
        }
    }
}

vector<string> split(const char *str, char c = ',')
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}

int findValue(Atribute atr, string value){

    for (int i=0; i<atr.values.size();i++){
        if(atr.values.at(i).tag.compare(value) == 0)
            return i;
    }

    return -1;
}

bool allSameClass(vector<int> exs){ //se todos os exemplos tiverem a mesma classe
    for(int i = 1; i < exs.size(); i++){
        if(examples[exs[0]].clas != examples[exs[i]].clas){
            return false;
        }
    }
    return true;
}


//count instances of a value in examples with indexes exs and atribute of index a
int count_value(int a, int v, int n_exs, vector<int> exs){
    int c = 0;
    for(int i = 0; i < n_exs; i++){
        if(examples[exs[i]].atr_val[a].second == v){
            c++;
        }
    }
    return c;
}

float entropy(int atr, int n_exemplos, vector<int> exs){
    int n_values = atrs[atr].values.size();
    float ent=0;
    float p[n_values];
    //calculamos as probabilidades para cara valor do atributo
    if(n_exemplos>0){
        for(int i=0; i<n_values; i++){
            p[i] = (float) count_value(atr, i, n_exemplos, exs)/n_exemplos;
         }
    } else{
      perror("invalid data size");
      exit(EXIT_FAILURE);
    }
    for(int i=0; i<n_values; i++){
        if(p[i]>0){
            ent += p[i]*log2(1/p[i]);
        }
    }
    return ent;
}


int chooseHighestEntropy(vector<int> atribs, vector<int> exs){
    int n_exs = exs.size();
    int n_atrs = atribs.size();
    float max_entropy = 0;
    float temp;
    int max_atribute;

    for(int i = 0; i < n_atrs; i++){
        temp = entropy(atribs[i], n_exs, exs);
        if(temp > max_entropy){
            max_entropy = temp;
            max_atribute = atribs[i];
        }
    }
    return max_atribute;
}

vector<int> index_vector(int size){
    vector<int> v;
    for(int i = 0; i < size; i++){
        v.push_back(i);
    }
    return v;
}

void test_node(Node root){
    cout<<" Node test: "<<root.type<<endl;
    if(root.type == 'a' || root.type == 'v'){
        cout<<" Atribute: "<<atrs[root.atr].tag<<endl;
    }
    if(root.type == 'v'){
        cout<<" Value: "<<atrs[root.atr].values[root.val].tag<<endl;
    }
    if(root.type == 'l'){
        cout<<" Leaf Node Class: "<<root.clas<<endl;
    }
    for(int i = 0; i < root.atrs_left.size(); i++){
        cout<<"atrs_left ["<<i<<"] : "<<root.atrs_left[i]<<endl;
    }
    for(int i = 0; i < root.exs.size(); i++){
        cout<<"exs ["<<i<<"] : "<<root.exs[i]<<endl;
    }
}

void buildTree(Node *root){
    if((*root).type == 'a'){
        //cout<<"Expanding atribute node :"<<atrs[(*root).atr].tag<<endl;
        expandAtributeNode(root);
    }
    else{
        if((*root).type == 'v'){
            //cout<<"Expanding value node :"<<atrs[(*root).atr].tag<<";"<<atrs[(*root).atr].values[(*root).val].tag<<endl;
            expandValueNode(root);
        }
    }

    for(int i = 0; i < (*root).children.size(); i++){
        buildTree((*root).children[i]);
    }
}

void printTree(Node *root, int depth){
    if((*root).type == 'a'){
        cout<<endl;
        for(int i = 0; i < depth; i++){
            cout<<"   ";
        }
        cout<<"<"<<atrs[(*root).atr].tag<<">"<<endl;
    }
    else{
        if((*root).type == 'v'){
            for(int i = 0; i < depth; i++){
                cout<<"   ";
            }
            if(discre.at((*root).atr)){
                if((*root).val == 0){
                    cout<<"<"<<atrs[(*root).atr].value_limits[0]<<":";
                }
                else{
                    if((*root).val==3){
                        cout<<">"<<atrs[(*root).atr].value_limits[2]<<":";
                    }
                    else{
                        cout<<atrs[(*root).atr].value_limits[(*root).val - 1]<<"<"<<
                        atrs[(*root).atr].value_limits[(*root).val]<<":";
                    }
                }
            }
            else{
                cout<<atrs[(*root).atr].values[(*root).val].tag<<":";
            }
        }
        else{
            if((*root).type == 'l'){
                cout<<(*root).clas;
            }
        }
    }
    for(int i = 0; i < (*root).children.size(); i++){
        printTree((*root).children[i], depth+1);
    }
}

bool isFloat( string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}


bool discretize(int index){
    //Estamos a trabalhar com vetores globais de atributes e examples

    int min = 1000000;
    int max = -100000;

    vector<Val> copy_values = atrs[index].values;

    for(int i=0; i<copy_values.size();i++){
        //cout<<"INDEX COUNT: "<<i<<endl;
        //cout<<"Content: "<<copy_values[i].tag<<endl;
        float value = std::stof(copy_values[i].tag);
        if(value<min){
            min=value;
        };
        if(max<value){
            max=value;
        };
    };
    float part = (max-min)/4.0;
    vector<float> limit;
    vector<string> values;
    limit.resize(3);
    values.resize(4);

    //tiramos os valores anteriores

    atrs[index].values.clear();
    atrs[index].values.resize(5);
    //limite guarda o valor maior admitido em cada intervalo
    for(int i=0;i<limit.size();i++){
        limit.at(i)=min+(i+1)*part;
        values.at(i)= std::to_string(min+i*part) + '-' + std::to_string(min+(i+1)*part);
        Val v(values.at(i),0);
        atrs[index].addValue(v);
    }


    for(int i=0;i<examples.size();i++){
        //queremos ter acceso ao tag do valor associado
        int e_index = examples[i].atr_val[index].second;
        //cout<<"index value "<<e_index<<" no ex "<<i<<endl;
        float val = std::stof(copy_values[e_index].tag);
        //cout<<"VAL: "<<val<<endl;
        if(val<limit[0]){
            examples[i].atr_val[index].second=0;
        }
        else if(val<limit[1]){
            examples[i].atr_val[index].second=1;
        }
        else if(val<limit[2]){
            examples[i].atr_val[index].second=2;
        }
        else{
            examples[i].atr_val[index].second=3;
        }
    }

    atrs[index].value_limits = limit;
    return true;

}


void giveTestClass(Node *root, int index){
    Example e = tests[index];
    tests[index].clas = "Uncertain.";
    if((*root).type == 'a'){
       for(int i = 0; i < (*root).children.size(); i++){
            if((*root).children[i]->val == e.atr_val[(*root).atr].second){
                giveTestClass((*root).children[i], index);
            }
        }
        //se chega aqui entao é porque nunca viu este valor antes
    }
    else{
        if((*root).type == 'v'){
            giveTestClass((*root).children[0], index); //nodes tipo v so podem ter um filho, ou um node folha ou um atributo
        }
        else{
            if((*root).type == 'l'){
                tests[index].clas = (*root).clas;
            }
        }
    }

}

void giveTestsClass(Node *root){
    for(int i = 0; i < tests.size(); i++){
        giveTestClass(root, i);
        cout<<tests[i].tag<<": "<<tests[i].clas;
        if(tests[i].clas == "Uncertain."){
            cout<<endl;
        }
    }
}

int main(int argc, char *argv[]){

    FILE* fp;

    char* line = NULL;
    vector<string> line_elements;
    size_t len = 0;
    ssize_t read;
    int n_atrs = 0;
    int n_examples=0;
    int n_tests = 0;
    vector<string> line_atributes;
    //vector<Example> examples;

    fp = fopen(argv[1], "r");
    if (!fp){
        printf("error: cannot open %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    //a primera linha serve só para contar;
    if(read = getline(&line, &len, fp) < 0){
        printf("error: cannot read first line\n");
        return EXIT_FAILURE;
    }

    line_atributes = split(line);

    n_atrs = line_atributes.size() - 2;

    Atribute a;
    atrs.resize(n_atrs);

    for(int i=0; i<n_atrs;i++){
        // a.setAtribute(line_atributes[i+1]);
        atrs[i].setAtribute(line_atributes[i+1]);
    }

    int line_number = 1;

    n_examples++;
    int atr = 0;
    int Value = 0;
    int v_index;
    pair<int,int> atr_val;

    discre.resize(atrs.size());

    read = getline(&line, &len, fp);
    line_elements = split(line);
    line_number++;
    n_examples;
    examples.resize(n_examples);

    examples[0].tag= line_elements[0];
    if(isFloat(line_elements[1])){
        discre.at(0)=true;
    }
    Val v(line_elements[1],1);
    atrs[0].addValue(v);
    atr_val.first = 0;
    atr_val.second = 0;
    examples[0].addAtribute(atr_val,0);

    for (int i=1; i<atrs.size(); i++){
        if(isFloat(line_elements[i])){
            discre.at(i-1)=true;
        }
        Val v(line_elements[i+1],1);
        atrs[i].addValue(v);
        atr_val.first= i;
        examples[0].addAtribute(atr_val,i);
    }
    examples[0].setClass(line_elements[line_elements.size()-1]);

    //printAtributes(atrs);


     while (read = getline(&line, &len, fp) >= 0){
         line_number++;
         n_examples++;
         examples.resize(n_examples);
         line_elements = split(line);
         examples[n_examples-1].tag = line_elements[0];

            for (int i=0; i<atrs.size(); i++){

                atr_val.first= i;
                v_index = ValueAlreadyRead(atrs,line_elements[i+1],i);
                if(v_index != -1){
                    atr_val.second= v_index;
                    atrs[i].values[v_index].counter++;
                } else {
                    Val v(line_elements[i+1],1);
                    atrs[i].addValue(v);
                    atr_val.second= atrs[i].n_values-1;
                }
                examples[n_examples-1].addAtribute(atr_val,i);

        }
         examples[n_examples-1].setClass(line_elements[line_elements.size()-1]);
     }


    for(int i = 0; i < atrs.size(); i++){
        if(discre.at(i)){
            //cout<<"CONSIDERING ATRIBUTE : "<<atrs[i].tag<<endl;
            discretize(i);
        }
    }

    vector<int> atr_index = index_vector(atrs.size());
    vector<int> ex_index = index_vector(examples.size());
    Node root = rootNode(atr_index, ex_index);

    buildTree(&root);
    printTree(&root, 0);
    printf("\n");

    fclose(fp);
    //ler o segundo ficheiro com os testes
    fp = fopen(argv[2], "r");
    if (!fp){
        printf("error: cannot open %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    if(read = getline(&line, &len, fp) < 0){
        printf("error: cannot read first line\n");
        return EXIT_FAILURE;
    }
    line_number = 1;
    //esta primeira linha vai so ter atributos que nos ja registamos

    float f;

    while (read = getline(&line, &len, fp) >= 0){
        line_number++;
        n_tests++;
        tests.resize(n_tests);
        line_elements = split(line);
        tests[n_tests-1].tag = line_elements[0];

            for (int i=0; i<atrs.size(); i++){
                atr_val.first= i;
                if(discre.at(i)){
                    f = std::stof(line_elements[i+1]);
                    if(f<atrs[i].value_limits[0]){
                        atr_val.second = 0;
                    }
                    else if(f<atrs[i].value_limits[1]){
                        atr_val.second = 1;
                    }
                    else if(f<atrs[i].value_limits[2]){
                        atr_val.second = 2;
                    }
                    else{
                        atr_val.second = 3;
                    }
                }
                else{
                    v_index = ValueAlreadyRead(atrs,line_elements[i+1],i);
                    atr_val.second= v_index; // IMPORTANT IF THE VALUE ON A TEST EXAMPLE IS -1 THEN THE VALUE DOES NOT APPEAR IN THE TRAINING SET
                    //cout<<" Atr "<<i<<" value "<<atrs[i].values[v_index].getTag()<<" actualizado a "<<atrs[i].values[v_index].counter;
                }
                tests[n_tests-1].addAtribute(atr_val,i);
        }
    }

    cout<<"Tests:"<<endl;
    giveTestsClass(&root);
return 0;
}
