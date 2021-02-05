#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define M_PI 3.14159265358979323846

typedef struct{
	unsigned long id;
	char *nombre;
	double longitud, latitud;
	unsigned short n_sucesores;
	unsigned long *sucesores;
	short estado;
}nodo;

typedef struct lista{
	nodo *nodo_i;
	double coste, coste_h;
	nodo *padre;
	struct lista *siguiente;
}lista;		

nodo *b_search(unsigned long id, nodo *nodos, int n_nodos, nodo *nodo_errores){
	nodo *puntero;
	int i_max = n_nodos-1, i_min = 0, contador = 0, c_max = 1000;
	int i = i_min + (i_max - i_min) / 2;
	if((id>nodos[i_max].id)||(id<nodos[i_min].id)) {
	puntero = nodo_errores;
	return puntero;}
	unsigned long x = nodos[i].id;
	while((x != id)&&(contador<c_max)){
		if(x>id) i_max = i;
		else if(x<id) i_min = i;
		else break;
		i = i_min + (i_max - i_min) / 2;
		x = nodos[i].id;
		contador++;
	}
	puntero = &nodos[i];
	if(x!=id) puntero = nodo_errores;
	return puntero;
}

//This function computes the distance between two a certain node and the final one.
double h(nodo *nodo2, nodo *final){
	
	double distancia, a, b, lat_dif, long_dif, R, lat1, lat2, long1, long2;
	
	R = 6371;
	lat1 = final->latitud*M_PI/180; lat2 = nodo2->latitud*M_PI/180;
	long1 = final->longitud*M_PI/180; long2 = nodo2->longitud*M_PI/180;
	lat_dif = fabs(lat1 - lat2);
	long_dif = fabs(long1 - long2);
	
	a = pow(sin(lat_dif/2), 2) + cos(lat1) * cos(lat2) * pow(sin(long_dif/2), 2);
	b = 2 * atan2(sqrt(a), sqrt(1 - a));
	distancia = R * b;
	
	return distancia;
}

//This function returns a pointer to that node in a list.
lista *busqueda_lista(lista *listado, nodo *ptr_nodo){
	lista *variable;
	variable = listado;
	while(variable!=NULL){
		if(variable->nodo_i == ptr_nodo) break;
		variable = variable->siguiente;
	}
	return variable;
}

//Function adds a node to a list properly ordering it.
lista *poner_lista(lista **listado, nodo *ptr_nodo, double val){
	lista *variable, *anterior, *nuevo, *original;
	int count=0;
	variable = *listado;
	original = variable;
	anterior = NULL;
	//printf("//////////////////////////////////////////////////////////\n");
	//printf("Valor a introducir: %f\n", val);
	while(variable != NULL){
		if(variable->coste_h > val){
			//printf("check 2\n");
			nuevo = (lista*)malloc(sizeof(lista));
			anterior->siguiente = nuevo;
			nuevo->nodo_i = ptr_nodo;
			nuevo->siguiente = variable;
			nuevo->coste_h = val;
			if(count==0) *listado = anterior->siguiente;
			if(count>3) printf("%f\t%f\t%f\n", original->coste_h, original->siguiente->coste_h, original->siguiente->siguiente->coste_h);
			break;
		}
		else{
			anterior = variable;
			variable = variable->siguiente;
			count++;

		}
	}
	return nuevo;
}

int quitar_lista(lista **listado, nodo *ptr_nodo){
	lista *variable, *anterior;
	variable = *listado;
	anterior = NULL;
	int count=0;
	while(variable != NULL){
		//printf("CHECK quitar 1, nodo id %ld es igual a %ld?\n", variable->nodo_i->id, ptr_nodo->id);
		if(variable->nodo_i->id == ptr_nodo->id){
			//printf("lista NO actualizada %p\t%p\n", listado, variable->siguiente);
			if(count==0) {*listado = variable->siguiente;}
			else {anterior->siguiente = variable->siguiente;}
			//printf("lista actualizada %p\n", *listado);
			free(variable);
			//printf("lista actualizada2 %p\n", *listado);
			return 1;
		}
		else{
			//printf("CHECK quitar 2\n");
			anterior = variable;
			variable = variable->siguiente;	
			count++;			
		}
	}
	//printf("final de quitar_lista\n");
	return 1;
}
int main(){
	int n_nodos, i;
	unsigned long *sucesores, tot_sucesores, id_inicio, id_final;
	nodo *nodos, *inicio, *final, *actual, *nodo_errores;
	FILE *filepointer;
	filepointer = fopen("info_nodos.bin", "r");

	fread(&n_nodos, sizeof(int), 1, filepointer);
	fread(&tot_sucesores, sizeof(unsigned long), 1, filepointer);
	printf("%d\t%ld\n", n_nodos, tot_sucesores);
	
	nodos = malloc(sizeof(nodo)*n_nodos);
	if(nodos == NULL) printf("Error al asignar memória");
	sucesores = (unsigned long*)malloc(sizeof(unsigned long)*tot_sucesores);
	if(sucesores == NULL) printf("Error al asignar la memória");

	if(fread(nodos, sizeof(nodo), n_nodos, filepointer)!=n_nodos) printf("Error");
	fread(sucesores, sizeof(unsigned long), tot_sucesores, filepointer);
			
	fclose(filepointer);

	for(i=0; i<n_nodos; i++) if(nodos[i].n_sucesores){
		nodos[i].sucesores = sucesores;
		sucesores += nodos[i].n_sucesores;
	}

	//nodos de inicio y final
	id_inicio = 771979683; //(Girona)
	id_final =  429854583; //(Lleida)
	inicio = b_search(id_inicio, nodos, n_nodos, nodo_errores);
	final = b_search(id_final, nodos, n_nodos, nodo_errores);
	printf("%f\t%f\n", final->latitud, final->longitud);
	printf("%f\t%f\n", inicio->latitud, inicio->longitud);
	double distancia;
	distancia = h(inicio, final);
	printf("%fkm\n", distancia);

/*-----------------------------------------ASTAR----------------------------------------*/
	unsigned long id_sucesor;
	double coste_sucesor, coste_sucesor_h;
	nodo *sucesor;
	lista *lista_open, *ptr_lista_open, *lista_closed, *ptr_lista_closed;
	lista_open = (lista*)malloc(sizeof(lista));
	lista_open->siguiente = (lista*)malloc(sizeof(lista));
	lista_open->siguiente->coste_h = 9999999;
	if(lista_open == NULL) printf("Error creando lista open\n");
	lista_closed = (lista*)malloc(sizeof(lista));
	lista_closed->siguiente = (lista*)malloc(sizeof(lista));
	if(lista_closed == NULL) printf("Error creando lista closed\n");
	lista_closed->coste_h = 0;
	lista_closed->siguiente->coste_h = 9999999;
	lista_open->nodo_i = inicio;
	lista_open->coste_h = h(inicio, final);

	while(lista_open != NULL){
		if(lista_open->nodo_i == final) break;
		printf("Es: %ld\n", lista_open->nodo_i->id);
		for(i=0; i<lista_open->nodo_i->n_sucesores; i++){
			id_sucesor = lista_open->nodo_i->sucesores[i];
			sucesor = b_search(id_sucesor, nodos, n_nodos, nodo_errores);
			coste_sucesor = lista_open->coste + h(lista_open->nodo_i, sucesor);
			coste_sucesor_h = coste_sucesor + h(sucesor, final);
			if(sucesor->estado == 1){
				ptr_lista_open = busqueda_lista(lista_open, sucesor);
				if(ptr_lista_open->coste <= coste_sucesor) goto label;
				quitar_lista(&lista_open, sucesor);
				ptr_lista_open = poner_lista(&lista_open, sucesor, coste_sucesor_h);
			}
			else if(sucesor->estado == 2){
				ptr_lista_closed = busqueda_lista(lista_closed, sucesor);
				if(ptr_lista_closed->coste <= coste_sucesor) goto label;
				quitar_lista(&lista_closed, sucesor);
				ptr_lista_open = poner_lista(&lista_open, sucesor, coste_sucesor_h);
				ptr_lista_open->nodo_i->estado = 1;
			}
			else {
				//printf("Entrando en el else: %f\n", coste_sucesor_h);
				ptr_lista_open = poner_lista(&lista_open, sucesor, coste_sucesor_h);
				ptr_lista_open->nodo_i->estado = 1;
			}
			
			ptr_lista_open->coste = coste_sucesor;
			ptr_lista_open->padre = lista_open->nodo_i;
			
			label:;
		}
		poner_lista(&lista_closed, lista_open->nodo_i, lista_open->coste_h);
		lista_open->nodo_i->estado = 2;
		//printf("lista_open primer id -> %p\n", lista_open);
		quitar_lista(&lista_open, lista_open->nodo_i);
		//printf("lista_open primer id -> %p\n", lista_open);
		//printf("CHECK FINAL\n");
	}
	if(lista_closed->nodo_i != final) printf("Error, no funcionaaaaaAAAAHHH\n");
/*-----------------------------------------ASTAR----------------------------------------*/
	//Prints para hacer check.
	printf("%ld\n%f\t%f\n%d\n", nodos[0].id, nodos[0].longitud, nodos[0].latitud, nodos[0].n_sucesores);
	printf("Los sucesores del nodo 233242 (id=%ld) son:\n", nodos[233241].id);
	for (i=0; i<nodos[233241].n_sucesores; i++){
		printf("%ld\t", nodos[233241].sucesores[i]);
	}
	
	printf("\n");
	printf("Direccion de memoria del primer nodo %p\nDireccion de memoria del nodo 1000: %p\n", &nodos[0], &nodos[999]);
	printf("La id del primer nodo es: %ld\t La id del nodo 1000 es: %ld\n", nodos[0].id, nodos[n_nodos-1].id);
	printf("La latitud y longitud del primer nodo son: %f y %f\nDel nodo 1000 son: %f y %f\n", nodos[0].latitud, nodos[0].longitud, nodos[999].latitud, nodos[999].longitud);
	//printf("El nombre del nodo 2.958.799 es: %s\n", nodos[2958798].nombre);
	
	free(nodos);
	//free(sucesores);
	return 0;
}