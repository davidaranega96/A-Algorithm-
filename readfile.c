#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct{
	unsigned long id;
	char *nombre;
	double longitud, latitud;
	unsigned short n_sucesores;
	unsigned long *sucesores;
	short estado;
}nodo;

//Función que cuenta el número de nodos.
int numnodos(FILE *filepointer){
	int n=0; 
	size_t max = 100000;
	char *buffer = NULL;
	while(getline(&buffer, &max, filepointer)!=-1){
		if(strncmp(buffer, "n", 1) == 0){
			n++;
		}
		else break;
	}
	return n;
}
//Búsqueda por el método de la bisección. Devuelve un puntero de nodo con el que tiene una id a la de entrada.
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

//Recuento de los sucesores de cada nodo.
void nn_sucesores(FILE *filepointer, nodo *nodos, int n_nodos, nodo *nodo_errores){
	char *buffer = NULL, *buffer_part = NULL, *puntero;
	int line;
	int oneway=1, line_counter = 0;;
	size_t max=0, a; 
	unsigned long id; 
	nodo *busqueda, *busqueda2;
	for (line = 0; line<=n_nodos; line++){
	getline(&buffer, &max, filepointer);
	}
	int flag = 1, error;
	while(strncmp(buffer, "w", 1) == 0){
			buffer_part = strsep(&buffer, "|");
			while(buffer_part != NULL){
				if((buffer_part[0]!=0)&&(flag>10)){
					id = strtoul(buffer_part, &puntero, 10);
					busqueda2 = b_search(id, nodos, n_nodos, nodo_errores);
					if(busqueda2 == nodo_errores) {}
					else {
					busqueda->n_sucesores++;
					busqueda = busqueda2;
					if(error!=1) busqueda -> n_sucesores = busqueda->n_sucesores + oneway;
					error = 0;
					}
				}
				else if(flag==10){
					id = strtoul(buffer_part, &puntero, 10);
					busqueda = b_search(id, nodos, n_nodos, nodo_errores);
					if(busqueda == nodo_errores) error = 1;
				}
				else if((flag==8)&&(buffer_part[0]!=0)) {oneway = 0;}
			buffer_part = strsep(&buffer, "|");
			flag++;
			}
			getline(&buffer, &max, filepointer);
			line_counter++;
			flag = 1;
			oneway = 1;
		}
	free(buffer);
	free(buffer_part);
	printf("%d lineas en total\n", line_counter);
}

void v_sucesores(FILE *filepointer, nodo *nodos, int n_nodos, nodo *nodo_errores){
	char *buffer = NULL, *buffer_part = NULL, *puntero;
	int line;
	int oneway=1;
	size_t max=0, a; 
	unsigned long id, *v, id2; 
	nodo *busqueda, *busqueda2;
	for (line = 0; line<=n_nodos; line++){
	getline(&buffer, &max, filepointer);
	}
	int flag = 1, error=0;
	while(strncmp(buffer, "w", 1) == 0){
			buffer_part = strsep(&buffer, "|");
			while(buffer_part != NULL){
				if(flag == 10) {
					id = strtoul(buffer_part, &puntero, 10);
					busqueda = b_search(id, nodos, n_nodos, nodo_errores);
					if(busqueda==nodo_errores) error = 1;
					v = busqueda->sucesores;
				}
				else if((flag==8)&&(buffer_part[0]!=0)) {oneway = 0;}
				else if((buffer_part!=0)&&(flag>10)){
					id2 = id;
					id = strtoul(buffer_part, &puntero, 10);
					busqueda2 = b_search(id, nodos, n_nodos, nodo_errores);
					if(busqueda2==nodo_errores) {}
					else{
					if(error!=1) {v[busqueda->n_sucesores] = id; busqueda->n_sucesores++;}
					busqueda = busqueda2;
					v = busqueda->sucesores;
					if(oneway==1&&error!=1) {v[busqueda->n_sucesores] = id2; busqueda->n_sucesores++;}
					error = 0;
					}
				}
			buffer_part = strsep(&buffer, "|");
			flag++;
			}
			getline(&buffer, &max, filepointer);
			flag = 1;
			oneway = 1;
		}
	free(buffer);
	free(buffer_part);
}

//Primera lectura del fichero. 
void lectura_nodos(FILE *filepointer, nodo *nodos, int n_nodos){
	rewind(filepointer);
	char *buffer, *buffer_part;
	unsigned short flag; int i; char *puntero; size_t max=10000;
	for(i=0; i<n_nodos; i++){
		getline(&buffer, &max, filepointer);
			for (flag=1; flag < 12; flag++){
				buffer_part = strsep(&buffer, "|");
				if(*buffer_part == 0) ;
				else if (flag == 2) nodos[i].id = strtoul(buffer_part, &puntero, 10);
				else if(flag == 10) nodos[i].latitud = atof(buffer_part);
				else if(flag == 11) nodos[i].longitud = atof(buffer_part);
				else if(flag == 3) nodos[i].nombre = buffer_part;
			}
	}
}
int main(){
	FILE*filepointer, *ptr;
	filepointer = fopen("cataluna.csv" , "r");
	if (filepointer == NULL) printf("Error al abrir el achivo.\n");
	int n_nodos;
	clock_t start, end;
    double cpu_time_used;
     
    start = clock();
	
	//Calcula el número de nodos.
	n_nodos = numnodos(filepointer);
	printf("Numero de nodos: %d\n", n_nodos);

	//Asignación de la memória para los nodos.
	nodo *nodos;
	nodos = malloc(sizeof(nodo) * n_nodos);
	if(nodos == NULL) printf("Error al asignar la memoria para los nodos");
	nodo *nodo_errores;
	nodo_errores = malloc(sizeof(nodo));
	if(nodo_errores == NULL) printf("Error al asignar la memoria para los nodos");
	
	//Lectura del fichero.
	lectura_nodos(filepointer, nodos, n_nodos);
	
	int i;
	
	rewind(filepointer); 
	//check del numero de sucesores.
	nn_sucesores(filepointer, nodos, n_nodos, nodo_errores);
	printf("Número de sucesores del nodo 233242, con (id=%ld): %hu\n", nodos[233241].id, nodos[233241].n_sucesores);
	printf("Numero de nodos que no aparecen en la lista: %hu\n", nodo_errores->n_sucesores);
	
	//Asignación de memória para los vectores con los nodos.
	
	unsigned long tot_sucesores = 0;
	for(i=0; i<n_nodos; i++){
		nodos[i].sucesores = malloc(sizeof(unsigned long) * nodos[i].n_sucesores);
		tot_sucesores = tot_sucesores + nodos[i].n_sucesores;
		if(nodos[i].sucesores == NULL) printf("Error al asignar la memória para los sucesores");
		//Se resetea el valor de n_sucesores a 0.
		nodos[i].n_sucesores = 0; nodos[i].estado = 0;
	}
	
	printf("En total hay %ld sucesores\n", tot_sucesores);
	
	nodo_errores->sucesores = malloc(sizeof(unsigned long));
	if(nodo_errores->sucesores == NULL) printf("Error!");
	
	rewind(filepointer);
	//Función que va a rellenar los vectores de sucesores con las ids.
	v_sucesores(filepointer, nodos, n_nodos, nodo_errores);
	
	printf("Los sucesores del nodo 233242 son:\n");
	for (i=0; i<nodos[233241].n_sucesores; i++){
			printf("%ld\t", nodos[233241].sucesores[i]);
	}
	printf("\n");

	//Creación del archivo .bin
	ptr = fopen("info_nodos.bin", "wb");
	if(fwrite(&n_nodos, sizeof(int), 1, ptr) + fwrite(&tot_sucesores, sizeof(unsigned long), 1, ptr) != 2)
		printf("Error initializing binary file");
	if(fwrite(nodos, sizeof(nodo), n_nodos, ptr) != n_nodos)
		printf("Error initializing nodes");
	for(i=0; i<n_nodos; i++) {
		if(fwrite(nodos[i].sucesores, sizeof(unsigned long), nodos[i].n_sucesores, ptr) != nodos[i].n_sucesores)
		 printf("Error initializing succesors");
	}
	fclose(ptr);
	
	//Prints para hacer check.
	printf("Direccion de memoria del primer nodo %p\nDireccion de memoria del nodo 1000: %p\n", &nodos[0], &nodos[999]);
	printf("La id del primer nodo es: %ld\t La id del nodo 1000 es: %ld\n", nodos[0].id, nodos[n_nodos-1].id);
	printf("La latitud y longitud del primer nodo son: %f y %f\nDel nodo 1000 son: %f y %f\n", nodos[0].latitud, nodos[0].longitud, nodos[999].latitud, nodos[999].longitud);

	rewind(filepointer);
	fclose(filepointer);
	free(nodos);
	
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Reading the file took %f seconds.\n", cpu_time_used); 
	return 0;
}