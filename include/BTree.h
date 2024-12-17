#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED
#include <queue>
#include <utility>
#include "node.h"
#include "typedfile.h"

using namespace std;

template <class T, const unsigned int min_degree>
class Btree : public typedFile<T, min_degree>
{
public:
    Btree(const string name, const string type, const unsigned int ver);
    virtual ~Btree();

    void printTreeB();
    void printByLevel();

    pair<node<T, min_degree>, int> bTreeSearch(const node<T, min_degree>& x, const T& k);
    node<T, min_degree> diskReadNo(const long long int i);

    void bTreeSplitChild(node<T, min_degree>& x,const int i, long long int indexX);
    void bTreeInsertNonfull(node<T, min_degree> x, T k, long long int indexX);
    void bTreeInsert(T k);

    void bTreeDelete(T k);
    void bTreeRemove(node<T, min_degree> x, T k, long long indexX);
    T getPredecessor(node<T, min_degree> y);
    T getSucessor(node<T, min_degree> z);
    void mergeNodes(node<T, min_degree>& x, int indexX, int i, node<T, min_degree>& y, int& indexY, node<T, min_degree>& z, int indexZ);
    bool borrowFromLeft(node<T, min_degree>& x, long long int indexX, long long int i, node<T, min_degree>& c, long long int indexC, node<T, min_degree>& childrenLeft, long long int indexChildrenLeft);
    bool borrowFromRight(node<T, min_degree>& x, long long int indexX, long long int i, node<T, min_degree>& c, long long int indexC, node<T, min_degree>& childrenRight, long long int indexChildrenRight);
private:
    node<T, min_degree> root;    //a raiz da B-árvore está sempre na memória principal
};

template <class T, const unsigned int min_degree>
Btree<T, min_degree>::Btree(const string name, const string type, const unsigned int ver):typedFile<T, min_degree>(name,type,ver){          //crio a árvore aqui
    //o construtor padrao da classe herdada "typedFile<T, min_degree>" ira abrir o arquivo
    record<T, min_degree> recRaiz;

    typedFile<T, min_degree>::readRecord(recRaiz, 1);
    this->root = recRaiz.getData();    //carrego a raiz da memoria secundaria para a memoria principal
}

template <class T, const unsigned int min_degree>
Btree<T, min_degree>::~Btree(){
    typedFile<T, min_degree>::close();
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::printTreeB() {
    this->root = this->diskReadNo(1);

    printByLevel(); //impressao por level
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::printByLevel() {
    queue<pair<node<T, min_degree>, int>> q; //queue é uma fila que possui o tipo de dado sendo pair (uma classe padrao do c++ que guarda ate 2 atributos, um dos atributos sera meu no, e o outro será meu level)

    //Enfileira a raiz com level 0
    q.push({this->root, 0});

    int currentLevel = 0;

    while (!q.empty()) {
        auto [currentNode, level] = q.front();
        q.pop();

        // Se mudar de level, imprime uma nova linha
        if (level > currentLevel) {
            cout << endl;
            currentLevel = level;
        }

        // Imprime todas as chaves do nó atual
        cout << "[";
        for (int i = 0; i < currentNode.getN(); i++) {
            cout << currentNode.getKeys(i).getValue();
            if (i < currentNode.getN() - 1) cout << " ";
        }
        cout << "] ";

        // Se o nó não for folha, enfileira os filhos
        if (!currentNode.getLeaf()) {
            for (int i = 0; i <= currentNode.getN(); i++) {
                long long int indexChild = currentNode.getChildren(i);
                node<T, min_degree> childNode = diskReadNo(indexChild);
                q.push({childNode, level + 1});
            }
        }
    }
    cout << endl;
}


//recebe de entrada um nó raiz e uma chave k para procurar, retorna o nó e o indice que a chave esta no vector de filhos
template <class T, const unsigned int min_degree>
pair<node<T, min_degree>, int> Btree<T, min_degree>::bTreeSearch(const node<T, min_degree>& x, const T& k){
    int i = 0;

    while ( (i < x.getN()) && (k > x.getKeys(i))){
        i  = i + 1;
    }
    if((i < x.getN()) && (k == x.getKeys(i))){
        return make_pair(x,i);
    }
    else if(x.getLeaf()){  //quando for folha
            return make_pair(node<T, min_degree>(), -1);  //retorna um nó padrao e um indice -1 indicando que é um nó folha e a chave nao existe
       }else {
            node<T, min_degree> y;
            y = diskReadNo(x.children[i]);  //novo no raiz irá ser o filho x.children[i]
            return bTreeSearch(y, k);
       }
}
//indice do arquivo
//recebe como entrada o indice do nó que vai ser lido na memoria e retorna ele
template <class T, const unsigned int min_degree>
node<T, min_degree> Btree<T, min_degree>::diskReadNo(const long long int i){
    record<T, min_degree> aux;
    typedFile<T, min_degree>::readRecord(aux,i);
    return aux.getData();
}

//indice do vetor de filhos, e indice de x no arquivo
//recebe como entrada o nó pai e o indice do filho deste nó, o qual será splitado (o filho será o spltiado)
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeSplitChild(node<T, min_degree>& x, const int i, long long int indexX){
    long long int indexY = x.getChildren(i);
    node<T, min_degree> y = diskReadNo(indexY);

    node<T, min_degree> z;
    z.setLeaf(y.getLeaf());
    z.setN(z.minKeys);        //minKeys = menor quantidade de chaves que um nó pode ter

    for(int j = 0; j < z.minKeys; j++){
        z.setKeys(y.getKeys(j + min_degree), j);  // "-1" porque o vector começa no índice 0
    }
    if(!y.getLeaf()){
        for(int j = 0; j < z.minChildrens; j++){
            z.setChildren(y.getChildren(j +(min_degree)), j);
        }
    }
    y.setN(y.minKeys);

    for(int j = x.getN(); j >= (i+1); j--){   //x.getN()+1 é o ultimo filho de x, como o vector começa em 0, entao basta x.getN()
        x.setChildren(x.getChildren(j), j+1);   //j+1 é o filho de x ao qual iremos deslocar (deslocar para direita os filhos de x), abrindo espaço para z
    }

    long long int indexZ = typedFile<T, min_degree>::insertNo(z);  //metodo que insere z na memoria e retorna o indice em que z esta na memoria
    x.setChildren(indexZ, i +1);

    for(int j = x.getN()-1; j >= i; j--){
        x.setKeys(x.getKeys(j), j+1);
    }
    x.setKeys(y.getKeys(min_degree-1), i);   //min_degree-1 é a chave mediana de y
    x.setN(x.getN()+1);

    typedFile<T, min_degree>::writeNo(y, indexY);
    typedFile<T, min_degree>::writeNo(x,indexX);
}
//indexX no arquivo
//esse metodo é o que percorre a arvore procurando onde inserir
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeInsertNonfull(node<T, min_degree> x, T k, long long int indexX){
    int i = x.getN() -1;   //-1 porque o vector começa em 0 // Começar pelo índice da última chave
    if(x.getLeaf()){
        while((i >= 0) && (k < x.getKeys(i))){ //encontra onde deve ser inserido a chave k no nó folha
            x.setKeys( x.getKeys(i), i+1);
            i = i -1;
        }
        x.setKeys(k, i+1);
        x.setN(x.getN() +1);

        typedFile<T, min_degree>::writeNo(x, indexX);
    }else {
        while((i >= 0) && (k < x.getKeys(i))){ //encontra o filho onde deve ser procurado onde inserir a chave k
            i = i - 1;
        }
        i = i + 1;
        long long int indexC = x.getChildren(i);   //indexC é o filho que tentaremos inserir a chave k
        node<T, min_degree> c = this->diskReadNo(indexC);

        if(c.getN() == c.maxKeys){
            this->bTreeSplitChild(x, i, indexX);
            if(k > x.getKeys(i)){ //determina qual dos dois filhos é agora o filho correto para o qual descer.
                i = i+1;
            }
        indexC = x.getChildren(i);
        c = this->diskReadNo(indexC);
        }
        this->bTreeInsertNonfull(c, k, indexC);
    }
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeInsert(T k){
    this->root = this->diskReadNo(1);

    //se a raiz estiver cheia faz este if
    if(this->root.getN() == this->root.maxKeys){ //a arvore b so aumenta seu tamanho neste momento
        node<T, min_degree> r;
        r = this->root;
        long long int indexR = typedFile<T, min_degree>::insertNo(r);

        node<T, min_degree> s;  //um nó vazio é criado e atribuido a raiz, entao os filhos e as chaves da raiz serão zeradas
        this->root = s;
        this->root.setLeaf(false);
        this->root.setN(0);
        this->root.setChildren(indexR, 0);
        bTreeSplitChild(this->root, 0, 1);
        bTreeInsertNonfull(this->root, k, 1); //1 é o indice da raiz no arquivo
    }else{
        bTreeInsertNonfull(this->root, k, 1);
    }
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeDelete(T k){
    this->root = this->diskReadNo(1);
    bTreeRemove(this->root, k, 1);
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeRemove(node<T, min_degree> x, T k, long long indexX){
    int i = 0;
    bool present;

    while(i < x.getN() && k > x.getKeys(i)){ //encontro a posicao da chave k
            i = i + 1;
        }
        if((i < x.getN()) && k == x.getKeys(i)){ //verifico se está no nó atual
            present = true;    //se esta presente, i é o indice onde se encontra a chave k
        }else{
            present = false;  //se nao estiver presente, o indice i vira o indice do filho de x que iremos acessar
        }
    //CASO 1: x é uma folha e a chave esta presente nele
    if(x.getLeaf() && present){ //esta presente no nó folha
        while(i < x.getN()-1){ // Desloca as chaves para preencher o espaço da chave removida //nao precisa fazer nada com o ultimo valor, por isso tá x.getN()-1
            x.setKeys( x.getKeys(i+1), i);
            i = i + 1;
        }
        x.setN(x.getN() -1);
        typedFile<T, min_degree>::writeNo(x, indexX);

    }else if (x.getLeaf() && !present){
        cout << "Esse valor nao existe na arvore."<<endl;
    }
    //fim caso 1

    //CASO 2: É um no interno e a chave k esta presente neste nó (Não é folha e esta presente no nó x)
    else if(!x.getLeaf() && present){
        int indexY = x.getChildren(i);  //filho a esquerda
        int indexZ = x.getChildren(i+1);//filho a direita

        node<T, min_degree> y = diskReadNo(indexY);
        node<T, min_degree> z = diskReadNo(indexZ);

        //CASO 2.a.
        if(y.getN() >= y.minKeys + 1){  //y.minKeys + 1 == min_degree
            T kLine = getPredecessor(y);
            x.setKeys(kLine, i);   //substitui k por k’ em x.
            typedFile<T, min_degree>::writeNo(x, indexX);
            bTreeRemove(y, kLine, indexY);
        //CASO 2.b.
        }else if(z.getN() >= z.minKeys + 1){    //y.minKeys + 1 == min_degree
            T kLine = getSucessor(z);
            x.setKeys(kLine, i);   //substitui k por k’ em x.
            typedFile<T, min_degree>::writeNo(x, indexX);
            bTreeRemove(z, kLine, indexZ);
        //CASO 2.c.
        }else{
            mergeNodes(x, indexX,  i, y, indexY, z, indexZ);    //i é o indice em que a chave k esta em x
            bTreeRemove(y, k, indexY);
        }
    //fim do caso 2

    //CASO 3: Se a chave k não estiver presente no nó interno x
    }else if((!x.leaf) && !present){
        //Determino o filho de x onde k esta
        int indexC = x.getChildren(i);
        node<T, min_degree> c = this->diskReadNo(indexC);

        // Verifica os irmãos
        long long indexChildrenLeft = (i > 0) ? x.getChildren(i - 1) : -1;    //-1 se nao tiver nó a esquerda
        long long indexChildrenRight = (i < x.getN()) ? x.getChildren(i + 1) : -1;  //-1 se nao tiver nó a direita

        node<T, min_degree> childrenLeft;
        node<T, min_degree> childrenRight;

        if (indexChildrenLeft != -1) {
            childrenLeft = this->diskReadNo(indexChildrenLeft);
        }

        if (indexChildrenRight != -1) {
            childrenRight = this->diskReadNo(indexChildrenRight);
        }


         if (c.getN() == c.minKeys) {
            bool borrow = false;
            if (i == x.getN()) { // Filho mais à direita
                if (indexChildrenLeft != -1) {
                    borrow = this->borrowFromLeft(x, indexX, i, c, indexC, childrenLeft, indexChildrenLeft);
                }
                if (!borrow) {
                    mergeNodes(x, indexX, i - 1, c, indexC, childrenLeft, indexChildrenLeft);
                }
            } else if (i == 0) { // Filho mais à esquerda
                if (indexChildrenRight != -1) {
                    borrow = this->borrowFromRight(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
                if (!borrow) {
                    mergeNodes(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
            } else {
                if (indexChildrenLeft != -1) {
                    borrow = this->borrowFromLeft(x, indexX, i, c, indexC, childrenLeft, indexChildrenLeft);
                }
                if (!borrow && indexChildrenRight != -1)  {
                    borrow = this->borrowFromRight(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
                if (!borrow && indexChildrenLeft != -1) {
                    mergeNodes(x, indexX, i - 1, c, indexC, childrenLeft, indexChildrenLeft);
                }
            }
        }
        bTreeRemove(c, k, indexC);
    }
}

template <class T, const unsigned int min_degree>
T Btree<T, min_degree>::getPredecessor(node<T, min_degree> y) {
    while (!y.getLeaf()) {
        int rightMostIndex = y.getChildren(y.getN());  //o filho mais a direita do nó y
        y = this->diskReadNo(rightMostIndex);
    }
    return y.getKeys(y.getN() - 1); //chave kLinha
}

template <class T, const unsigned int min_degree>
T Btree<T, min_degree>::getSucessor(node<T, min_degree> z) {
    while (!z.getLeaf()) {
        int leftMostIndex = z.getChildren(0);  //o filho mais a esquerda do nó z
        z = this->diskReadNo(leftMostIndex);
    }
    return z.getKeys(0); //chave kLinha
}

//i aqui é o indice da chave que queremos remover/fundir
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::mergeNodes(node<T, min_degree>& x, int indexX, int i, node<T, min_degree>& y, int& indexY, node<T, min_degree>& z, int indexZ) {
    //transferindo k do nó x, para y
    y.setKeys(x.getKeys(i), y.minKeys); //y.minkeys == neste caso é o indice do meio / o indice após a última chave de y

    //Transferir todas as chaves de z para y (z neste momento possui T - 1 chaves == minKeys)
    int idx = 0;
    while(idx < z.getN()){
        y.setKeys(z.getKeys(idx), min_degree + idx); //min_degree == neste caso é a chave depois do meio
        idx++;
    }

    // Transferir todos os filhos de z para y, se não for folha
    int indF = 0;
    if(!z.getLeaf()){
        while(indF <= z.getN()){ //x.getN() neste caso é o ultimo filho
            //x perde o ponteiro para z
            y.setChildren(z.getChildren(indF), min_degree + indF); //min_degree é o filho a direita da chave que esta no meio
            indF++;
        }
    }

    //atualiza o numero de chaves de y
    y.setN(y.maxKeys);

    //x perde k
    int idxK = i;
    while(idxK < x.getN() - 1){ //x.getN() - 1 neste caso é a ultima chave, ou seja, nao fazer isso para a ultima chave (nao mover a proxima chave para a ultima chave)
        //x perde k
        x.setKeys(x.getKeys(idxK + 1), idxK);
        idxK++;
    }

    //x perde ponteiro para z (perde o filho z)
    indF = i+1; //i+1 é o indice do filho Z que devo apagar de x
    while(indF < x.getN()){ //x.getN() neste caso é o ultimo filho, ou seja, nao fazer isso para o ultimo filho (nao mover o proximo filho para o ultimo filho)
        //x perde o ponteiro para z
        x.setChildren(x.getChildren(indF + 1), indF);
        indF++;
    }

    x.setN(x.getN() - 1);

    //atualizando valores na memoria secundaria

    typedFile<T, min_degree>::deleteRecord(indexZ);

    //diminuindo a altura da arvore caso a raiz fique com 0 chaves
    if(x.getN() == 0 && indexX == 1 && !x.getLeaf()){   //faço o teste !x.getLeaf() para que a raiz nunca seja apagada da memoria

        typedFile<T, min_degree>::writeNo(y, 1);          //y vira raiz, escrevendo seu valor sobre a raiz anterior presente na memoria, recebendo entao uma nova posicao na memoria
        typedFile<T, min_degree>::deleteRecord(indexY);   //desaloca y da posicao que ele estava antes na memoria pois agora ele tem outra posicao na memoria (ele é raiz)
        indexY = 1;    //esse é o novo indexY, novo indice do y na memoria (vira a raiz)
    }else{
        typedFile<T, min_degree>::writeNo(x, indexX);
        typedFile<T, min_degree>::writeNo(y, indexY);
    }
}

//i é o index do filho x.c no vetor de filhos de x
template <class T, const unsigned int min_degree>
bool Btree<T, min_degree>::borrowFromLeft(node<T, min_degree>& x, long long int indexX, long long int i, node<T, min_degree>& c, long long int indexC, node<T, min_degree>& childrenLeft, long long int indexChildrenLeft){
    if(childrenLeft.getN() > childrenLeft.minKeys){ //se tiver mais que o minimo de chaves
        //armazena a ultima chave de childrenLeft e "apaga" ela do vetor de chaves
        T lastKeyChildrenLeft = childrenLeft.getKeys(childrenLeft.getN()-1);    //childrenLeft.getN()-1 é o indice da última chave no vetor de chaves
        long long int lastChildrenOfTheLeft;
        if(!childrenLeft.getLeaf()){
            lastChildrenOfTheLeft = childrenLeft.getChildren(childrenLeft.getN());  //childrenLeft.getN() é a posição do ultimo filho
        }
        childrenLeft.setN(childrenLeft.getN() - 1); //"apaga" a ultima chave, assim nao mais teremos acesso a ela neste nó

        T keyOfTheX = x.getKeys(i-1);  //pego a chave q vou mover para o filho x.c e salvo na variavel
        x.setKeys(lastKeyChildrenLeft, i - 1);  //insere a chave em x

        //realoco as chaves de x.c para a direita, para poder inserir a chave vinda de x
        int lastIndexKey = c.getN() -1;
        while(lastIndexKey >= 0){
            c.setKeys(c.getKeys(lastIndexKey), lastIndexKey + 1);
            lastIndexKey--;
        }
        c.setKeys(keyOfTheX, 0);

        //se tem filhos, tambem realoco os filhos
        if(!c.getLeaf()){
            int lastIndexChildren = c.getN();
            while(lastIndexChildren >= i-1){
                c.setChildren(c.getChildren(lastIndexChildren), lastIndexChildren+1);
                lastIndexChildren--;
            }
            c.setChildren(lastChildrenOfTheLeft, 0);
        }

        c.setN(c.getN()+1);

        typedFile<T, min_degree>::writeNo(x, indexX);
        typedFile<T, min_degree>::writeNo(c, indexC);
        typedFile<T, min_degree>::writeNo(childrenLeft, indexChildrenLeft);
        return true;
    }
    return false;
}

template <class T, const unsigned int min_degree>
bool Btree<T, min_degree>::borrowFromRight(node<T, min_degree>& x, long long int indexX, long long int i, node<T, min_degree>& c, long long int indexC, node<T, min_degree>& childrenRight, long long int indexChildrenRight){
    if(childrenRight.getN() > childrenRight.minKeys){ //se tiver mais que o minimo de chaves
        //armazena a primeira chave de childrenRight e "apaga" ela do vetor de chaves
        T lastKeyChildrenRight = childrenRight.getKeys(0);
        long long int lastChildrenOfTheRight;
        if(!childrenRight.getLeaf()){
            lastChildrenOfTheRight = childrenRight.getChildren(0);
        }

        //realoco as chaves do irmao para a esquerda, para poder "deletar" a chave que irá subir para x
        int cont = 0;
        while(cont <= childrenRight.getN()-2){ //childrenRight.getN()-2 é igual ao penultimo valor do vetor de chaves, pois nao precisa/(não pode) fazer para a ultima chave
            childrenRight.setKeys(childrenRight.getKeys(cont + 1), cont);
            cont++;
        }

        //se tem filhos, tambem realoco os filhos
        cont = 0;
        if(!childrenRight.getLeaf()){
            while(cont <= childrenRight.getN() - 1){//childrenRight.getN()-1 é igual ao penultimo valor do vetor de filhos, pois nao precisa/(não pode) fazer para o ultimo filho
                childrenRight.setChildren(childrenRight.getChildren(cont + 1), cont);
                cont++;
            }
        }
        childrenRight.setN(childrenRight.getN() - 1); //decrementa a quantidade de chaves no nó irmao

        T keyOfTheX = x.getKeys(i);  //pego a chave q vou mover para o filho x.c e salvo na variavel
        x.setKeys(lastKeyChildrenRight, i);  //insere a chave em x

        //insiro a chave vinda de x em x.c, sendo esta a ultima chave a partir de agora
        c.setKeys(keyOfTheX, c.getN());

        //se tem filhos, eu faço a conecção do filho da chave que foi emprestado
        if(!c.getLeaf()){
            c.setChildren(lastChildrenOfTheRight, c.getN()+1);
        }

        c.setN(c.getN()+1);

        typedFile<T, min_degree>::writeNo(x, indexX);
        typedFile<T, min_degree>::writeNo(c, indexC);
        typedFile<T, min_degree>::writeNo(childrenRight, indexChildrenRight);
        return true;
    }
    return false;
}

#endif // BTREE_H_INCLUDED
