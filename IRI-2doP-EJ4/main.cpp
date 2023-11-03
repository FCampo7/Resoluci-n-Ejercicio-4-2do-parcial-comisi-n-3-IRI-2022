#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

/*
 * structs
*/

typedef struct {
    string grano;
    float precio;
} sPrecio;

typedef struct {
    char campo;
    float precioXquintal; // Esto es quintal/hectarea, en el archivo está mal la cabecera
    float hectareas;
    string semilla;
} sBalanza;

typedef struct {
    char campo;
    float total;
} sCampo;

/*
 * Resizes
*/

void incrementarPrecio(sPrecio* &precio, unsigned int &tam){
    if(precio==nullptr){
        if(tam<=0){
            precio = new sPrecio[++tam];
        }
        return;
    }

    sPrecio* temporal = new sPrecio[++tam];

    for(unsigned int i = 0; i < tam-1; i++)
        temporal[i] = precio[i];

    delete[] precio;

    precio = temporal;
}

void incrementarBalanza(sBalanza* &balanza, unsigned int &tam){
    if(balanza==nullptr){
        if(tam<=0){
            balanza = new sBalanza[++tam];
        }
        return;
    }

    sBalanza* temporal = new sBalanza[++tam];

    for(unsigned int i = 0; i < tam-1; i++)
        temporal[i] = balanza[i];

    delete[] balanza;

    balanza = temporal;
}

void incrementarCampo(sCampo* &campo, unsigned int &tam){
    if(campo==nullptr){
        if(tam<=0){
            campo = new sCampo[++tam];
        }
        return;
    }

    sCampo* temporal = new sCampo[++tam];

    for(unsigned int i = 0; i < tam-1; i++)
        temporal[i] = campo[i];

    temporal[tam-1]={'\0', 0};

    delete[] campo;

    campo = temporal;
}

/*
 * Archivos
*/

void leerPrecios(ifstream &archi, sPrecio* &precio, unsigned int &tamP){
    string linea;
    stringstream s;

    if(archi.is_open()){

        getline(archi, linea);

        while(!archi.eof() && getline(archi, linea)){
            s.clear();
            s<<linea;

            incrementarPrecio(precio, tamP);

            getline(s, linea, ',');
            precio[tamP-1].grano=linea;
            getline(s, linea);
            precio[tamP-1].precio=stof(linea);
        }
    }
}

void leerBalanza(ifstream &archi, sBalanza* &balanza, unsigned int &tamB){
    string linea;
    stringstream s;

    if(archi.is_open()){

        getline(archi, linea);

        while(!archi.eof() && getline(archi, linea)){
            s.clear();
            s<<linea;

            incrementarBalanza(balanza, tamB);

            getline(s, linea, ',');
            balanza[tamB-1].campo = linea[0];
            getline(s,linea, ',');
            balanza[tamB-1].precioXquintal=stof(linea);
            getline(s, linea, ',');
            balanza[tamB-1].hectareas=stof(linea);
            getline(s, linea);
            balanza[tamB-1].semilla=linea;
        }
    }
}

void escribirTXT(ofstream &archiS, sCampo* campo, unsigned int tamC, float total){
    unsigned int j=0;
    if(archiS.is_open()){
        while(j<tamC){
            archiS<<"Campo ["<<campo[j].campo<<"] vendió a U$D ["<<campo[j].total<<"]\n";
            j++;
        }

        archiS<<"Ganancia total U$D ["<<total<<"]\n";
    }
}

/*
 * función procesar
*/

void procesar(sPrecio* precio, sBalanza* balanza, sCampo* &campo, unsigned int tamP, unsigned int tamB, unsigned int &tamC, float &total){
    unsigned int j=0;
    unsigned int k=0;
    float calculo=0;
    total=0;

    for(unsigned int i=0; i < tamB; i++){

        while(j < tamP && balanza[i].semilla != precio[j].grano){ // Busco el precio de mi semilla.
            j++;
        }

        while(k < tamC && balanza[i].campo != campo[k].campo){ // Busco el campo.
            k++;
        }

        if(k < tamC && j < tamP){ // Si el campo existe y el precio de la semilla se encontró => acumulo cuanto vendio el campo.
            calculo = balanza[i].precioXquintal * balanza[i].hectareas * precio[j].precio;
            campo[k].total += calculo;
            total+=calculo;
        }
        else if(k >= tamC && j < tamP){  // si encuentro el precio de la semilla, pero no el campo => creo un nuevo campo y acumulo su venta.
            calculo = balanza[i].precioXquintal * balanza[i].hectareas * precio[j].precio;
            incrementarCampo(campo, tamC);
            campo[tamC-1] = {balanza[i].campo, calculo};
            total+=calculo;
        }

        j=0; // No recuerdo si en la clase reinicie los contadores, pero estos se deben reiniciar!!
        k=0;
    }
}

/*
 * Main
*/

int main(){
    ifstream archi;

    /*
     * Lectura precios
    */
    archi.open("precios.csv");

    sPrecio* precio = nullptr;
    unsigned int tamP=0;

    leerPrecios(archi, precio, tamP);

    archi.close();

    /*
     * Lectura balanza
    */
    archi.open("balanza.csv");

    sBalanza* balanza = nullptr;
    unsigned int tamB = 0;

    leerBalanza(archi, balanza, tamB);

    archi.close();

    /*
     * Procesar y cargar campos
    */
    sCampo* campo = new sCampo[1];
    unsigned int tamC = 1;

    campo[0] = {balanza[0].campo, 0};

    float total=0;

    procesar(precio, balanza, campo, tamP, tamB, tamC, total);

    /*
     * Escribir archivo texto.
    */
    ofstream archiS("informe_cosecha.txt");

    escribirTXT(archiS, campo, tamC, total);

    archiS.close();

    /*
     * Chequeo que mis punteros no sean nulos y los elimino
    */
    if(precio != nullptr) {
        delete[] precio;
        precio = nullptr;
    }

    if(balanza != nullptr) {
        delete[] balanza;
        balanza = nullptr;
    }

    if(campo != nullptr) {
        delete[] campo;
        campo=nullptr;
    }

    return 0;
}
