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
#include <L2cache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_l2(const l1_l2_entry_info *l1_l2_info,
																 bool loadstore,
																 entry* l1_cache_blocks,
																 entry* l2_cache_blocks,
																 operation_result* l1_result,
																 operation_result* l2_result,
																 bool debug) 
{
  int temp_set, l1_idx, l2_idx, temp_evicted_address, l1_tag;
  
  //Hit_Store || HIT_LOAD en l1
  if(l1_result->miss_hit == HIT_STORE || l1_result->miss_hit == HIT_LOAD){
    for(int i=0;i < l1_l2_info->l1_associativity; i++){
        if(l1_cache_blocks[l1_idx+i].tag == l1_l2_info->l1_tag){
            temp_set = l1_cache_blocks[l1_idx+i].rp_value; 
            l1_cache_blocks[l1_idx+i].rp_value = l1_l2_info->l1_associativity - 1;
            if(loadstore==true){
              l1_result->dirty_eviction = true;
              }
        }
    } 
  for(int i=0; i < l1_l2_info->l1_associativity; i++){ 
      if(l1_cache_blocks[l1_idx+i].tag != l1_l2_info->l1_tag && l1_cache_blocks[l1_idx+i].rp_value > temp_set){
        l1_cache_blocks[l1_idx+i].rp_value = l1_l2_info->l1_associativity - 1;
        }
      }
  } // FIN Hit_Store || HIT_LOAD en l1
  
  //MISS_STORE || MISS_LOAD L1
  if(l1_result->miss_hit == MISS_STORE || l1_result->miss_hit == MISS_LOAD){
  
    //HIT_STORE l2 || HIT_LOAD l2
    if(l2_result->miss_hit == HIT_STORE || l2_result->miss_hit == HIT_LOAD){
       for(int i=0; i < l1_l2_info->l1_associativity; i++){
           if(l2_cache_blocks[l2_idx+i].tag == l1_l2_info->l2_tag){
              temp_set = l2_cache_blocks[l2_idx+i].rp_value; 
              l2_cache_blocks[l2_idx+i].rp_value = l1_l2_info->l2_associativity - 1;
              }
           }       
       if(loadstore==true){
          l2_result->dirty_eviction=true;
          }    
       for(int i=0; i < l1_l2_info->l2_associativity; i++){  //cambia valore rp_value l2
           if(l2_cache_blocks[l2_idx+i].tag != l1_l2_info->l2_tag && l2_cache_blocks[l2_idx+i].rp_value > temp_set){
              l2_cache_blocks[l2_idx+i].rp_value = l2_cache_blocks[l2_idx+i].rp_value - 1; 
              }
           }
       for(int i=0; i < l1_l2_info->l2_associativity; i++){ //determina el way a sustituir en l1
           if(l1_cache_blocks[l1_idx+i].rp_value==false){
              temp_evicted_address=l1_idx+i;
              }
           }
       //escribe los valores en l1    
       l1_cache_blocks[temp_evicted_address].valid = true;
       l1_cache_blocks[temp_evicted_address].dirty = true;
       l1_cache_blocks[temp_evicted_address].tag = l1_tag;
       l1_cache_blocks[temp_evicted_address].rp_value = l1_l2_info->l1_associativity - 1; 
       
       for(int i=0; i < l1_l2_info->l2_associativity ;i++){ ////cambia valore rp_value l1
           if(l1_cache_blocks[l1_idx+i].tag != l1_tag){
              l1_cache_blocks[l1_idx+i].rp_value = l1_cache_blocks[l1_idx+i].rp_value - 1;
              }
           }
    } //FIN HIT_STORE l2 || HIT_LOAD l2 
    
    //MISS_STORE || MISS_LOAD  //falla en esta parte
    if(l2_result->miss_hit == MISS_STORE || l2_result->miss_hit == MISS_LOAD){  
      
      
      //invalida dato l1
    /*  for(int i=0; i < l1_l2_info->l1_associativity ;i++){ //invalida dato l1
          if(l1_cache_blocks[l1_idx+i].tag == l1_tag){
              l1_cache_blocks[l1_idx+i].valid = 0;
            }
           }*/
           
      //determina bloque victimizar en l2 
       for(int i=0; i < l1_l2_info->l2_associativity; i++){
            if(l2_cache_blocks[l2_idx+i].rp_value == false){
               temp_evicted_address = l2_idx+i;
              }
            }
            
       //escribe nuevos valores en l2    
     /*   l2_cache_blocks[temp_evicted_address].valid = true;
        if(loadstore==true){l2_cache_blocks[temp_evicted_address].dirty = true;}
        l2_cache_blocks[temp_evicted_address].tag = l1_l2_info->l2_tag;
        l2_cache_blocks[temp_evicted_address].rp_value = l1_l2_info->l2_associativity - 1;  
        
       for(int i=0; i < l1_l2_info->l2_associativity ;i++){ ////cambia valores rp_value l2
           if(l2_cache_blocks[l2_idx+i].tag != l1_l2_info->l2_tag){
              l2_cache_blocks[l2_idx+i].rp_value = l2_cache_blocks[l2_idx+i].rp_value - 1;
              }
           } */
      
    }// FIN MISS_STORE || MISS_LOAD L2  
    
  }//Fin Miss Store l1 
	return OK;
}
