#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main(){
	 char comando[LONGITUD_COMANDO];
	 char orden[LONGITUD_COMANDO];
	 char argumento1[LONGITUD_COMANDO];
	 char argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
   EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     
     //...
     
     fent = fopen("particion.bin","r+b");

     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
    

     // Bucle de tratamiento de comandos
     for (;;){
		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) != 0);
	     if (strcmp(orden,"dir\n")==0) {
            Directorio(&directorio,&ext_blq_inodos);
            

            continue;
            }
            
        if(strcmp(orden, "info\n") == 0) {
          LeeSuperBloque(&ext_superblock);
          
          continue;
        } 
         /*//...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         */
         if (strcmp(orden,"salir\n")==0){
            //GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
 
    char *a = strtok(strcomando, " ");
    strcpy(orden, a);

    char *b = strtok(NULL, " ");

    if(b != NULL) {
      strcpy(argumento1, b);
    }
    char *c = strtok(NULL, " ");
        
    if(c != NULL) {
      strcpy(argumento2, c);
    }

    return 0;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup) {

    printf("Bloque %d bytes\n", psup->s_block_size);
    printf("Inodos particion = %d\n", psup->s_inodes_count);
    printf("Inodos libres = %d\n", psup->s_free_inodes_count);
    printf("Bloques particion = %d\n", psup->s_blocks_count);
    printf("Bloques libres = %d\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos = %d\n", psup->s_first_data_block);

}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {

    int inodo_fichero;
    int n_bloque;

    for(int i = 1; i < MAX_FICHEROS; i++) {

      inodo_fichero = directorio[i].dir_inodo;
      if(inodo_fichero != NULL_INODO){
        printf("Nombre: %s\tTamano: %d\tInodo: %d\tBloque: ", directorio[i].dir_nfich, inodos->blq_inodos[inodo_fichero].size_fichero, inodo_fichero);

        for(int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
          
          n_bloque = inodos->blq_inodos[inodo_fichero].i_nbloque[j];
          
          if(n_bloque != NULL_BLOQUE){
          printf("%d ", inodos->blq_inodos[inodo_fichero].i_nbloque[j]);
        }
      }
        printf("\n");

      }
      
    }
  }