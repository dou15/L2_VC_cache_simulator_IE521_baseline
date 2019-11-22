#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include "../include/L1cache.h"
#include "../include/L2cache.h"
#include "../include/Victimcache.h"
#include "../include/debug_utilities.h"

#define KB 1024
#define ADDRSIZE 32

using namespace std;

/* Helper funtions */
void print_usage ()
{
  printf ("Print print_usage\n");
  exit (0);
}

int main(int argc, char * argv []) {
  
    printf("Simulador de memoria caché, en C++,\nque permita obtener métricas de rendimiento \nutilizando un trace real.\nSe simularan dos optimizaciones de caché:\nun caché multinivel y un vitimcache.\n");
  
  cout << "---------------------------------------" << endl;

 //variables 
 char simbolo_numeral [10];
 int numero_instrucciones;
 string tipo_cache;
 
 string tipo_l2="l2";
 string tipo_vc="vc";
  
 int cachesize_kb;
 int associativity;
 int blocksize_bytes;
 
 int l1_cache_sets;
 int l2_cachesize_kb;
 int l2_cache_sets;
 int l2_associativity;
 
 int tag_size;
 int idx_size;
 int offset_size;
 
 int l2_tag_size;
 int l2_idx_size;
 int l2_offset_size;
 
 long address;
 int idx;
 int tag;
 int l2_idx;
 int l2_tag;
 
 bool loadstore;
 int temp_loadstore;
 bool debug;
 
  //int Temp_Block_Number;
 
 int Overall_miss_rate;
 int input_count=0; //contadores
 int l1_miss_rate=0;
 int l2_miss_rate=0;
 int global_miss_rate=0;
 int misses_l1=0;
 int hits_l1=0;
 int misses_l2=0;
 int hits_l2=0;
 int dirty_evictions=0; //solo se registra uno por tipo de memoria
 int misses_vc;
 int hits_vc;
 //+++ FIN variables 
 
   /* Parse argruments */
  if(argc==9){
	cachesize_kb = atoi(argv[2]);     //convierte entrada de parámetros de string a
	associativity = atoi(argv[4]); //entero
	blocksize_bytes = atoi(argv[6]);
	tipo_cache = argv[8];   //asigna el tipo de cache seleccionado (l2|vc)
}

if(argc!=9){
	cout << "ERROR, revisar instrucción de ejecución" << endl;
	cout << "gunzip -c mcf.trace.gz | cache -t <tamaño cache KB> -a <tipo asociatividad> -l <tamaño de la linea en bytes> -rp <vc|l2>" << endl;
	cout << "ejemplo" << endl;
	cout << "gunzip -c mcf.trace.gz | ./cache -t 16 -a 2 -l 32 -rp l2" << endl;
exit(0);
}

//operaciones intermedias,cachesize_kb, l1_cache_size, l2_cache_size, l2_associativity 
cachesize_kb=cachesize_kb*KB; 
l2_cachesize_kb=cachesize_kb*4;
l1_cache_sets=(cachesize_kb/blocksize_bytes);
l2_cache_sets=(l2_cachesize_kb/blocksize_bytes);
l2_associativity=associativity*2;

//declaracion structs
entry l1_cache_blocks[l1_cache_sets];
entry l2_cache_blocks[l2_cache_sets];
operation_result l1_result;
operation_result l2_result;

l1_l2_entry_info l1_l2_info;

//cache l1 y l2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if(tipo_cache==tipo_l2){

/* Get trace's lines and start your simulation */
  while(scanf( "%s %d %lx %d", &simbolo_numeral, &temp_loadstore, &address, &numero_instrucciones)!= EOF){
 
  loadstore=temp_loadstore;
  input_count++;
/*Get tag, index and offset length para l1*/
field_size_get(cachesize_kb,
               associativity,
               blocksize_bytes,
               &tag_size,
               &idx_size,
               &offset_size);

//Get tag, index and offset length for l2
field_size_get(l2_cachesize_kb,
               l2_associativity,
               blocksize_bytes,
               &l2_tag_size,
               &l2_idx_size,
               &l2_offset_size);

//Get tag and index from address for l1
address_tag_idx_get(address,
                    tag_size,
                    idx_size,
                    offset_size,
                    &idx,
                    &tag);

//Get tag and index from address for l2
address_tag_idx_get(address,
                    l2_tag_size,
                    l2_idx_size,
                    l2_offset_size,
                    &l2_idx,
                    &l2_tag);


//llenando l1_l2_info
l1_l2_info.idx = idx;
l1_l2_info.l1_tag = tag;
l1_l2_info.l1_associativity = associativity;
l1_l2_info.l2_idx = l2_idx;
l1_l2_info.l2_tag = l2_tag;
l1_l2_info.l2_associativity = l2_associativity;

//cout << hex << idx << endl;
//cout << hex << l2_idx << endl;

/// l2 usa funcion lru verificar hit en l1 y l2, se tiene un l1_result y l2_result para la toma
/// desiciones

//Search for an address in a cache set and
//replaces blocks using LRU policy

//L1
lru_replacement_policy (idx,
                        tag,
                        associativity,
                        loadstore,
                        &l1_cache_blocks[l1_cache_sets],
                        &l1_result,
                        debug);  

//L2
lru_replacement_policy (l2_idx,
                        l2_tag,
                        l2_associativity,
                        loadstore,
                        &l2_cache_blocks[l2_cache_sets],
                        &l2_result,
                        debug);

//cout << l1_result.miss_hit << endl;


/* 
 * Search for an address in a cache set and
 * replaces blocks using LRU policy and a updates L2 cache 
 */
lru_replacement_policy_l1_l2(&l1_l2_info,
                                   loadstore,
																   &l1_cache_blocks[l1_cache_sets],
																   &l2_cache_blocks[l2_cache_sets],
                                   &l1_result,
                                   &l2_result,
																   debug);

  if(l1_result.miss_hit == HIT_STORE){hits_l1++; dirty_evictions++;}
  if(l1_result.miss_hit == HIT_LOAD){hits_l1++; dirty_evictions++;}
  
  if(l1_result.miss_hit == MISS_STORE && l2_result.miss_hit == HIT_STORE){misses_l1++; hits_l2++; dirty_evictions++;}
  
  if(l1_result.miss_hit == MISS_STORE && l2_result.miss_hit == HIT_LOAD){misses_l1++; hits_l2++; dirty_evictions++;}
  
  if(l1_result.miss_hit == MISS_LOAD && l2_result.miss_hit == MISS_STORE){misses_l1++; misses_l2++;}

  if(l1_result.miss_hit == MISS_LOAD && l2_result.miss_hit == MISS_LOAD){misses_l1++; misses_l2++;}
  
  } //+++++FIN WHILE L1 Y L2
  
  l1_miss_rate=misses_l1/input_count;
  l2_miss_rate=misses_l2/misses_l1;
  global_miss_rate=misses_l2/input_count;
  Overall_miss_rate=l1_miss_rate+l2_miss_rate;
  
  cout << "----------------------------------" << endl;
  cout << "Cache parameters:" << endl;
  cout << "----------------------------------" << endl;
  cout << "L1 Cache Size (KB): " << cachesize_kb << endl;
  cout << "L2 Cache Size (KB): " << l2_cachesize_kb << endl;
  cout << "Cache L1 Associativity: " << associativity << endl;
  cout << "Cache L2 Associativity: " << l2_associativity << endl;
  cout << "Cache Block Size (bytes): " << blocksize_bytes << endl;
  cout << "----------------------------------" << endl;
  cout << "Simulation results:" << endl;
  cout << "----------------------------------" << endl;
  cout << "Overall miss rate: " << Overall_miss_rate << endl;
  cout << "L1 miss rate: " << l1_miss_rate << endl;
  cout << "L2 miss rate: " << l2_miss_rate << endl;
  cout << "Global miss rate " << global_miss_rate << endl;
  cout << "Misses (L1): " << misses_l1 << endl;
  cout << "Hits (L1): " << hits_l1 << endl;
  cout << "Misses (L2): " << misses_l2  << endl;
  cout << "Hits (L2): " << hits_l2 << endl;
  cout << "Dirty evictions (L2): " << dirty_evictions << endl;
  cout << "----------------------------------" << endl;

  
  
  } // Fin cache l1 y vc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  //cache l1 y vc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if(tipo_cache==tipo_vc){
      
      cout << "vitim cache en construcción" << endl;
      
  cout << "----------------------------------" << endl;
  cout << "Cache parameters:" << endl;
  cout << "----------------------------------" << endl;
  cout << "L1 Cache Size (KB): " << endl;
  cout << "Cache L1 Associativity: " << endl;
  cout << "Cache Block Size (bytes): " << endl;
  cout << "----------------------------------" << endl;
  cout << "Simulation results:" << endl;
  cout << "----------------------------------" << endl;
  cout << "Miss rate (L1+VC): "  << endl;
  cout << "Misses (L1+VC): " << endl;
  cout << "Hits (L1+VC): " << endl;
  cout << "Victim cache hits: " << endl;
  cout << "Dirty evictions: " << endl;
 
     } // Fin cache l1 y vc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  

  
  /* Print cache configuration */


  /* Print Statistics */ 
  
  //cout << "total entradas: " << input_count << endl;

  return 0;
}
