#include <iostream>
#include <clocale>
#include "Header.h"
#include "Record.h"
#include "TypedFile.h"
#include "IntSerial.h"
#include "Node.h"
#include "BTree.h"
#include "OrderExternal.h"

using namespace std;

constexpr unsigned int min_degree = 2;

int main()
{
    setlocale(LC_ALL, "Portuguese");

    OrderExternal<IntSerial, min_degree> orderExternal("tapes.dat", "TPG", 1);
    // gera entrada: arquivo com 3000 dados aleatorios e desordenados
    orderExternal.genRandomInput("random");
    // roda a ordenacao polifasica
    orderExternal.orderExternal("random");

    //orderDataExternal("random_input_data");
    //printIntercalationResult();

    int op, n;
    IntSerial key;

    BTree<IntSerial, min_degree> b("teste.dat", "TPG", 1);

    if (b.isOpen())
    {
        cout << "Arquivo aberto com sucesso!\n\n";

        do
        {
            cout << "Selecione uma opcao:\n\n"
                 << "1. Inserir uma chave\n"
                 << "2. Excluir uma chave\n"
                 << "3. Pesquisar um registro\n"
                 << "4. Printar a arvore b\n"
                 << "5. Sair\n\n"
                 << "Sua opcao: ";
            cin >> op;
            switch (op)
            {
            case 1:{
                cout << "Digite um numero inteiro: ";
                cin >> n;
                key.setValue(n);
                b.bTreeInsert(key);
                cout << "Valor " << n << " inserido com sucesso.\n" << endl;
                break;
            }

            case 2:{
                cout << "Digite valor deseja remover do arquivo: ";
                cin >> n;
                key.setValue(n);
                b.bTreeDelete(key);
                break;
            }
            /*
            case 3:{
                cout << "Digite o valor a ser pesquisado: ";
                cin >> n;

                node<intSerial, min_degree> auxNode;
                auxNode.keys[0].setValue(n);

                i = arq.searchLogic(auxNode);

                if (i != 0)
                    cout << "Valor " << n << " encontrado no registro " << i << ". (Esse indice é o indice logico e nao o indice fisico do arquivo.)\n" << endl;
                else
                    cout << "Valor " << n << " nao encontrado.\n" << endl;

                break;
            }
            */
            case 4:{
                cout << "Print da arvore b: " << endl;
                b.printTreeB();
                break;
            }
            case 5:
                cout << "Encerrando o programa... ";
                b.close();
                cout << "concluido." << endl;
                return 0;
                break;
            default:
                cout << "opcao invalida! Tente novamente..." << endl;
                break;
            }
        }
        while (true);
    }
    else
    {
        cout << "Nao foi possivel abrir o arquivo!" << endl;
        return 1;
    }

    return 0;
}
