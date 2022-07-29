#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct {
  int capacidad_correa;           // sss
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

//caja 2
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
// fin caja 2
int main(int argc, char* argv[]) {
  int error;
  struct timespec tiempo_ini;
  struct timespec tiempo_fin;
  pthread_t productor, consumidor;
  datos_compartidos_caja_t datos_compartidos;

  srandom(time(NULL));

  //caja 2
  int error2;
  struct timespec tiempo_ini2;
  struct timespec tiempo_fin2;
  pthread_t productor2, consumidor2;
  datos_compartidos_caja2_t datos_compartidos2;
  
  srandom(time(NULL));
  // fin caja 2
  if (argc == 5) {
    
    datos_compartidos.demora_min_cliente=atoi(argv[1]);
    datos_compartidos.demora_max_cliente=atoi(argv[2]);
    datos_compartidos.demora_min_cajero=atoi(argv[3]);
    datos_compartidos.demora_max_cajero=atoi(argv[4]);
  } else {
    printf("Usar: %s capacidad_correa clientes demora_min_cliente demora_max_cliente demora_min_cajero demora_max_cajero\n", argv[0]);
    exit(EXIT_FAILURE);
  }
//datos_compartidos.capacidad_correa=atoi(argv[1]);
datos_compartidos.capacidad_correa=rand ()% (15-5 +1) + 5; 
//datos_compartidos.clientes=atoi(argv[2]);
datos_compartidos.clientes = rand()% 11;

  // caja 2
  if (argc == 5) {
    
    datos_compartidos2.demora_min_cliente_c2=atoi(argv[1]);
    datos_compartidos2.demora_max_cliente_c2=atoi(argv[2]);
    datos_compartidos2.demora_min_cajero_c2=atoi(argv[3]);
    datos_compartidos2.demora_max_cajero_c2=atoi(argv[4]);
  } else {
    printf("Usar: %s capacidad_correa_c2 clientes_c2 demora_min_cliente_c2 demora_max_cliente_c2 demora_min_cajero_c2 demora_max_cajero_c2\n", argv[0]);
    exit(EXIT_FAILURE);
  }
    //datos_compartidos.capacidad_correa_c2=atoi(argv[1]);
    datos_compartidos2.capacidad_correa_c2=rand ()% (15-5 +1) + 5; 
    //datos_compartidos.clientes_c2=atoi(argv[2]);
    datos_compartidos2.clientes_c2 = rand()% 11;
    // fin caja 2

  datos_compartidos.correa = (float*) calloc(datos_compartidos.capacidad_correa, sizeof(float));
  sem_init(&datos_compartidos.cliente_puede_producir, 0, datos_compartidos.capacidad_correa);
  sem_init(&datos_compartidos.cajero_puede_consumir, 0, 0);

  clock_gettime(CLOCK_MONOTONIC, &tiempo_ini);

  //caja 2
  datos_compartidos2.correa_c2 = (float*) calloc(datos_compartidos2.capacidad_correa_c2, sizeof(float));
  sem_init(&datos_compartidos2.cliente_puede_producir_c2, 0, datos_compartidos2.capacidad_correa_c2);
  sem_init(&datos_compartidos2.cajero_puede_consumir_c2, 0, 0);

  clock_gettime(CLOCK_MONOTONIC, &tiempo_ini2);
  // fin caja 2

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

  //caja 2

  error2 = pthread_create(&productor2, NULL, cliente_c2, &datos_compartidos2);
  if (error2 == 0) {
    error2 = pthread_create(&consumidor2, NULL, cajero_c2, &datos_compartidos2);
    if (error2 != 0) {
      fprintf(stderr, "error2: no puede crear consumidor\n");
      error2 = 1;
    }
  } else {
    fprintf(stderr, "error2: no puede crear productor\n");
    error2 = 1;
  }
  if (error2 == 0) {
    pthread_join(productor2, NULL);
    pthread_join(consumidor2, NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &tiempo_fin2);
  // fin caja 2

  float periodo = (tiempo_fin.tv_sec - tiempo_ini.tv_sec) + 
          (tiempo_fin.tv_nsec - tiempo_ini.tv_nsec) * 1e-9;
  printf("Tiempo de ejecución: %.9lfs\n", periodo);

  sem_destroy(&datos_compartidos.cajero_puede_consumir);
  sem_destroy(&datos_compartidos.cliente_puede_producir);
  free(datos_compartidos.correa);

  return EXIT_SUCCESS;

  //CAJA 2
  float periodo2 = (tiempo_fin2.tv_sec - tiempo_ini2.tv_sec) + 
          (tiempo_fin2.tv_nsec - tiempo_ini2.tv_nsec) * 1e-9;
  printf("Tiempo de ejecución2: %.9lfs\n", periodo2);

  sem_destroy(&datos_compartidos2.cajero_puede_consumir_c2);
  sem_destroy(&datos_compartidos2.cliente_puede_producir_c2);
  free(datos_compartidos2.correa_c2);

  return EXIT_SUCCESS;
  // fin caja 2
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

//caja 2
void* cliente2(void* data) {
  datos_compartidos_caja2_t* datos_compartidos2 = (datos_compartidos_caja2_t*)data;
  int contador2 = 0;
  for (int ronda2 = 0; ronda2 < datos_compartidos2->clientes_c2; ++ronda2) {
    printf("INICIO RONDA P: %i\n", ronda2);
    for (int indice2 = 0; indice2 < datos_compartidos2->capacidad_correa_c2; ++indice2) {
      sem_wait(&datos_compartidos2->cliente_puede_producir_c2);
      usleep(1000 * random_entre(datos_compartidos2->demora_min_cliente_c2, datos_compartidos2->demora_max_cliente_c2));
      datos_compartidos2->correa_c2[indice2] = ++contador2;
      printf("Indice almacen %i se produce %lg\n", indice2, datos_compartidos2->correa_c2[indice2]);
      sem_post(&datos_compartidos2->cajero_puede_consumir_c2);
    }
  }
  return NULL;
}
//fin caja 2

void* cajero(void* data) {
  datos_compartidos_caja_t* datos_compartidos = (datos_compartidos_caja_t*)data;
  for (int ronda = 0; ronda < datos_compartidos->clientes; ++ronda) {
    printf("\t\tINICIO RONDA C: %i\n", ronda);
    for (int indice = 0; indice < datos_compartidos->capacidad_correa; ++indice) {
      sem_wait(&datos_compartidos->cajero_puede_consumir);
      float value = datos_compartidos->correa[indice];
      usleep(1000 * random_entre(datos_compartidos->demora_min_cajero, datos_compartidos->demora_max_cajero));
      printf("\t\tIndice almacen %i se consume %lg\n", indice, value);
      sem_post(&datos_compartidos->cliente_puede_producir);
    }
  }
  return NULL;
}

//caja2
void* cajero2(void* data) {
  datos_compartidos_caja2_t* datos_compartidos2 = (datos_compartidos_caja2_t*)data;
  for (int ronda2 = 0; ronda2< datos_compartidos2->clientes_c2; ++ronda2) {
    printf("\t\tINICIO RONDA C2: %i\n", ronda2);
    for (int indice2 = 0; indice2 < datos_compartidos2->capacidad_correa_c2; ++indice2) {
      sem_wait(&datos_compartidos2->cajero_puede_consumir_c2);
      float value2 = datos_compartidos2->correa_c2[indice2];
      usleep(1000 * random_entre(datos_compartidos2->demora_min_cajero_c2, datos_compartidos2->demora_max_cajero_c2));
      printf("\t\tIndice almacen %i se consume %lg\n", indice2, value2);
      sem_post(&datos_compartidos2->cliente_puede_producir_c2);
    }
  }
  return NULL;
}
//fin caja 2

int random_entre(int min, int max) {
  int aux=0;
  if (max > min)
    aux=random() % (max - min);
  return min + aux;
}