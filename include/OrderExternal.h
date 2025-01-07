#ifndef ORDEREXTERNAL_H
#define ORDEREXTERNAL_H
#include <fstream>
#include <random>
#include <array>
#include <cstdlib>
#include <sstream>
#include "TypedFile.h"

using namespace std;

class OrderExternal
{
    public:
        OrderExternal(const string name);
        virtual ~OrderExternal(){};
        void genRandomInput(const string name);
        void orderExternal(const string filename);
    private:
        const static unsigned int MEMORY_SIZE = 5;
        const static unsigned int NUM_TAPES = 3;

        bool readIntoMemory(ifstream &file);
        void memoryToTape();
        void sortInternalMemory();
        void memoryToTape(array<fstream, NUM_TAPES>& tapes, unsigned int currentTape);
        void fillInitialTapes(array<fstream, NUM_TAPES>& tapes, unsigned int& current);
        void intercalateData(array<fstream, NUM_TAPES>& tapes);
        bool thereAreActiveTapes(vector<bool> tapesActive);
        bool thereAreFreeBuffers(vector<int> buffer);
        int readFromTape(fstream& tape, int position);
        pair<int, int> getSmallestBufferValue(vector<int> buffer);
        void writeToTape(fstream& tape, int value, bool nextBlock);

        array<int, MEMORY_SIZE> memory;
};

OrderExternal::OrderExternal(const string name)
{

}

void OrderExternal::genRandomInput(const string name)
{
    ofstream outFile(name);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 3000);

    for (int i = 0; i < 3000; ++i) {
        outFile << dis(gen) << "\n";
    }
}

void OrderExternal::orderExternal(const string filename)
{
    ifstream file(filename);
    if (!file) {
        cerr << "Unable to open file.\n";
        return;
    }

    bool finished = false;
    array<fstream, NUM_TAPES> tapes;

    // cria as fitas temporarias
    for (unsigned int i = 0; i < NUM_TAPES; ++i) {
        string tapeName = "temp_" + to_string(i) + ".txt";
        tapes[i].open(tapeName, ios::in | ios::out | ios::trunc);
        if (!tapes[i]) {
            cerr << "Unable to create temporary file: " << tapeName << "\n";
            return;
        }
    }
    unsigned int current = 0;

    // preenche as duas fitas iniciais
    while(!finished) {
        finished = readIntoMemory(file);
        if (finished)
            break;
        sortInternalMemory();
        fillInitialTapes(tapes, current);
    }
    // intercala as fitas ate finalizar a ordenacao externa
    intercalateData(tapes);

    file.close();
}

bool OrderExternal::thereAreActiveTapes(vector<bool> tapesActive) {
    return tapesActive[0] == 1 || tapesActive[1] == 1 || tapesActive[2] == 1;
}

bool OrderExternal::thereAreFreeBuffers(vector<int> buffer) {
    return buffer[0] == -1 || buffer[1] == -1 || buffer[2] == -1;
}

int OrderExternal::readFromTape(fstream& tape, int position) {
    string line;

    if (!tape.is_open()) {
        cout << "File is not open." << endl;
        return -1;
    }
    if (tape.eof()) {
        cout << "End of file reached." << endl;
        return -1;
    }
    if (tape.fail()) {
        cout << "Error reading from file." << endl;
        return -1;
    }

    streampos tapePos = tape.tellg();
    if (getline(tape, line)) {
        // splita a linha em ints separados
        istringstream ss(line);
        int number;
        int currentPosition = 0;

        if (position < MEMORY_SIZE)
            tape.seekg(tapePos);

        // passa por todos os valores da linha
        while (ss >> number) {
            if (currentPosition == position) {  // se estiver na pos certa retorna o valor
                return number;
            }
            currentPosition++;
        }
    }

    return -1;
}

void OrderExternal::writeToTape(fstream& tape, int value, bool nextBlock) {
    if (!tape.is_open()) {
        cout << "File is not open." << endl;
    }
    if (tape.eof()) {
        cout << "End of file reached." << endl;
    }
    if (tape.fail()) {
        cout << "Error reading from file." << endl;
    }

    tape << value << " ";

    if(nextBlock) { // se deve passar para um novo bloco
        tape << "\n";
    }
}


void OrderExternal::intercalateData(array<fstream, NUM_TAPES>& tapes) {
    // resetar as fitas que foram manipuladas anteriormente
    tapes[0].seekg(0, ios::beg);
    tapes[0].clear();
    tapes[1].seekg(0, ios::beg);
    tapes[1].clear();

    vector<int> buffer = {-1, -1, -1}; // um espaco de memoria para cada fita
    //vector<bool> bufferFull = {0, 0, 0}; // representa se o buffer esta com valor util naquela posicao
    vector<bool> tapesActive = {1, 1, 0}; // represente se a fita esta ativa
    vector<int> tapesPosition = {0, 0, 0}; // representa a posicao no bloco na fita
    int outputTape = 2;
    int shouldBreak = 0;
    // leia os buffers
    while (thereAreActiveTapes(tapesActive)) {
        // leia as fitas ativas para o buffer, se tiver buffer disponivel
        while (thereAreFreeBuffers(buffer)) {
            for (int i=0; i < NUM_TAPES; i++) {
                if (tapesActive[i] == 1) { // se a fita estiver ativa
                    for(int b=0; b<NUM_TAPES; b++) { // para cada buffer
                        if(buffer[b] == -1) { // se o buffer estiver pronto para receber valor
                            buffer[b] = readFromTape(tapes[i], tapesPosition[i]); // leia o valor na posicao correta
                            if (buffer[b] == -1) { // entao acabou de ler o arquivo ou deu algum problema
                                tapesActive[i] = false;
                            }
                            //bufferFull[b] = true; // o buffer encheu com valor, entao nao pode mexer mais por enquanto
                            break; // sai do loop para nao preencher o proximo buffer com a mesma fita
                        }
                    }
                    tapesPosition[i]++;
                    if (tapesPosition[i] >= MEMORY_SIZE) { // se tiver acabado o bloco, reseta
                        tapesActive[i] = 0;
                        tapesPosition[i] = 0;
                    }
                }
            }
            for (int b : buffer) {
                cout << b << " ";
            }
            cout << endl;
        }
        // coloca o menor valor do buffer na fita de saida, que é a fita nao ativa
        for (int i=0; i < NUM_TAPES; i++) {
            if (tapesActive[i] == 0) { // se a fita nao estiver ativa, ela é a fita de saida
                // pega o menor valor do buffer e a posicao dele
                auto [smallestBufferValue, position] = getSmallestBufferValue(buffer);
                //bufferFull[position] = false; // a posicao esta pronta para receber mais valores
                writeToTape(tapes[i], smallestBufferValue, false);
                buffer[position] = -1;
            }
        }
        shouldBreak++;
        if (shouldBreak>=100){
            break;
        }
        //tapesActive = {0, 0, 0};
    }

}

pair<int, int> OrderExternal::getSmallestBufferValue(vector<int> buffer) {
    int smallestValue = buffer[0];
    int position = 0;

    for (int i = 1; i < buffer.size(); ++i) {
        if (buffer[i] < smallestValue) {
            smallestValue = buffer[i];
            position = i;
        }
    }

    return {smallestValue, position};
}


void OrderExternal::fillInitialTapes(array<fstream, NUM_TAPES>& tapes, unsigned int& current) {
    if (current == 0) {
        memoryToTape(tapes, current);
        current++;
    } else if (current == 1) {
        memoryToTape(tapes, current);
        current--;
    }
}


void OrderExternal::memoryToTape(array<fstream, NUM_TAPES>& tapes, unsigned int currentTape)
{
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        tapes[currentTape] << memory[i] << " ";
    }
    tapes[currentTape] << "\n";
}

bool OrderExternal::readIntoMemory(ifstream &file)
{
    // numero de itens lidos para a memoria
    int count = 0;
    int value = 0;
    for (int i = 0; i < MEMORY_SIZE && file >> value; ++i) {
        memory[i] = value;
        ++count;
    }
    // se nao leu nada ou leu menos do que cabia na memoria, termine
    if (count == 0 || count < MEMORY_SIZE)
        return true;
    return false;
}

void OrderExternal::sortInternalMemory()
{
    qsort(
        memory.data(),
        MEMORY_SIZE,
        sizeof(int),
        [](const void * a, const void * b) {
            return ( *(int*)a - *(int*)b );
        });
}


#endif // ORDEREXTERNAL_H
