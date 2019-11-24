/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: II-2019
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <debug_utilities.h>
#include <L1cache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;
int field_size_get( int cachesize_kb,
                    int associativity,
                    int blocksize_bytes,
                    int *tag_size,
                    int *idx_size,
                    int *offset_size)
{ 
  *offset_size = log2(blocksize_bytes);
  *idx_size = log2(cachesize_kb) - (log2(associativity)+log2(blocksize_bytes));
  *tag_size = ADDRSIZE - *offset_size - *idx_size;
 
  return OK;
} /*-------------Fin field_size_get --------------*/

void address_tag_idx_get(long address,
                        int tag_size,
                        int idx_size,
                        int offset_size,
                        int &idx,
                        int &tag)
{
  int temp_idx = address >> offset_size;
  temp_idx = temp_idx & ((1 << idx_size) - 1);
  idx = temp_idx;

  int temp_tag = address >> (offset_size+idx_size);
  temp_tag = temp_tag & ((1 << tag_size) - 1);
  tag = temp_tag;
  
  return;
} /*-------------Fin address_tag_idx_get --------------*/

int srrip_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
   return ERROR;
}


int lru_replacement_policy (int idx,
                            int tag,
                            int associativity,
                            bool loadstore,
                            entry* cache_blocks,
                            operation_result* result,
                            bool debug)
{
  //Ciclo verificación hit
for(int i=0;i<associativity;i++){
    
    //*****Caso Hit  
    if(tag==cache_blocks[idx+i].tag){ 
        
    //cocurre HIT_STORE
       if(cache_blocks[idx+i].valid==true && loadstore==true){
         result -> miss_hit=HIT_STORE;
         break;
         }//if 1.1
    
      //ocurre HIT_LOAD
       if(cache_blocks[idx+i].valid==true && loadstore==false){
          result -> miss_hit=HIT_LOAD;
          break;
          }//if 1.2
       }//if 1
}//FIN Ciclo verificación hit  

    //*****Caso Miss
    if(tag!=cache_blocks[idx].tag){
        
       //ocurre MISS_STORE
       if(loadstore==true){
          result -> miss_hit=MISS_STORE;
          }//if 2.1 
    
       //ocurre MISS_LOAD
       if(loadstore==false){
          result -> miss_hit=MISS_LOAD;
          }//if 2.2
       }//if 2
  
  
   return ERROR;
} /*-------------Fin lru_replacement_policy --------------*/
