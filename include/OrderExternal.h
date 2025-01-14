#ifndef ORDEREXTERNAL_H
#define ORDEREXTERNAL_H
#include <fstream>
#include <random>
#include <array>
#include <cstdlib>
#include <sstream>
#include <limits.h>
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
        void intercalateData(array<fstream, NUM_TAPES>& tapes, vector<bool> tapesActive, int outputTape, vector<int> bufferTape);
        bool thereAreActiveTapes(vector<bool> tapesActive);
        bool thereAreFreeBuffers(vector<int> buffer);
        //int readFromTape(fstream& tape, int position);
        int readFromTape(fstream& tape, int rowPosition, int columnPosition);
        pair<int, int> getSmallestBufferValue(vector<int> buffer);
        void writeToTape(fstream& tape, int value, bool nextBlock);
        void fillBuffer(
            vector<int>& buffer,
            array<fstream,
            NUM_TAPES>& tapes,
            vector<bool>& tapesActive,
            vector<int>& bufferTape,
            vector<int>& tapesPosition,
            vector<bool>& tapeHasData,
            int block
        );
        void bufferToTapeOnce(vector<int>& buffer, array<fstream, NUM_TAPES>& tapes, int outputTape, int& numbersWritten);
        void bufferToTape(vector<int>& buffer, array<fstream, NUM_TAPES>& tapes, int outputTape, int& numbersWritten, bool dump);
        void reactivateTapes(vector<bool>& tapesActive, vector<bool>& tapeHasData, int outputTape);
        bool inputTapesHaveData(vector<bool> tapeHasData);
        void splitData(array<fstream, NUM_TAPES>& tapes, vector<bool> tapesActive);

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
        string tapeName = "tape_" + to_string(i+1) + ".txt";
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
    vector<bool> tapesActive = {1, 1, 0}; // intercala da fita 1 e 2 para 3
    vector<int> bufferTape = {0, 1, 0};
    intercalateData(tapes, tapesActive, 2, bufferTape);

    tapesActive = {0, 0, 1}; // splita a fita 3 para a primeira fita inativa para continuar a ordencao
    splitData(tapes, tapesActive);
    tapesActive = {1, 0, 1}; // intercala da fita 1 e 3 para a 2
    bufferTape = {1, 2, 1};
    // FIX HERE
    intercalateData(tapes, tapesActive, 1, bufferTape);

    file.close();
}

void OrderExternal::splitData(array<fstream, NUM_TAPES>& tapes, vector<bool> tapesActive) {
    int tapeToReadFrom = -1;
    int destinationTape = -1;

    // pega a fita de entrada
    for (int i = 0; i < NUM_TAPES; ++i) {
        if (tapesActive[i]) {
            tapeToReadFrom = i;
            break;
        }
    }

    // pega a fita de saida
    for (int i = 0; i < NUM_TAPES; ++i) {
        if (!tapesActive[i]) {
            destinationTape = i;
            break;
        }
    }

    // encontrou as duas fitas?
    if (tapeToReadFrom == -1 || destinationTape == -1) {
        cerr << "Error: Could not determine tapes for splitting.\n";
        return;
    }
    // continua

    // comeca no inicio das duas fitas
    tapes[tapeToReadFrom].seekg(0, ios::beg);
    tapes[destinationTape].seekp(0, ios::beg);

    vector<string> lines;
    string line;
    while (getline(tapes[tapeToReadFrom], line)) {
        lines.push_back(line); // pega as linhas da fonte
    }

    // pega metade das linhas para mover para a outra fita
    size_t halfSize = lines.size() / 2;

    // escreve a primeira metade na fita de saida
    tapes[destinationTape].close();
    tapes[destinationTape].open("tape_" + to_string(destinationTape+1) + ".txt", ios::out | ios::trunc);
    for (size_t i = halfSize; i < lines.size(); ++i) {
        tapes[destinationTape] << lines[i] << "\n";
    }

    // escreve a segunda metade novamente na fita de entrada
    tapes[tapeToReadFrom].close();
    tapes[tapeToReadFrom].open("tape_" + to_string(tapeToReadFrom+1) + ".txt", ios::out | ios::trunc);
    for (size_t i = halfSize; i < lines.size(); ++i) {
        tapes[tapeToReadFrom] << lines[i] << "\n";
    }

    tapes[tapeToReadFrom].flush();
    tapes[destinationTape].flush();

    cout << "Successfully split data: Moved " << halfSize << " lines from tape_"
         << tapeToReadFrom+1 << " to tape_" << destinationTape+1 << ".\n";
}


bool OrderExternal::thereAreActiveTapes(vector<bool> tapesActive) {
    return tapesActive[0] == 1 || tapesActive[1] == 1 || tapesActive[2] == 1;
}

bool OrderExternal::thereAreFreeBuffers(vector<int> buffer) {
    return buffer[0] == -1 || buffer[1] == -1 || buffer[2] == -1;
}

int OrderExternal::readFromTape(fstream& tape, int rowPosition, int columnPosition) {
    string line;

    if (!tape.is_open()) {
        cout << "File is not open." << endl;
        return -1;
    }
    if (tape.fail()) {
        cout << "Error reading from file." << endl;
        return -1;
    }

    // Move to the specified row
    tape.clear(); // Clear any EOF or fail flags
    tape.seekg(0); // Go to the beginning of the file
    for (int currentRow = 0; currentRow <= rowPosition; ++currentRow) {
        if (!getline(tape, line)) {
            cout << "Row position out of range." << endl;
            return -1;
        }
    }

    // Process the line to find the specified column
    istringstream ss(line);
    int number;
    int currentColumn = 0;

    while (ss >> number) {
        if (currentColumn == columnPosition) {
            return number; // Return the value at the desired position
        }
        currentColumn++;
    }

    cout << "Column position out of range." << endl;
    return -1; // Return error if column is out of range
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

void OrderExternal::fillBuffer(
    vector<int>& buffer,
    array<fstream, NUM_TAPES>& tapes,
    vector<bool>& tapesActive,
    vector<int>& bufferTape,
    vector<int>& tapesPosition,
    vector<bool>& tapeHasData,
    int block
) {
    for (int b=0; b<NUM_TAPES; b++) { // para cada posicao do buffer
        // se a fita da posicao do buffer estiver ativa e o buffer estiver vazio
        if (tapesActive[bufferTape[b]] && buffer[b] == -1) {
            // leia a fita correspondente para o buffer
            buffer[b] = readFromTape(tapes[bufferTape[b]], block,tapesPosition[bufferTape[b]]);
            tapesPosition[bufferTape[b]]++; // incrementa a posicao no bloco da fita
            if (buffer[b] == -1) {
                tapeHasData[b] = 0;
            }
        }
        if (tapesPosition[bufferTape[b]] >= 5) { // se o bloco acabou
            tapesActive[bufferTape[b]] = false; // a fita nao esta mais ativa
            tapesPosition[bufferTape[b]] = 0; // a posicao dela e redefinida
            // encontra a proxima fita ativa para substituir pela fita que agora esta inativa
            for (int i=0; i<NUM_TAPES; i++) { // para cada buffer
                if (tapesActive[bufferTape[i]] == false) { // se a fita do buffer esta inativa
                    for (int j=0; j<NUM_TAPES; j++) { // percorre todas as fitas
                        if (tapesActive[j] == true) { // pega a primeira fita ativa encontrada
                            bufferTape[i] = tapesActive[j]; // e coloca nas fitas do buffer
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    for (int b : buffer) {
        cout << b << " ";
    }
    cout << endl;

}

void OrderExternal::bufferToTapeOnce(vector<int>& buffer, array<fstream, NUM_TAPES>& tapes, int outputTape, int& numbersWritten) {
    // coloca o menor valor do buffer na fita de saida, que é a fita nao ativa
    for (int i=0; i < NUM_TAPES; i++) {
        if (outputTape == i) { // se estamos na fita de saida
            // pega o menor valor do buffer e a posicao dele
            auto [smallestBufferValue, position] = getSmallestBufferValue(buffer);
            cout << "SMALLEST VALUE: " << smallestBufferValue << " POS " << position << endl;
            numbersWritten++;
            if (smallestBufferValue != -1) { // se retornou algum valor
                writeToTape(tapes[i], smallestBufferValue, numbersWritten%10 == 0); // escreve na fita de saida
                buffer[position] = -1; //
            }
        }
    }
}

void OrderExternal::bufferToTape(vector<int>& buffer, array<fstream, NUM_TAPES>& tapes, int outputTape, int& numbersWritten, bool dump) {
    if (dump) {
        while (buffer[0] > 0 || buffer[1] > 0 || buffer[2] > 0) {
            bufferToTapeOnce(buffer, tapes, outputTape, numbersWritten);
        }
    } else {
        bufferToTapeOnce(buffer, tapes, outputTape, numbersWritten);
    }
    for (int b : buffer) {
        cout << b << " ";
    }
    cout << endl;
}

void OrderExternal::reactivateTapes(vector<bool>& tapesActive, vector<bool>& tapeHasData, int outputTape) {
    for (int i=0; i< tapesActive.size(); i++) {
        if (tapesActive[i] == 0 and i != outputTape) {
            tapesActive[i] = 1;
        }
    }
}

bool OrderExternal::inputTapesHaveData(vector<bool> tapeHasData) {
    for (int i=0; i<tapeHasData.size(); i++) {
        if (tapeHasData[i] == 1) {
            return true;
        }
    }
    return false;
}

void OrderExternal::intercalateData(
    array<fstream, NUM_TAPES>& tapes,
    vector<bool> tapesActive, // fitas de entrada
    int outputTape, // fita de saida
    vector<int> bufferTape // representa a fita que o buffer esta lindo primeiramente
) {
    // resetar as fitas que foram manipuladas anteriormente
    tapes[0].seekg(0, ios::beg);
    tapes[0].clear();
    tapes[1].seekg(0, ios::beg);
    tapes[1].clear();

    vector<int> buffer = {-1, -1, -1}; // um espaco de memoria para cada fita
    vector<int> tapesPosition = {0, 0, 0}; // representa a posicao no bloco na fita
    vector<int> originalBufferTape = bufferTape; // configuracao de leitura de fitas original

    vector<bool> tapeHasData = {1, 1, 1};
    int numbersWritten = 0;
    int block = 0;

    // enquanto fitas de entrada tiverem dados
    while (inputTapesHaveData(tapeHasData)) {
        fillBuffer(buffer, tapes, tapesActive, bufferTape, tapesPosition, tapeHasData, block); // enche o buffer com dados das fitas ativas
        if (thereAreActiveTapes(tapesActive)) { // se ainda tiver fitas ativas
            bufferToTape(buffer, tapes, outputTape, numbersWritten, 0); // remove 1
        } else { // se nao tiver mais fitas ativas
            bufferToTape(buffer, tapes, outputTape, numbersWritten, 1); // remove o resto que tiver no buffer
            reactivateTapes(tapesActive, tapeHasData, outputTape);
            block++;
            bufferTape = originalBufferTape;
        }
    }

}

pair<int, int> OrderExternal::getSmallestBufferValue(vector<int> buffer) {
    int smallestValue = INT_MAX;
    int position = 0;
    for (int i = 0; i < buffer.size(); i++) {
        if (buffer[i] != -1) {
            smallestValue = buffer[i];
            position = i;
            break;
        }
    }

    if (smallestValue == INT_MAX)
        return {-1, -1};

    for (int i = 0; i < buffer.size(); i++) {
        if (buffer[i] != -1) {
            if (buffer[i] < smallestValue) {
                smallestValue = buffer[i];
                position = i;
            }
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
