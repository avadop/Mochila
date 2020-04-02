#ifndef __COLAPRIORIDADMAXIMOS_H
#define __COLAPRIORIDADMAXIMOS_H

#include <iostream>
using namespace std;

template<class T>
class ColaPrioridad{

public:
	ColaPrioridad(): _max(NULL){}

	~ColaPrioridad(){
		//libera(_max);
		_max = NULL;
	}
/*Dice si esta vacia la cola de maximos*/
bool esVacio(){
	if(_max == NULL)
		return true;
	else return false;
}

/*Elimina el elemento maximo, reestructura el resto de elementos*/
void eliminarMax(){
	if(_max != NULL && _max->_siguiente != NULL){
		Nodo *aux = _max;
		_max = aux->_siguiente; //asignamos el siguiente en la cola
		delete aux; //y borramos
	}
	else {
		delete _max;
		_max = NULL;
	}
	//si no hay elementos no hace nada
}

/*Anyade un nuevo elemento a la cola de prioridad, con su prior.
Le coloca donde le corresponde dentro de la cola.*/
void anyadir(const T & elem, int priori){
	Nodo* nuevo = new Nodo(elem,priori);
	if(esVacio())
			_max = nuevo;
	else{
		Nodo* aux = new Nodo();

		if (_max->_priori < nuevo->_priori) {
			aux = _max;
			_max = nuevo;
			_max->_siguiente = aux;
		}
		else{
		Nodo* anterior = encontrarAnterior(priori);

		aux = anterior->_siguiente;
		anterior->_siguiente = nuevo;
		nuevo->_siguiente = aux;

		anterior = nullptr; delete anterior;
		}	
		aux = nullptr;  delete aux; 
	}	
	nuevo = nullptr;  delete nuevo;
	
}

/*Funcion de consulta que devuelve el maximo de la cola
funcion de consulta, si no hay elems devuelve una excepcion*/
T maximo() const{
		return _max->_elem;
	//lanza excepcion
}

protected:
	class Nodo{
		public:
		/*Constructora de nodo vacio*/ 
			Nodo() : _siguiente(NULL) {}
		/*Constructora nodo con un elemento
		_siguiente se anyade cuando se anyade el nodo a la cola, tras encontrar pos*/
			Nodo(const T & elem, int pri) : _elem(elem), _priori(pri), _siguiente(NULL) {}

		T _elem;	
		int _priori;
		Nodo* _siguiente; 
	};

	ColaPrioridad(Nodo *max): _max(max){}

	/*Componentes de la cola, guarda el maximo, y a partir de el se pueden acceder al resto*/
	Nodo *_max;
	
private:

	Nodo* encontrarAnterior(int prior){
		Nodo* anterior = _max;
		while(anterior->_siguiente != NULL && anterior->_siguiente->_priori >= prior){ 
			//mientras prioridad del insertado sea menor que la prioridad del siguiente que se va a explorar y ese sea distinto de null
			anterior = anterior->_siguiente; //cuando es menor para
		}
		return anterior;
	}
	/**
	Elimina todos los nodos de una cola de prioridad
	que comienza con el puntero ini.
	Se admite que el nodo sea NULL (no habra nada que
	liberar).
	*/
	/*static void libera(Nodo *ini) {
		if (ini != NULL) {
			libera(ini->_siguiente);
			delete ini;
		}
	}*/


};

#endif // __COLAPRIORIDADMAXIMOS_H