#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int contador_escritura = 0;
int contador_lectura = 0;

vector<vector<short>> matrizglobal;

class Node
{
public:
    short x;
    short x2;
    short y;
    short y2;
    short data;
    Node* subnodos[4];
    bool eshoja;

    Node(short x, short x2, short y, short y2)
    {
        for(int i = 0; i < 4; i++)
        {
            subnodos[i] = nullptr;
        }
        this-> x = x;
        this-> x2 = x2;
        this-> y = y;
        this-> y2 = y2;
        eshoja = false;
    }

    void printNodesConexiones(fstream &file){
        if(this->subnodos[2] != nullptr){
            file << "\"" << this << "\"->";
            file << "\"" << subnodos[2] << "\";\n";
            this->subnodos[2]->printNodesConexiones(file);
        }
        if(this->subnodos[3] != nullptr){
            file << "\"" << this << "\"->";
            file << "\"" << subnodos[3] << "\";\n";
            this->subnodos[3]->printNodesConexiones(file);
        }
        if(this->subnodos[0] != nullptr){
            file << "\"" << this << "\"->";
            file << "\"" << subnodos[0] << "\";\n";
            this->subnodos[0]->printNodesConexiones(file);
        }
        if(this->subnodos[1] != nullptr){
            file << "\"" << this << "\"->";
            file << "\"" << subnodos[1] << "\";\n";
            this->subnodos[1]->printNodesConexiones(file);
        }
    }

    void printAllNodes(fstream &file){
        file << "\"" << this << "\" [\n";
        file << "\tlabel = \"" <<"\\ P: "<< this->x<<","<<this->y<<" \\ c: "<< this->data <<"\\ s: "<<this->x <<","
             << this->y << "\\ e: "<<this->x2 <<"," << this->y2 <<" \"\n]\n";
        if(this->subnodos[2] != nullptr){
            this->subnodos[2]->printAllNodes(file);
        }
        if(this->subnodos[3] != nullptr){
            this->subnodos[3]->printAllNodes(file);
        }
        if(this->subnodos[0] != nullptr){
            this->subnodos[0]->printAllNodes(file);
        }
        if(this->subnodos[1] != nullptr){
            this->subnodos[1]->printAllNodes(file);
        }
    }

};

class QuadTree {
private:
    Node* root;
    vector<vector<short>> matriz_de_pixeles;
public:
    QuadTree() = default;
    QuadTree(vector<vector<short>> matriz)
    {
        this->matriz_de_pixeles = matriz;
        Node* raiz = new Node(0,matriz[0].size()-1,0,matriz.size()-1);
        this->root = raiz;
        cout << matriz[0].size() << " " << matriz.size();

        build_node(0,matriz[0].size()-1,0,matriz.size()-1, this->root);
    }


    void build_node(short x, short x2, short y, short y2, Node* nodo)
    {
        //    cout << "PRINT 1: "<<endl;
        //    cout << "x: "<< x << " x2: " << x2 << " y: " << y << " y2: " << y2<<endl;

        bool color_entero = revisar_color_entero(x,x2,y,y2);

        if(!color_entero)
        {
            short mitadx = (x + x2)/2;
            short mitady = (y + y2)/2;

            auto temp = new Node(x,mitadx,y,mitady);
            nodo->subnodos[0] = temp;

            temp = new Node(mitadx + 1, x2, y, mitady);
            nodo->subnodos[1] = temp;

            temp = new Node(x,mitadx,mitady + 1, y2);
            nodo->subnodos[2] = temp;

            temp = new Node(mitadx + 1, x2, mitady + 1, y2);
            nodo->subnodos[3] = temp;

            build_node(x,mitadx,y,mitady, nodo->subnodos[0]);
            build_node(mitadx + 1, x2, y, mitady, nodo->subnodos[1]);
            build_node(x,mitadx,mitady + 1, y2, nodo->subnodos[2]);
            build_node(mitadx + 1, x2, mitady + 1, y2, nodo->subnodos[3]);
        }
        else
        {
            nodo->eshoja = 1;
            nodo->data = matriz_de_pixeles[y][x];

            const string archivo = "quadtree.dat";

            ofstream file(archivo, ios::binary | ios::app);
            if(file.is_open())
            {
                file.seekp(0,ios::beg);
                //        cout << "PRINT2: "<<endl;
                file.write((char*) &x, sizeof(short));
                file.write((char*) &x2, sizeof(short));
                file.write((char*) &y, sizeof(short));
                file.write((char*) &y2, sizeof(short));
                file.write((char*) &nodo->data, sizeof(short));
                contador_escritura++;
                //          cout << "x: "<< x << " x2: " << x2 << " y: " << y << " y2: " << y2<<endl;
                file.close();
            }
        }

    }


    bool revisar_color_entero(short x, short x2, short y, short y2)
    {
        short color = matriz_de_pixeles[y][x];
        for(int i = y; i <= y2; i++)
        {
            for(int j = x; j <= x2; j++)
            {
                if(matriz_de_pixeles[i][j] != color)
                {
                    return false;
                }
            }
        }
        return true;
    }

    void descomprimir_imagen(int ancho, int alto)
    {
        vector<vector<short>> matriz (alto,vector<short>(ancho));
        ifstream file("quadtree.dat");
        if(file.is_open())
        {
            file.seekg(0,ios::beg);
            short x, x2, y, y2;
            short data = 0;
            while(file.good()){
                file.read((char*) &x, sizeof(short));
                file.read((char*) &x2, sizeof(short));
                file.read((char*) &y, sizeof(short));
                file.read((char*) &y2, sizeof(short));
                file.read((char*) &data, sizeof(short));
                contador_lectura++;
                //       cout << x << " " << x2 << " " << y << " " << y2<<endl;
                for(int a = y; a <= y2; a++)
                {
                    for(int b = x; b <= x2; b++)
                    {
                        matriz[a][b] = data;
                    }
                }
            }
        }
        file.close();
        matriz_a_ppm(matriz);
    }

    void matriz_a_ppm(vector<vector<short>> matriz)
    {
        ofstream archivo ("quadtree.pgm", ios::binary);
        if(archivo.is_open()){
            archivo.seekp(0,ios::beg);
            archivo << "P2\n";
            archivo << "#Por: Nicolas y Cristian\n";
            archivo << matriz[0].size() <<" "<<  matriz.size()<<"\n";
            archivo << 15 << "\n";
            for (int i = 0 ; i < matriz.size(); i++) {
                for (int j = 0; j < matriz[0].size(); j++)
                {
                    //  cout << "i:" << i << " j:" << j << " " << matriz[i][j] <<endl;
                    archivo << matriz[i][j]<<" ";
                }
                archivo << "\n";
            }
            archivo.close();
        }
    }
    void generatePDF() {
        fstream file("graph.vz", fstream::out | fstream::trunc);
        if (file.is_open()) {
            file << "digraph G\n";
            file << "{\n";
            root->printAllNodes(file);
            root->printNodesConexiones(file);
            file << "}\n";
            file.close();
            system("dot -Tpdf graph.vz -o graph.pdf");
        }
    }

};

vector<vector<short>> LeerArchivo(const char *file){
    int largo = 0;
    int cifra = 0;
    int ancho = 0;
    vector<vector<short>> matriz;
    string largito;
    string cifrita;
    string anchito;
    fstream hola;
    string data;
    int i = 0;
    int contador = 1;
    int ContadorParaSaberSiEntraEnElVector=0;

    hola.open(file,ios::in|ios::out);
    if (hola.is_open()) {
        vector<short> numerosDeLaMatriz;
        while(getline(hola,data)) {

            stringstream stream(data);
            string valor;



            while (getline(stream, valor, ' '))
            {
                string x, y;

                if (contador == 3)
                {
                    if (i == 0) {
                        largito = valor;
                        largo = stoi(largito);
                        i++;
                    } else {
                        anchito = valor;
                        ancho = stoi(anchito);
                        i--;
                    }
                }
                if (contador >= 5)
                {
                    if(valor != ""){
                        cifrita = valor;
                        cifra = stoi(valor);
                        numerosDeLaMatriz.push_back(short(cifra));
                        ContadorParaSaberSiEntraEnElVector++;
                    }
                    else{

                        continue;
                    }
                }
            }
            if(contador >= 5){
                if(ContadorParaSaberSiEntraEnElVector == largo){
                    matriz.push_back(numerosDeLaMatriz);
                    numerosDeLaMatriz.clear();
                    ContadorParaSaberSiEntraEnElVector = 0;
                }
            }
            contador++;
        }
    }
    return matriz;
}

int main() {

    vector<vector<short>> matriz2;
    int relacionsin;
    int relacioncon;
    cout << "Prueba 2:" << endl;
    matriz2 = LeerArchivo("Prueba2.pgm");
    cout << "Dimension de la imagen: " << endl;
    //cout << matriz2.size() << " " << matriz2[0].size();
    auto arbol = QuadTree(matriz2);
    //auto arbol = QuadTree();
    arbol.descomprimir_imagen(matriz2[0].size(),matriz2.size());
    cout << endl << "Relacion  de  compresion: " << endl;
    ifstream file("quadtree.dat");
    if(file.is_open())
    {
        file.seekg(0,ios::end);
        relacionsin = file.tellg();
    }
    file.close();
    cout <<"Comprimir: " <<relacionsin << endl;

    ifstream file2("Prueba2.pgm");
    if(file2.is_open())
    {
        file2.seekg(0,ios::end);
        relacioncon = file2.tellg();
    }
    file2.close();
    cout <<"Sin Comprimir: "<< relacioncon << endl;



    return 0;
}