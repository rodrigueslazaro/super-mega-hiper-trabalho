#include "TypedFile.h"

/*
template <class T, const unsigned int min_degree>
typedFile<T, min_degree>::typedFile() : fstream()
{
    //cria um arquivo padrao, com um cabe�alho padrao criado pelo construtor do cabe�alho
}
*/
template <class T, const unsigned int min_degree>
TypedFile<T, min_degree>::TypedFile(const string name, const string type, const unsigned int ver) : fstream(name.c_str(), mode)
{
    this->open(name, type, ver, mode);
}

template <class T, const unsigned int min_degree>
TypedFile<T, min_degree>::~TypedFile()
{
    this->close();
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::open(const string name, const string type, const unsigned int ver, ios::openmode openmode)
{
    if (!this->isOpen()) // se nao estiver aberto (neste caso significa que o arquivo nao existe na memoria)
    {
        fstream::open(name.c_str(), ios::out); // crio na memoria o arquivo         //quando tem o mesmo nome preciso colocar o operador de resolucao de escopo para me dizer de qual classe � aquele membro
        fstream::close();
        fstream::open(name.c_str(), openmode); // se nao escolher o tipo de abertura, ira abrir com o tipo padra (openmode = mode)

        this->head = header(type, ver);
        this->writeHeader(this->head);

        record<T, min_degree> recRaiz;
        this->insertRecord(recRaiz); // inserindo o n� raiz pois ele nao existe

        cout << "Arquivo criado com sucesso!" << endl;
        return true;
    }
    else
    {
        this->readHeader(this->head);
        if ((type == this->head.getType()) && (ver == this->head.getVersion())) // verifica se o arq � da mesma versao e do tipo declarado anteriormente
        {
            cout << "Arquivo acessado com sucesso!" << endl;

            return true;
        }
        else
        {
            cout << "ESSE ARQUIVO NAO � DESTE TIPO OU/E DESTA VERS�O." << endl;
            this->close();
        }

        return false;
    }
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::isOpen()
{
    return this->is_open(); // ou return fstream::is_open(); pois veio da classe mae
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::close()
{
    fstream::close();
    return true;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::clear()
{
    fstream::clear();
    return true;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::readRecord(record<T, min_degree> &r, unsigned long long int i)
{
    char *aux = new char[r.size()];
    this->clear();
    this->seekg(this->index2pos(i), ios::beg);
    this->read(aux, r.size());
    string serializedRecord = string(aux, r.size());
    r.fromString(serializedRecord);
    delete[] aux;
    aux = nullptr;
    return true;
}

// indice do arquivo
template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::writeRecord(record<T, min_degree> &r, unsigned long long int i)
{
    this->clear();
    this->seekp(this->index2pos(i), ios::beg);
    this->write(r.toString().c_str(), r.size());
    return true;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::writeNo(Node<T, min_degree> &n, unsigned long long int i)
{
    record<T, min_degree> r;
    r.setData(n);

    this->clear();
    this->seekp(this->index2pos(i), ios::beg);
    this->write(r.toString().c_str(), r.size());
    return true;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::insertRecord(record<T, min_degree> &r)
{
    if (this->getFirstDeleted() != 0) // quero inserir o novo registro r na posicao index do ultimo deletado
    {
        record<T, min_degree> recordAuxFirstDeleted;
        this->readRecord(recordAuxFirstDeleted, this->index2pos(this->getFirstDeleted()));

        // r.next nesta linha abaixo � uma auxiliar
        r.setNext(recordAuxFirstDeleted.getNext()); // o primeiro deletado � preservado aqui no registro r.next que neste momento serve apenas como auxiliar

        recordAuxFirstDeleted.setNext(this->getFirstValid());
        this->setFirstValid(this->getFirstDeleted()); // setamos o firstValid para a primeira posicao deletada

        this->setFirstDeleted(r.getNext()); // uso o valor de r.next que esta servindo como auxiliar aqui

        recordAuxFirstDeleted.setData(r.getData()); // envio o dado que quero inserir para recordAuxFirstDeleted
        recordAuxFirstDeleted.undel();              // marco ele como nao deletado ou seja valido

        this->writeHeader(this->head);
        this->writeRecord(recordAuxFirstDeleted, this->getFirstValid()); // firstValid foi anteriormente setada para que essa posicao seja realmente a posicao que queremos
        return true;
    }
    else // se nao houver nenhum deletado adiciona ao final do arquivo o registro
    {
        unsigned long long int position;
        seekp(0, ios::end);
        position = fstream::tellp();
        r.setNext(this->getFirstValid());
        this->setFirstValid(this->pos2index(position));
        this->writeHeader(this->head);
        this->writeRecord(r, this->pos2index(position));
        return true;
    }
}

template <class T, const unsigned int min_degree>
long long int TypedFile<T, min_degree>::insertNo(Node<T, min_degree> &n)
{
    record<T, min_degree> r;
    r.setData(n);

    if (this->getFirstDeleted() != 0) // quero inserir o novo registro r na posicao index do ultimo deletado
    {
        record<T, min_degree> recordAuxFirstDeleted;
        this->readRecord(recordAuxFirstDeleted, this->index2pos(this->getFirstDeleted()));

        // r.next nesta linha abaixo � uma auxiliar
        r.setNext(recordAuxFirstDeleted.getNext()); // o primeiro deletado � preservado aqui no registro r.next que neste momento serve apenas como auxiliar

        recordAuxFirstDeleted.setNext(this->getFirstValid());
        this->setFirstValid(this->getFirstDeleted()); // setamos o firstValid para a primeira posicao deletada

        this->setFirstDeleted(r.getNext()); // uso o valor de r.next que esta servindo como auxiliar aqui

        recordAuxFirstDeleted.setData(r.getData()); // envio o dado que quero inserir para recordAuxFirstDeleted
        recordAuxFirstDeleted.undel();              // marco ele como nao deletado ou seja valido

        this->writeHeader(this->head);
        this->writeRecord(recordAuxFirstDeleted, this->getFirstValid()); // firstValid foi anteriormente setada para que essa posicao seja realmente a posicao que queremos
        return this->getFirstValid();
    }
    else // se nao houver nenhum deletado adiciona ao final do arquivo o registro
    {
        unsigned long long int position;
        seekp(0, ios::end);
        position = fstream::tellp();
        r.setNext(this->getFirstValid());
        this->setFirstValid(this->pos2index(position));
        this->writeHeader(this->head);
        this->writeRecord(r, this->pos2index(position));
        return this->pos2index(position);
    }
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::deleteRecord(unsigned long long int i)
{
    record<T, min_degree> r;
    this->readRecord(r, i);
    if (search(r.getData()) != 0) // se o indice � valido e existe no arquivo fa�a
    {
        // fa�a isso se o primeiro valor valido for aquele que queremos remover
        if (this->getFirstValid() == i)
        {
            this->setFirstValid(r.getNext());
        }
        else // fa�a isso se o valor a ser removido for diferente do primeiro valor valido (firstValid)
        {
            /*
            //desta forma o programa acessa todos os registros validos do arquivo
            unsigned long long int iaux = this->getFirstValid();
            while (iaux != 0)
            {
                record<T,min_degree> recorAux;
                this->readRecord(recorAux, iaux);
                if(recorAux.getNext() == i)
                {
                    recorAux.setNext(r.getNext());
                    this->writeRecord(recorAux, iaux);
                    //atribuo zero para parar de verificar o arquivo
                    iaux = 0;
                }
                //percorre next para percorrer o arquivo (decrementa o valor de iaux)
                iaux = recorAux.getNext();
            }
            */

            // desta forma o programa acessa o arquivo a partir do registro a ser deletado ate encontrar o que precisa. (acessa do registro a ser deletado at� "o inicio do arquivo ou at� econtrar o registro que precisa)
            record<T, min_degree> recordValid; // por padrao o construtor cria ele nao deletado (deleted = false)
            recordValid.del();
            unsigned long long int iaux = i;
            while (recordValid.isDeleted())
            {
                iaux += 1;
                this->readRecord(recordValid, iaux);
            }
            recordValid.setNext(r.getNext());
            this->writeRecord(recordValid, iaux);
        }

        // atualiza o primeiro deletado para o indice removido
        r.setNext(this->getFirstDeleted());
        this->setFirstDeleted(i);
        r.del(); // seto como deletado
        // escreve as atualiza��es no arquivo
        this->writeHeader(this->head);
        this->writeRecord(r, i);
        return true;
    }
    else // se o indice nao existe/n�o � valido no arquivo
    {
        return false;
    }
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::getFirstValid()
{
    return this->head.getFirstValid();
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::getFirstDeleted()
{
    return this->head.getFirstDeleted();
}

template <class T, const unsigned int min_degree>
void TypedFile<T, min_degree>::setFirstValid(unsigned long long int valid)
{
    this->head.setFirstValid(valid);
    // sempre que modificar algum valor do cabe�alho, ele deve ser atualizado na memoria
    this->writeHeader(this->head);
}

template <class T, const unsigned int min_degree>
void TypedFile<T, min_degree>::setFirstDeleted(unsigned long long int deleted)
{
    this->head.setFirstDeleted(deleted);
    this->writeHeader(this->head);
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::search(Node<T, min_degree> data) // retorna o indice em que o registro esta no arquivo fisico e nao o indice logico do registro
{
    unsigned long long int i = this->getFirstValid();

    record<T, min_degree> r;
    while (i != 0)
    {
        this->readRecord(r, i);
        if (data == r.getData())
        {
            return i;
        }
        i = r.getNext();
    }
    return 0;
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::searchLogic(Node<T, min_degree> data) // retorna o indice logico do registro  e nao o indice em que o registro esta no arquivo fisico
{
    if (this->search(data) != 0) // se o dado existe no arquivo
    {
        this->seekp(0, ios::end);
        unsigned long long int indexFinal = this->pos2index(this->tellp());
        unsigned long long int indexBegLogic = 1;
        record<T, min_degree> r;

        for (int i = 1; i < indexFinal; i++)
        {
            this->readRecord(r, i);
            if (!r.isDeleted())
            {
                if ((r.getData() == data))
                {
                    return indexBegLogic;
                }
                indexBegLogic++;
            }
        }
    }
    return 0;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::readHeader(header &h)
{
    char *aux = new char[h.size()];
    this->clear();
    this->seekg(0, ios::beg);
    this->read(aux, h.size());
    string serializedHead = string(aux, h.size());
    h.fromString(serializedHead);
    delete[] aux;
    aux = nullptr;
    return true;
}

template <class T, const unsigned int min_degree>
bool TypedFile<T, min_degree>::writeHeader(header &h)
{
    string serializedHead = h.toString();
    this->clear();
    this->seekp(0, ios::beg);
    this->write(serializedHead.c_str(), h.size());
    return true;
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::index2pos(unsigned long long int i)
{
    record<T, min_degree> r;
    unsigned long long position = this->head.size() + ((i - 1) * r.size());
    return position;
}

template <class T, const unsigned int min_degree>
unsigned long long int TypedFile<T, min_degree>::pos2index(unsigned long long int p)
{
    record<T, min_degree> r;
    unsigned long long index = ((p - this->head.size()) / r.size()) + 1;
    return index;
}