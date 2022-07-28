/* Programa de ejemplo de semaphore. */
/* para compilar usar: gcc -o tarea3_ejemplo tarea3_ejemplo.c -lpthread */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct {
  int capacidad_correa;           // 
  float* correa;                  // Almacen
  int clientes;                   // Rondas random
  int demora_min_cliente;         // Productor
  int demora_max_cliente;         // Productor
  int demora_min_cajero;          // Consumidor
  int demora_max_cajero;          // Consumidor
  sem_t cliente_puede_producir;   // Puede_producir
  sem_t cajero_puede_consumir;    // Puede _consumir
} datos_compartidos_caja_t;

void* cliente(void* data);
void* cajero(void* data);
int random_entre(int min, int max);

typedef struct { 
  int capacidad_correa_c2;           // Almacen random c2=caja 2
  float* correa_c2;                  // Almacen
  int clientes_c2;                   // Rondas random
  int demora_min_cliente_c2;         // Productor
  int demora_max_cliente_c2;         // Productor
  int demora_min_cajero_c2;          // Consumidor
  int demora_max_cajero_c2;          // Consumidor
  sem_t cliente_puede_producir_c2;   // Puede_producir
  sem_t cajero_puede_consumir_c2;    // Puede _consumir
} datos_compartidos_caja2_t;

void* cliente_c2(void* data);
void* cajero_c2(void* data);
int random_entre(int min2, int max2);

int main(int argc, char* argv[]) {
  int error;
  struct timespec tiempo_ini;
  struct timespec tiempo_fin;
  pthread_t productor, consumidor;
  datos_compartidos_caja_t datos_compartidos;

  srandom(time(NULL));

  if (argc == 5) {
    
    datos_compartidos.demora_min_cliente=atoi(argv[1]);
    datos_compartidos.demora_max_cliente=atoi(argv[2]);
    datos_compartidos.demora_min_cajero=atoi(argv[3]);
    datos_compartidos.demora_max_cajero=atoi(argv[4]);
  } else {
    printf("Usar: %s capacidad_almacen rondas demora_min_productor demora_max_productor demora_min_consumidor demora_max_consumidor\n", argv[0]);
    exit(EXIT_FAILURE);
  }
//datos_compartidos.capacidad_correa=atoi(argv[1]);
datos_compartidos.capacidad_correa=rand ()% (15-5 +1) + 5; 
//datos_compartidos.clientes=atoi(argv[2]);
datos_compartidos.clientes = rand()% 11;




  datos_compartidos.correa = (float*) calloc(datos_compartidos.capacidad_correa, sizeof(float));
  sem_init(&datos_compartidos.cliente_puede_producir, 0, datos_compartidos.capacidad_correa);
  sem_init(&datos_compartidos.cajero_puede_consumir, 0, 0);

  clock_gettime(CLOCK_MONOTONIC, &tiempo_ini);

  error = pthread_create(&productor, NULL, cliente, &datos_compartidos);
  if (error == 0) {
    error = pthread_create(&consumidor, NULL, cajero, &datos_compartidos);
    if (error != 0) {
      fprintf(stderr, "error: no puede crear consumidor\n");
      error = 1;
    }
  } else {
    fprintf(stderr, "error: no puede crear productor\n");
    error = 1;
  }
  if (error == 0) {
    pthread_join(productor, NULL);
    pthread_join(consumidor, NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &tiempo_fin);

  float periodo = (tiempo_fin.tv_sec - tiempo_ini.tv_sec) + 
          (tiempo_fin.tv_nsec - tiempo_ini.tv_nsec) * 1e-9;
  printf("Tiempo de ejecución: %.9lfs\n", periodo);

  sem_destroy(&datos_compartidos.cajero_puede_consumir);
  sem_destroy(&datos_compartidos.cliente_puede_producir);
  free(datos_compartidos.correa);

  return EXIT_SUCCESS;
}

void* cliente(void* data) {
  datos_compartidos_caja_t* datos_compartidos = (datos_compartidos_caja_t*)data;
  int contador = 0;
  for (int ronda = 0; ronda < datos_compartidos->clientes; ++ronda) {
    printf("INICIO RONDA P: %i\n", ronda);
    for (int indice = 0; indice < datos_compartidos->capacidad_correa; ++indice) {
      sem_wait(&datos_compartidos->cliente_puede_producir);
      usleep(1000 * random_entre(datos_compartidos->demora_min_cliente, datos_compartidos->demora_max_cliente));
      datos_compartidos->correa[indice] = ++contador;
      printf("Indice almacen %i se produce %lg\n", indice, datos_compartidos->correa[indice]);
      sem_post(&datos_compartidos->cajero_puede_consumir);
    }
  }
  return NULL;
}

void* cajero(void* data) {
  datos_compartidos_caja_t* datos_compartidos = (datos_compartidos_caja_t*)data;
  for (int ronda = 0; ronda < datos_compartidos->clientes; ++ronda) {
    printf("\t\tINICIO RONDA C: %i\n", ronda);
    for (int indice = 0; indice < datos_compartidos->capacidad_correa; ++indice) {
      sem_wait(&datos_compartidos->cajero_puede_consumir);
      float value = datos_compartidos->correa[indice];
      usleep(1000 * random_entre(datos_compartidos->demora_min_cajero
        , datos_compartidos->demora_max_cajero));
      printf("\t\tIndice almacen %i se consume %lg\n", indice, value);
      sem_post(&datos_compartidos->cliente_puede_producir);
    }
  }
  return NULL;
}

int random_entre(int min, int max) {
  int aux=0;
  if (max > min)
    aux=random() % (max - min);
  return min + aux;
}


