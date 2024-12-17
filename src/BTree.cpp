#include "BTree.h"

template <class T, const unsigned int min_degree>
Btree<T, min_degree>::Btree(const string name, const string type, const unsigned int ver) : TypedFile<T, min_degree>(name, type, ver)
{ // crio a �rvore aqui
    // o construtor padrao da classe herdada "typedFile<T, min_degree>" ira abrir o arquivo
    record<T, min_degree> recRaiz;

    TypedFile<T, min_degree>::readRecord(recRaiz, 1);
    this->root = recRaiz.getData(); // carrego a raiz da memoria secundaria para a memoria principal
}

template <class T, const unsigned int min_degree>
Btree<T, min_degree>::~Btree()
{
    TypedFile<T, min_degree>::close();
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::printTreeB()
{
    this->root = this->diskReadNo(1);

    printByLevel(); // impressao por level
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::printByLevel()
{
    queue<pair<Node<T, min_degree>, int>> q; // queue � uma fila que possui o tipo de dado sendo pair (uma classe padrao do c++ que guarda ate 2 atributos, um dos atributos sera meu no, e o outro ser� meu level)

    // Enfileira a raiz com level 0
    q.push({this->root, 0});

    int currentLevel = 0;

    while (!q.empty())
    {
        auto [currentNode, level] = q.front();
        q.pop();

        // Se mudar de level, imprime uma nova linha
        if (level > currentLevel)
        {
            cout << endl;
            currentLevel = level;
        }

        // Imprime todas as chaves do n� atual
        cout << "[";
        for (int i = 0; i < currentNode.getN(); i++)
        {
            cout << currentNode.getKeys(i).getValue();
            if (i < currentNode.getN() - 1)
                cout << " ";
        }
        cout << "] ";

        // Se o n� n�o for folha, enfileira os filhos
        if (!currentNode.getLeaf())
        {
            for (int i = 0; i <= currentNode.getN(); i++)
            {
                long long int indexChild = currentNode.getChildren(i);
                Node<T, min_degree> childNode = diskReadNo(indexChild);
                q.push({childNode, level + 1});
            }
        }
    }
    cout << endl;
}

// recebe de entrada um n� raiz e uma chave k para procurar, retorna o n� e o indice que a chave esta no vector de filhos
template <class T, const unsigned int min_degree>
pair<Node<T, min_degree>, int> Btree<T, min_degree>::bTreeSearch(const Node<T, min_degree> &x, const T &k)
{
    int i = 0;

    while ((i < x.getN()) && (k > x.getKeys(i)))
    {
        i = i + 1;
    }
    if ((i < x.getN()) && (k == x.getKeys(i)))
    {
        return make_pair(x, i);
    }
    else if (x.getLeaf())
    {                                                // quando for folha
        return make_pair(Node<T, min_degree>(), -1); // retorna um n� padrao e um indice -1 indicando que � um n� folha e a chave nao existe
    }
    else
    {
        Node<T, min_degree> y;
        y = diskReadNo(x.children[i]); // novo no raiz ir� ser o filho x.children[i]
        return bTreeSearch(y, k);
    }
}
// indice do arquivo
// recebe como entrada o indice do n� que vai ser lido na memoria e retorna ele
template <class T, const unsigned int min_degree>
Node<T, min_degree> Btree<T, min_degree>::diskReadNo(const long long int i)
{
    record<T, min_degree> aux;
    TypedFile<T, min_degree>::readRecord(aux, i);
    return aux.getData();
}

// indice do vetor de filhos, e indice de x no arquivo
// recebe como entrada o n� pai e o indice do filho deste n�, o qual ser� splitado (o filho ser� o spltiado)
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeSplitChild(Node<T, min_degree> &x, const int i, long long int indexX)
{
    long long int indexY = x.getChildren(i);
    Node<T, min_degree> y = diskReadNo(indexY);

    Node<T, min_degree> z;
    z.setLeaf(y.getLeaf());
    z.setN(z.minKeys); // minKeys = menor quantidade de chaves que um n� pode ter

    for (int j = 0; j < z.minKeys; j++)
    {
        z.setKeys(y.getKeys(j + min_degree), j); // "-1" porque o vector come�a no �ndice 0
    }
    if (!y.getLeaf())
    {
        for (int j = 0; j < z.minChildrens; j++)
        {
            z.setChildren(y.getChildren(j + (min_degree)), j);
        }
    }
    y.setN(y.minKeys);

    for (int j = x.getN(); j >= (i + 1); j--)
    {                                           // x.getN()+1 � o ultimo filho de x, como o vector come�a em 0, entao basta x.getN()
        x.setChildren(x.getChildren(j), j + 1); // j+1 � o filho de x ao qual iremos deslocar (deslocar para direita os filhos de x), abrindo espa�o para z
    }

    long long int indexZ = TypedFile<T, min_degree>::insertNo(z); // metodo que insere z na memoria e retorna o indice em que z esta na memoria
    x.setChildren(indexZ, i + 1);

    for (int j = x.getN() - 1; j >= i; j--)
    {
        x.setKeys(x.getKeys(j), j + 1);
    }
    x.setKeys(y.getKeys(min_degree - 1), i); // min_degree-1 � a chave mediana de y
    x.setN(x.getN() + 1);

    TypedFile<T, min_degree>::writeNo(y, indexY);
    TypedFile<T, min_degree>::writeNo(x, indexX);
}
// indexX no arquivo
// esse metodo � o que percorre a arvore procurando onde inserir
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeInsertNonfull(Node<T, min_degree> x, T k, long long int indexX)
{
    int i = x.getN() - 1; //-1 porque o vector come�a em 0 // Come�ar pelo �ndice da �ltima chave
    if (x.getLeaf())
    {
        while ((i >= 0) && (k < x.getKeys(i)))
        { // encontra onde deve ser inserido a chave k no n� folha
            x.setKeys(x.getKeys(i), i + 1);
            i = i - 1;
        }
        x.setKeys(k, i + 1);
        x.setN(x.getN() + 1);

        TypedFile<T, min_degree>::writeNo(x, indexX);
    }
    else
    {
        while ((i >= 0) && (k < x.getKeys(i)))
        { // encontra o filho onde deve ser procurado onde inserir a chave k
            i = i - 1;
        }
        i = i + 1;
        long long int indexC = x.getChildren(i); // indexC � o filho que tentaremos inserir a chave k
        Node<T, min_degree> c = this->diskReadNo(indexC);

        if (c.getN() == c.maxKeys)
        {
            this->bTreeSplitChild(x, i, indexX);
            if (k > x.getKeys(i))
            { // determina qual dos dois filhos � agora o filho correto para o qual descer.
                i = i + 1;
            }
            indexC = x.getChildren(i);
            c = this->diskReadNo(indexC);
        }
        this->bTreeInsertNonfull(c, k, indexC);
    }
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeInsert(T k)
{
    this->root = this->diskReadNo(1);

    // se a raiz estiver cheia faz este if
    if (this->root.getN() == this->root.maxKeys)
    { // a arvore b so aumenta seu tamanho neste momento
        Node<T, min_degree> r;
        r = this->root;
        long long int indexR = TypedFile<T, min_degree>::insertNo(r);

        Node<T, min_degree> s; // um n� vazio � criado e atribuido a raiz, entao os filhos e as chaves da raiz ser�o zeradas
        this->root = s;
        this->root.setLeaf(false);
        this->root.setN(0);
        this->root.setChildren(indexR, 0);
        bTreeSplitChild(this->root, 0, 1);
        bTreeInsertNonfull(this->root, k, 1); // 1 � o indice da raiz no arquivo
    }
    else
    {
        bTreeInsertNonfull(this->root, k, 1);
    }
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeDelete(T k)
{
    this->root = this->diskReadNo(1);
    bTreeRemove(this->root, k, 1);
}

template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::bTreeRemove(Node<T, min_degree> x, T k, long long indexX)
{
    int i = 0;
    bool present;

    while (i < x.getN() && k > x.getKeys(i))
    { // encontro a posicao da chave k
        i = i + 1;
    }
    if ((i < x.getN()) && k == x.getKeys(i))
    {                   // verifico se est� no n� atual
        present = true; // se esta presente, i � o indice onde se encontra a chave k
    }
    else
    {
        present = false; // se nao estiver presente, o indice i vira o indice do filho de x que iremos acessar
    }
    // CASO 1: x � uma folha e a chave esta presente nele
    if (x.getLeaf() && present)
    { // esta presente no n� folha
        while (i < x.getN() - 1)
        { // Desloca as chaves para preencher o espa�o da chave removida //nao precisa fazer nada com o ultimo valor, por isso t� x.getN()-1
            x.setKeys(x.getKeys(i + 1), i);
            i = i + 1;
        }
        x.setN(x.getN() - 1);
        TypedFile<T, min_degree>::writeNo(x, indexX);
    }
    else if (x.getLeaf() && !present)
    {
        cout << "Esse valor nao existe na arvore." << endl;
    }
    // fim caso 1

    // CASO 2: � um no interno e a chave k esta presente neste n� (N�o � folha e esta presente no n� x)
    else if (!x.getLeaf() && present)
    {
        int indexY = x.getChildren(i);     // filho a esquerda
        int indexZ = x.getChildren(i + 1); // filho a direita

        Node<T, min_degree> y = diskReadNo(indexY);
        Node<T, min_degree> z = diskReadNo(indexZ);

        // CASO 2.a.
        if (y.getN() >= y.minKeys + 1)
        { // y.minKeys + 1 == min_degree
            T kLine = getPredecessor(y);
            x.setKeys(kLine, i); // substitui k por k� em x.
            TypedFile<T, min_degree>::writeNo(x, indexX);
            bTreeRemove(y, kLine, indexY);
            // CASO 2.b.
        }
        else if (z.getN() >= z.minKeys + 1)
        { // y.minKeys + 1 == min_degree
            T kLine = getSucessor(z);
            x.setKeys(kLine, i); // substitui k por k� em x.
            TypedFile<T, min_degree>::writeNo(x, indexX);
            bTreeRemove(z, kLine, indexZ);
            // CASO 2.c.
        }
        else
        {
            mergeNodes(x, indexX, i, y, indexY, z, indexZ); // i � o indice em que a chave k esta em x
            bTreeRemove(y, k, indexY);
        }
        // fim do caso 2

        // CASO 3: Se a chave k n�o estiver presente no n� interno x
    }
    else if ((!x.leaf) && !present)
    {
        // Determino o filho de x onde k esta
        int indexC = x.getChildren(i);
        Node<T, min_degree> c = this->diskReadNo(indexC);

        // Verifica os irm�os
        long long indexChildrenLeft = (i > 0) ? x.getChildren(i - 1) : -1;         //-1 se nao tiver n� a esquerda
        long long indexChildrenRight = (i < x.getN()) ? x.getChildren(i + 1) : -1; //-1 se nao tiver n� a direita

        Node<T, min_degree> childrenLeft;
        Node<T, min_degree> childrenRight;

        if (indexChildrenLeft != -1)
        {
            childrenLeft = this->diskReadNo(indexChildrenLeft);
        }

        if (indexChildrenRight != -1)
        {
            childrenRight = this->diskReadNo(indexChildrenRight);
        }

        if (c.getN() == c.minKeys)
        {
            bool borrow = false;
            if (i == x.getN())
            { // Filho mais � direita
                if (indexChildrenLeft != -1)
                {
                    borrow = this->borrowFromLeft(x, indexX, i, c, indexC, childrenLeft, indexChildrenLeft);
                }
                if (!borrow)
                {
                    mergeNodes(x, indexX, i - 1, c, indexC, childrenLeft, indexChildrenLeft);
                }
            }
            else if (i == 0)
            { // Filho mais � esquerda
                if (indexChildrenRight != -1)
                {
                    borrow = this->borrowFromRight(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
                if (!borrow)
                {
                    mergeNodes(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
            }
            else
            {
                if (indexChildrenLeft != -1)
                {
                    borrow = this->borrowFromLeft(x, indexX, i, c, indexC, childrenLeft, indexChildrenLeft);
                }
                if (!borrow && indexChildrenRight != -1)
                {
                    borrow = this->borrowFromRight(x, indexX, i, c, indexC, childrenRight, indexChildrenRight);
                }
                if (!borrow && indexChildrenLeft != -1)
                {
                    mergeNodes(x, indexX, i - 1, c, indexC, childrenLeft, indexChildrenLeft);
                }
            }
        }
        bTreeRemove(c, k, indexC);
    }
}

template <class T, const unsigned int min_degree>
T Btree<T, min_degree>::getPredecessor(Node<T, min_degree> y)
{
    while (!y.getLeaf())
    {
        int rightMostIndex = y.getChildren(y.getN()); // o filho mais a direita do n� y
        y = this->diskReadNo(rightMostIndex);
    }
    return y.getKeys(y.getN() - 1); // chave kLinha
}

template <class T, const unsigned int min_degree>
T Btree<T, min_degree>::getSucessor(Node<T, min_degree> z)
{
    while (!z.getLeaf())
    {
        int leftMostIndex = z.getChildren(0); // o filho mais a esquerda do n� z
        z = this->diskReadNo(leftMostIndex);
    }
    return z.getKeys(0); // chave kLinha
}

// i aqui � o indice da chave que queremos remover/fundir
template <class T, const unsigned int min_degree>
void Btree<T, min_degree>::mergeNodes(Node<T, min_degree> &x, int indexX, int i, Node<T, min_degree> &y, int &indexY, Node<T, min_degree> &z, int indexZ)
{
    // transferindo k do n� x, para y
    y.setKeys(x.getKeys(i), y.minKeys); // y.minkeys == neste caso � o indice do meio / o indice ap�s a �ltima chave de y

    // Transferir todas as chaves de z para y (z neste momento possui T - 1 chaves == minKeys)
    int idx = 0;
    while (idx < z.getN())
    {
        y.setKeys(z.getKeys(idx), min_degree + idx); // min_degree == neste caso � a chave depois do meio
        idx++;
    }

    // Transferir todos os filhos de z para y, se n�o for folha
    int indF = 0;
    if (!z.getLeaf())
    {
        while (indF <= z.getN())
        { // x.getN() neste caso � o ultimo filho
            // x perde o ponteiro para z
            y.setChildren(z.getChildren(indF), min_degree + indF); // min_degree � o filho a direita da chave que esta no meio
            indF++;
        }
    }

    // atualiza o numero de chaves de y
    y.setN(y.maxKeys);

    // x perde k
    int idxK = i;
    while (idxK < x.getN() - 1)
    { // x.getN() - 1 neste caso � a ultima chave, ou seja, nao fazer isso para a ultima chave (nao mover a proxima chave para a ultima chave)
        // x perde k
        x.setKeys(x.getKeys(idxK + 1), idxK);
        idxK++;
    }

    // x perde ponteiro para z (perde o filho z)
    indF = i + 1; // i+1 � o indice do filho Z que devo apagar de x
    while (indF < x.getN())
    { // x.getN() neste caso � o ultimo filho, ou seja, nao fazer isso para o ultimo filho (nao mover o proximo filho para o ultimo filho)
        // x perde o ponteiro para z
        x.setChildren(x.getChildren(indF + 1), indF);
        indF++;
    }

    x.setN(x.getN() - 1);

    // atualizando valores na memoria secundaria

    TypedFile<T, min_degree>::deleteRecord(indexZ);

    // diminuindo a altura da arvore caso a raiz fique com 0 chaves
    if (x.getN() == 0 && indexX == 1 && !x.getLeaf())
    { // fa�o o teste !x.getLeaf() para que a raiz nunca seja apagada da memoria

        TypedFile<T, min_degree>::writeNo(y, 1);        // y vira raiz, escrevendo seu valor sobre a raiz anterior presente na memoria, recebendo entao uma nova posicao na memoria
        TypedFile<T, min_degree>::deleteRecord(indexY); // desaloca y da posicao que ele estava antes na memoria pois agora ele tem outra posicao na memoria (ele � raiz)
        indexY = 1;                                     // esse � o novo indexY, novo indice do y na memoria (vira a raiz)
    }
    else
    {
        TypedFile<T, min_degree>::writeNo(x, indexX);
        TypedFile<T, min_degree>::writeNo(y, indexY);
    }
}

// i � o index do filho x.c no vetor de filhos de x
template <class T, const unsigned int min_degree>
bool Btree<T, min_degree>::borrowFromLeft(Node<T, min_degree> &x, long long int indexX, long long int i, Node<T, min_degree> &c, long long int indexC, Node<T, min_degree> &childrenLeft, long long int indexChildrenLeft)
{
    if (childrenLeft.getN() > childrenLeft.minKeys)
    { // se tiver mais que o minimo de chaves
        // armazena a ultima chave de childrenLeft e "apaga" ela do vetor de chaves
        T lastKeyChildrenLeft = childrenLeft.getKeys(childrenLeft.getN() - 1); // childrenLeft.getN()-1 � o indice da �ltima chave no vetor de chaves
        long long int lastChildrenOfTheLeft;
        if (!childrenLeft.getLeaf())
        {
            lastChildrenOfTheLeft = childrenLeft.getChildren(childrenLeft.getN()); // childrenLeft.getN() � a posi��o do ultimo filho
        }
        childrenLeft.setN(childrenLeft.getN() - 1); //"apaga" a ultima chave, assim nao mais teremos acesso a ela neste n�

        T keyOfTheX = x.getKeys(i - 1);        // pego a chave q vou mover para o filho x.c e salvo na variavel
        x.setKeys(lastKeyChildrenLeft, i - 1); // insere a chave em x

        // realoco as chaves de x.c para a direita, para poder inserir a chave vinda de x
        int lastIndexKey = c.getN() - 1;
        while (lastIndexKey >= 0)
        {
            c.setKeys(c.getKeys(lastIndexKey), lastIndexKey + 1);
            lastIndexKey--;
        }
        c.setKeys(keyOfTheX, 0);

        // se tem filhos, tambem realoco os filhos
        if (!c.getLeaf())
        {
            int lastIndexChildren = c.getN();
            while (lastIndexChildren >= i - 1)
            {
                c.setChildren(c.getChildren(lastIndexChildren), lastIndexChildren + 1);
                lastIndexChildren--;
            }
            c.setChildren(lastChildrenOfTheLeft, 0);
        }

        c.setN(c.getN() + 1);

        TypedFile<T, min_degree>::writeNo(x, indexX);
        TypedFile<T, min_degree>::writeNo(c, indexC);
        TypedFile<T, min_degree>::writeNo(childrenLeft, indexChildrenLeft);
        return true;
    }
    return false;
}

template <class T, const unsigned int min_degree>
bool Btree<T, min_degree>::borrowFromRight(Node<T, min_degree> &x, long long int indexX, long long int i, Node<T, min_degree> &c, long long int indexC, Node<T, min_degree> &childrenRight, long long int indexChildrenRight)
{
    if (childrenRight.getN() > childrenRight.minKeys)
    { // se tiver mais que o minimo de chaves
        // armazena a primeira chave de childrenRight e "apaga" ela do vetor de chaves
        T lastKeyChildrenRight = childrenRight.getKeys(0);
        long long int lastChildrenOfTheRight;
        if (!childrenRight.getLeaf())
        {
            lastChildrenOfTheRight = childrenRight.getChildren(0);
        }

        // realoco as chaves do irmao para a esquerda, para poder "deletar" a chave que ir� subir para x
        int cont = 0;
        while (cont <= childrenRight.getN() - 2)
        { // childrenRight.getN()-2 � igual ao penultimo valor do vetor de chaves, pois nao precisa/(n�o pode) fazer para a ultima chave
            childrenRight.setKeys(childrenRight.getKeys(cont + 1), cont);
            cont++;
        }

        // se tem filhos, tambem realoco os filhos
        cont = 0;
        if (!childrenRight.getLeaf())
        {
            while (cont <= childrenRight.getN() - 1)
            { // childrenRight.getN()-1 � igual ao penultimo valor do vetor de filhos, pois nao precisa/(n�o pode) fazer para o ultimo filho
                childrenRight.setChildren(childrenRight.getChildren(cont + 1), cont);
                cont++;
            }
        }
        childrenRight.setN(childrenRight.getN() - 1); // decrementa a quantidade de chaves no n� irmao

        T keyOfTheX = x.getKeys(i);         // pego a chave q vou mover para o filho x.c e salvo na variavel
        x.setKeys(lastKeyChildrenRight, i); // insere a chave em x

        // insiro a chave vinda de x em x.c, sendo esta a ultima chave a partir de agora
        c.setKeys(keyOfTheX, c.getN());

        // se tem filhos, eu fa�o a conec��o do filho da chave que foi emprestado
        if (!c.getLeaf())
        {
            c.setChildren(lastChildrenOfTheRight, c.getN() + 1);
        }

        c.setN(c.getN() + 1);

        TypedFile<T, min_degree>::writeNo(x, indexX);
        TypedFile<T, min_degree>::writeNo(c, indexC);
        TypedFile<T, min_degree>::writeNo(childrenRight, indexChildrenRight);
        return true;
    }
    return false;
}