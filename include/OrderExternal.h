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
    for (unsigned int i = 0; i < NUM_TAPES; ++i) {
        string tapeName = "temp_" + to_string(i) + ".txt";
        tapes[i].open(tapeName, ios::in | ios::out | ios::trunc);
        if (!tapes[i]) {
            cerr << "Unable to create temporary file: " << tapeName << "\n";
            return;
        }
    }
    unsigned int current = 0;

    while(!finished) {
        finished = readIntoMemory(file);
        if (finished)
            break;
        sortInternalMemory();
        fillInitialTapes(tapes, current);
    }
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
        // Use a stringstream to split the line into individual numbers
        cout << "THE LINE: " << endl << line << endl;
        istringstream ss(line);
        int number;
        int currentPosition = 0;

        if (position < MEMORY_SIZE)
            tape.seekg(tapePos);

        // Iterate over the numbers in the line
        while (ss >> number) {
            if (currentPosition == position) {  // If the current position matches, return the number
                return number;
            }
            currentPosition++;
        }
    }

    // Return a sentinel value if unable to read the number (in case of EOF or invalid position)
    return -1;
}

void OrderExternal::intercalateData(array<fstream, NUM_TAPES>& tapes) {
    // resetar as fitas que foram manipuladas anteriormente
    tapes[0].seekg(0, ios::beg);
    tapes[0].clear();
    tapes[1].seekg(0, ios::beg);
    tapes[1].clear();

    vector<int> buffer = {-1, -1, -1}; // um espaco de memoria para cada fita
    vector<bool> tapesActive = {1, 1, 0};
    vector<int> tapesPosition = {0, 0, 0};
    int outputTape = 2;
    int currentBuffer = 0;

    while (thereAreActiveTapes(tapesActive)) {
        // read an active tape into buffer if available
        while (thereAreFreeBuffers(buffer)) {
            cout << "FREE BUFFERS? " << thereAreFreeBuffers(buffer) << endl;
            for (int i=0; i < NUM_TAPES; i++) {
                cout << "THE I " << i << endl;
                if (tapesActive[i] == 1) { // se a fita estiver ativa
                    cout << "TAPE " << i << "IS ACTIVE" << endl;
                    buffer[currentBuffer] = readFromTape(tapes[i], tapesPosition[i]); // leia o valor na posicao correta
                    tapesPosition[i]++;
                    if (tapesPosition[i] >= MEMORY_SIZE) { // se tiver acabado o bloco, reseta
                        tapesActive[i] = 0;
                        tapesPosition[i] = 0;
                    }
                }
            }
            currentBuffer++;
            cout << "THE BUFFER" << endl;
            for (int b : buffer) {
                cout << b << " ";
            }
            cout << endl;
        }
        tapesActive = {0, 0, 0};
    }
    cout << "CLOSED" << endl;
    for (int b : buffer) {
        cout << b << " ";
    }
    cout << endl;
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
