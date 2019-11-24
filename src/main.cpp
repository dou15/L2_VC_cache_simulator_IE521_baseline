#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <L1cache.h>
#include <L2cache.h>
#include <Victimcache.h>
#include <debug_utilities.h>

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
  
   //variables
  int l1_cachesize_kb;
  int l1_associativity;
  int blocksize_bytes;
  string tipo_cache;
  
  int l2_cachesize_kb;
  int l2_associativity;
  
  string tipo_l2="l2"; //Se utiliiza mas adelante para seleccionar
  string tipo_vc="vc"; //entre simular una memoria cache con l1 y l2
                       //o un Victimcache
  
  char simbolo_numeral[10];
  int temp_loadstore;
  bool loadstore;
  long address;
  int numero_instrucciones;
  
  int input_count=0;
  
  int l1_tag_size;
  int l1_idx_size;
  int l1_offset_size;
  
  int l2_tag_size;
  int l2_idx_size;
  int l2_offset_size;
  
  int l1_cache_sets;
  int l2_cache_sets;
  
  int l1_idx;
  int l2_idx;
  int l1_tag;
  int l2_tag;
  
  bool debug;
  
  int l1_miss_rate=0;
  int misses_l1=0;
  int l2_miss_rate=0;
  int misses_l2=0;
  int global_miss_rate=0;
  int Overall_miss_rate=0;
  int hits_l1=0;
  int dirty_evictions=0;
  int hits_l2=0;
  
  //+++ FIN variables 
  
  printf("Simulador de memoria caché, en C++,\nque permita obtener métricas de rendimiento \nutilizando un trace real.\nSe simularan dos optimizaciones de caché:\nun caché multinivel y un vitimcache.\n");
  cout << endl ;
  
  /* Parse argruments */
  if(argc==9){
    l1_cachesize_kb = atoi(argv[2]);     //convierte entrada de parámetros de string a
    l1_associativity = atoi(argv[4]);    //entero
    blocksize_bytes = atoi(argv[6]);
    tipo_cache = argv[8];             //asigna el tipo de cache seleccionado (l2|vc)
    }

  if(argc!=9){
    cout << "ERROR, revisar instrucción de ejecución" << endl;
    cout << "gunzip -c mcf.trace.gz | ./cache -t <tamaño cache KB> -a <tipo asociatividad> -l <tamaño de la linea en bytes> -opt <vc|l2>" << endl;
    cout << "Ejemplo ejecución correcta" << endl;
    cout << "gunzip -c mcf.trace.gz | ./cache -t 16 -a 2 -l 32 -opt l2" << endl;
    exit(0);
    }
        
//cache l1 y l2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if(tipo_cache==tipo_l2){
  
  //operaciones intermedias,l1_cachesize_kb, l2_cache_size, l2_associativity 
    l1_cachesize_kb=l1_cachesize_kb*KB;
    l2_cachesize_kb=l1_cachesize_kb*4;
    l1_cache_sets=l1_cachesize_kb/blocksize_bytes;
    l2_cache_sets=l2_cachesize_kb/blocksize_bytes;
    l2_associativity=l1_associativity*2;
    
    //declaracion structs
    entry l1_cache_blocks[l1_cache_sets];
    entry l2_cache_blocks[l2_cache_sets];
    operation_result l1_result;
    operation_result l2_result;

    l1_l2_entry_info l1_l2_info;
    
  /* Get trace's lines and start your simulation */
  while(scanf( "%s %d %lx %d", simbolo_numeral, &temp_loadstore, &address, &numero_instrucciones)!= EOF){
    
    loadstore = temp_loadstore; //combierte en valor booleano temp_loadstore;
    
    input_count++; //toma la cuenta del total de entradas totales del trace;    
    
    /*Get tag, index and offset length para l1*/
    field_size_get( l1_cachesize_kb,
                    l1_associativity,
                    blocksize_bytes,
                    &l1_tag_size,
                    &l1_idx_size,
                    &l1_offset_size);
    
    //Get tag, index and offset length for l2
    field_size_get(l2_cachesize_kb,
                   l2_associativity,
                   blocksize_bytes,
                   &l2_tag_size,
                   &l2_idx_size,
                   &l2_offset_size);
    
    //Get tag and index from address for l1
    address_tag_idx_get(address,
                        l1_tag_size,
                        l1_idx_size,
                        l1_offset_size,
                        l1_idx,
                        l1_tag);
    
    //Get tag and index from address for l1
    address_tag_idx_get(address,
                        l2_tag_size,
                        l2_idx_size,
                        l2_offset_size,
                        l2_idx,
                        l2_tag);
    
  //Search for an address in a cache set and
  //replaces blocks using LRU policy L1
  lru_replacement_policy (l1_idx,
                          l1_tag,
                          l1_associativity,
                          loadstore,
                          &l1_cache_blocks[l1_cache_sets],
                          &l1_result,
                          debug);
  
  //Search for an address in a cache set and
  //replaces blocks using LRU policy L2
  lru_replacement_policy (l2_idx,
                          l2_tag,
                          l2_associativity,
                          loadstore,
                          &l2_cache_blocks[l2_cache_sets],
                          &l2_result,
                          debug);
  
  /* 
 * Search for an address in a cache set and
 * replaces blocks using LRU policy and a updates L2 cache 
 */
  lru_replacement_policy_l1_l2(&l1_l2_info,
															 loadstore,
															 &l1_cache_blocks[l1_idx],
															 &l2_cache_blocks[l2_idx],
															 &l1_result,
															 &l2_result,
															 debug); 
  
  lru_replacement_policy_l1_l2(&l1_l2_info,
															loadstore,
															&l1_cache_blocks[l1_idx],
															&l2_cache_blocks[l2_idx],
															&l1_result,
															&l2_result,
															debug); 
    
  } //Fin WHILE
  
  cout << "----------------------------------" << endl;
  cout << "Cache parameters:" << endl;
  cout << "----------------------------------" << endl;
  cout << "L1 Cache Size (KB): " << l1_cachesize_kb << endl;
  cout << "L2 Cache Size (KB): " << l2_cachesize_kb << endl;
  cout << "Cache L1 Associativity: " << l1_associativity << endl;
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
  
}// Fin cache l1 l2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//cache l1 y vc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if(tipo_cache==tipo_vc){
    
    int vc_associativity = 16;
    
    //operaciones intermedias
    l1_cachesize_kb=l1_cachesize_kb*KB;
    l1_cache_sets=l1_cachesize_kb/blocksize_bytes;
    
    //declaracion structs
    entry l1_cache_blocks[l1_cache_sets];
    operation_result l1_result;
    entry vc_cache_blocks[vc_associativity];
    operation_result vc_result;
    l1_vc_entry_info l1_vc_info;
    
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
  cout << "Total de entradas del trace " << input_count << endl;
  
return 0;
} // **** FIN main ****//
