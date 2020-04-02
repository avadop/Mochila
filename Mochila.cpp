#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include "colaPrioridadMaximos.h"
using namespace std;

const int INGENUA = 0;
const int EXHAUST = 1;

struct nodo {
	vector<int> sol; //solucion que se va formando con 0s y 1s
	int k; //valor por el que vamos recorriendo cada nodo
	int valorAcum; //valor que lleva de momento esta solucion
	int pesoAcum; //peso que lleva la solucion
	int valor_optim; //el beneficio optimista que podemos obtener con esta solucion
};

/*
Ordenamos los datos por su densidad V[i]/P[i]
*/
void ordenar(vector<int> & peso, vector<int> & valor, int N) {
	int actual_p, actual_v;
	int j;
	float densidad, densi;

	for (int i = 1; i < N; i++) {
		actual_p = peso[i]; 
		actual_v = valor[i];
		j = i;

		densidad = static_cast<double>(valor[j - 1]) / static_cast<double>(peso[j - 1]);
		densi = static_cast<double>(actual_v) / static_cast<double>(actual_p);
		while(j > 0 && (densidad < densi)){
			peso[j] = peso[j - 1];
			valor[j] = valor[j - 1];
			j--;
			if (j > 0) 
				densidad = static_cast<double>(valor[j - 1]) / static_cast<double>(peso[j - 1]);
			
		}	
		peso[j] =actual_p;
		valor[j] = actual_v;
	}
}

/**
CALCULO DE BENEFICIOS INGNORANTE

-- EL beneficio pesimista va a consistir en el valor acumulado que llevamos hasta el momento.
-- El beneficio optimista es la suma de todos los objetos que quedan por 
meter, es decir, asumimos que entran todos los restantes.


@int P[] -> pesos de los objetos, almacenado en su pos i
@int V[] -> valores de los objetos, almacenado en su pos i
@int M -> maximo peso que se puede llevar en la mochila.
@int k -> posicion del ultimo elemento evaluado 
@int peso -> peso acumulado hasta el momento.
@int valor -> valor acumulado hasta el momento
@int & pesim -> Cota pesimista, se devuelve como parametro
@return int -> devuelve la cota optimista
*/

int  calculo_estimado_ingenuo(const vector<int> & p, const vector<int> & v, int M, int k, int peso, int valor, int & pesim,int N) {
	pesim = valor; //cogemos como cota pesima el peso que llevamos
	int optim = valor;
	
	for (int j = k + 1; j < N; j++) //optim maximo valor si todos entraran.
		optim = optim + v[j];

	return optim;
}

/**
CALCULO DE BENEFICIOS COSTOSO

-- EL beneficio pesimista consiste en meter todos los objetos posibles en el 
orden que estan colocados.
-- El beneficio optimista es el problema resuelto con metodo voraz suponiendo
que los objetos son fraccionables.

@int P[] -> pesos de los objetos, almacenado en su pos i
@int V[] -> valores de los objetos, almacenado en su pos i
@int M -> maximo peso que se puede llevar en la mochila.
@int k -> posicion del ultimo elemento evaluado
@int peso -> peso acumulado hasta el momento.
@int valor -> valor acumulado hasta el momento
@int & pesim -> Cota pesimista, se devuelve como parametro
@return int -> devuelve la cota optimista
*/
int calculo_estimado_costoso(const vector<int> & p, const vector<int> & v, int M, int k, int peso, int valor, int & pesim, int N) {
	int optim = valor; 	pesim = valor;
	int j = k + 1, huecoRestante = M - peso;

	while (j < N && p[j] <= huecoRestante) { //podemos coger j
		huecoRestante = huecoRestante - p[j];
		pesim = pesim + v[j];
		optim = optim + v[j];
		j++;
	}
	if (j < N) { //quedan objetos por probar
		double fraccion = static_cast<double>(huecoRestante) / static_cast<double>(p[j]);
		optim += fraccion * v[j];
		j = j++;
		while (j < N && huecoRestante > 0) {
			if (p[j] <= huecoRestante) {
				huecoRestante = huecoRestante - p[j];
				pesim = pesim + v[j];
			}
			j++;
		}
	}
	return optim;
}

/**
Devuelve un int con el mejor valor de los objetos almacenados
en la mochila.

@int P[] -> pesos de los objetos, almacenado en su pos i
@int V[] -> valores de los objetos, almacenado en su pos i
@int M -> maximo peso que se puede llevar en la mochila.
@int sol_mejor[] -> devuelve la solucion de los objetos i
que se usan(1) y los que no se cogen(0).
@return int -> devuelve el mayor valor obtenido, el de la solcion
*/
int obj_mochila_cota(const vector<int> & p,const vector<int> & v, int M, vector<int> & sol_mejor, int cota) {
	int mejor_valor = 0;
	clock_t begin, end;
	srand(time(NULL));
	int nodos = 0;
	int tam = sol_mejor.size();

	begin = clock();
	//generamos la raiz
	nodo Y, X;
	Y.k = -1; Y.pesoAcum = 0; Y.valorAcum = 0;
	Y.valor_optim = 0; Y.sol = vector<int>(tam);
	//calculamos el beneficio optimista inicial y la cota pesimista inicial = mejor_valor

	if(cota == INGENUA)
		Y.valor_optim = calculo_estimado_ingenuo(p, v, M, Y.k, Y.pesoAcum, Y.valorAcum, mejor_valor,tam);
	else if(cota == EXHAUST)
		Y.valor_optim = calculo_estimado_costoso(p, v, M, Y.k, Y.pesoAcum, Y.valorAcum, mejor_valor,tam);

	ColaPrioridad<nodo> C = ColaPrioridad<nodo>();

	//añadir Y a la cola de prioridad de max. Ordenada por valores optimos.
	C.anyadir(Y, Y.valor_optim); 

	//si aun quedan nodos por explorar cuyo valor optimo es mejor que el mejor valor real sigue.
	while (!C.esVacio() && C.maximo().valor_optim >= mejor_valor) {
		nodos++;
		Y = C.maximo(); //extraemos el maximo
		C.eliminarMax();
		//X va a ser la siguiente solucion generada a partir de Y.
		X.k = Y.k + 1; 
		X.sol = Y.sol;

		//probamos a meter el objeto en la mochila
		if (Y.pesoAcum + p[X.k] <= M) { 
			X.sol[X.k] = 1;
			X.pesoAcum = Y.pesoAcum + p[X.k];
			X.valorAcum = Y.valorAcum + v[X.k];
			X.valor_optim = Y.valor_optim;

			if (X.k == (tam - 1)) { //ha llegado al ultimo objeto
				//se actualiza la solucion mejor con la solcuion del nodo actual
				//y el mejor beneficio obtenido con esta solucion.
					sol_mejor = X.sol; //sol_mejor = X.sol;
					mejor_valor = X.valorAcum;
			}
			//si no ha terminado simplemente se anyade a la cola para que se evalue
			else C.anyadir(X, X.valor_optim);
		}

		int pesim;
		/*se calcula el valor optimo de X como si no hubiesemos metido el objeto y su cota pesim.
		Comentar y descomentar segun la que se quiera usar		*/

		if(cota == INGENUA)
			X.valor_optim = calculo_estimado_ingenuo(p, v, M, X.k, Y.pesoAcum, Y.valorAcum, pesim,tam);
		else if(cota == EXHAUST)
			X.valor_optim = calculo_estimado_costoso(p, v, M, X.k, Y.pesoAcum, Y.valorAcum, pesim,tam);

		/*si el valor optimo de X recien calculado es mejor que el mejor 
		valor que llevamos calculado, evaluamos no meter el objeto k*/
		if (X.valor_optim >= mejor_valor) {
			X.sol[X.k] = 0;
			X.pesoAcum = Y.pesoAcum;
			X.valorAcum = Y.valorAcum;

			//si ya se ha completado la solcion se actualizan los valores que se van a pasar.
			if (X.k == (tam - 1)) {
					sol_mejor = X.sol;
					mejor_valor = X.valorAcum;
			}
			else { //si no es el final se mete en la cola y se actualiza mejor_valor con el benef. pesimista
				C.anyadir(X, X.valor_optim);
				if (mejor_valor < pesim)
					mejor_valor = pesim;
			}
		}
	}
	end = clock();
	double valorTiempo = (static_cast<double>(end - begin)) / CLOCKS_PER_SEC;
	cout << "TIEMPO EJECUCION (seg): " << valorTiempo << endl << "NODOS EXPLORADOS:" << nodos;
	cout << endl << "TIEMPO POR NODO: " << valorTiempo / static_cast<double>(nodos) << " t/nodo";
	return mejor_valor;
}

/**
Es una version del algoritmo simplificada para calcular el numero 
de nodos explorados sin ninguna cota*/

int obj_mochila_factibilidad(const vector<int> & p, const vector<int> & v, int M, vector<int> & sol_mejor) {
	int mejor_valor = 0;
	clock_t begin, end;
	srand(time(NULL));
	int nodos = 0;
	int tam = sol_mejor.size();

	begin = clock();
	nodo Y, X;
	Y.k = -1; Y.pesoAcum = 0; Y.valorAcum = 0;
	Y.valor_optim = 0; Y.sol = vector<int>(tam);

	ColaPrioridad<nodo> C = ColaPrioridad<nodo>();

	C.anyadir(Y, Y.valor_optim);

	//si aun quedan nodos por explorar sigue
	while (!C.esVacio()) {
		Y = C.maximo(); //extraemos el maximo
		C.eliminarMax();
		nodos++;

		//X va a ser la siguiente solucion generada a partir de Y.
		X.k = Y.k + 1;
		X.sol = Y.sol;// X.sol = Y.sol;

		//probamos a meter el objeto en la mochila
		if (Y.pesoAcum + p[X.k] <= M) {
			X.sol[X.k] = 1;
			X.pesoAcum = Y.pesoAcum + p[X.k];
			X.valorAcum = Y.valorAcum + v[X.k];
			X.valor_optim = Y.valor_optim;

			if (X.k == (tam - 1)) { 
				if (mejor_valor < X.valorAcum) {
					//si el valor acumulado de X es mejor que el mejor valor hasta el momento.
					sol_mejor = X.sol;
					mejor_valor = X.valorAcum;
				}
			}
			//si no ha terminado simplemente se anyade a la cola para que se evalue
			else C.anyadir(X, X.valor_optim);
		}
		
		//probamos a no meter el objeto k
			X.sol[X.k] = 0;
			X.pesoAcum = Y.pesoAcum;
			X.valorAcum = Y.valorAcum;

			//si ya se ha completado la solcion se actualizan los valores que se van a pasar.
			if (X.k == (tam - 1)) {
				if (mejor_valor < X.valorAcum) {
				sol_mejor = X.sol;
				mejor_valor = X.valorAcum;
			}
			}
			else  //si no es el final se mete en la cola
				C.anyadir(X, X.valor_optim);
		
	}
	end = clock();
	double valorTiempo = (static_cast<double>(end - begin)) / CLOCKS_PER_SEC; 
	cout << "TIEMPO EJECUCION (seg): " << valorTiempo << endl << "NODOS EXPLORADOS:" << nodos;
	cout << endl << "TIEMPO POR NODO: " << valorTiempo / static_cast<double>(nodos) << " t/nodo";
	return mejor_valor;
}


int main() {
	ifstream caso("caso1.txt");
	int maxMoch, objs;

	caso >> objs;
	caso >> maxMoch;

	vector<int> peso(objs);
	vector<int> valor(objs);

	for (int i = 0; i < objs; i++) 
		caso >> peso[i];
	
	for (int i = 0; i < objs; i++) 
		caso >> valor[i];

	caso.close();

	ordenar(peso, valor,objs); //ordena por densidades

	vector<int> mejor_sol(objs); 
	int beneficio;

	cout << "** Caso de prueba 1 **" << endl << endl;
	cout << "MOCHILA DE TAMANYO: " << maxMoch << "   NUMERO DE OBJETOS: " << objs << endl;

	cout << endl << "OBJETOS DISPONIBLES (peso, valor)" << endl << "ORDENADOS POR DENSIDAD DE MAYOR A MENOR: " << endl << "{";

	for (int i = 0; i < objs - 1; i++)
		cout << "(" << peso[i] << ", " << valor[i] << "), ";
	cout << "(" << peso[objs - 1] << ", " << valor[objs - 1] << ")}" << endl;

	cout << endl <<"--PROBLEMA CON COTA EXHAUSTIVA--" << endl << endl;

	beneficio = obj_mochila_cota(peso, valor, maxMoch, mejor_sol,EXHAUST);
	cout << endl << "BENEFICIO OBTENIDO: " << beneficio << endl << "SOLUCION: " << endl;

	for (int i = 0; i < objs; i++) {
		cout << mejor_sol[i] << " ";
	}

	cout << endl << endl << "--PROBLEMA CON COTA INGENUA--" << endl << endl;

	beneficio = obj_mochila_cota(peso, valor, maxMoch, mejor_sol,INGENUA);
	cout << endl << "BENEFICIO OBTENIDO: "<< beneficio << endl << "SOLUCION: " << endl;

	for (int i = 0; i < objs; i++) {
		cout << mejor_sol[i] << " ";
	}

	cout << endl << endl << "--PROBLEMA FACTIBILIDAD--" << endl << endl;

	beneficio = obj_mochila_factibilidad(peso, valor, maxMoch, mejor_sol);

	cout << endl << "BENEFICIO OBTENIDO: " << beneficio << endl << "SOLUCION: " << endl;
	for (int i = 0; i < objs; i++) {
		cout << mejor_sol[i] << " ";
	}

	cout << endl << endl << "OBJETOS EN MOCHILA: " << endl;

	for (int i = 0; i < objs; i++) {
		if (mejor_sol[i] != 0)
			cout << "PESO: " << peso[i] << "   VALOR: " << valor[i] << endl;
	}
	

	return 0;
}