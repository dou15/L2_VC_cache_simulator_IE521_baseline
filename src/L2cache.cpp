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
#include "../include/debug_utilities.h"
#include "../include/L1cache.h"
#include "../include/L2cache.h"

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_l2(const l1_l2_entry_info *l1_l2_info,
																 bool loadstore,
																 entry* l1_cache_blocks,
																 entry* l2_cache_blocks,
																 operation_result* l1_result,
																 operation_result* l2_result,
																 bool debug=false) 
{
int id=l1_l2_info->idx;    
int l2_id=l1_l2_info->l2_idx;   
int l1_estado=l1_result->miss_hit;
int l2_estado=l2_result->miss_hit;
int l1_assoc = l1_l2_info->l1_associativity; 
int l2_assoc = l1_l2_info->l2_associativity;
int l1_tagint= l1_l2_info->l1_tag;
int l2_tagint= l1_l2_info->l2_tag;
int temp_set;
int temp_evicted_address;
int l1_temp_evicted_address;
uint8_t promocion;

//Hit_Store || HIT_LOAD en l1
if(l1_estado==HIT_STORE || l1_estado==HIT_LOAD){
 for(int i=0;i<l1_assoc;i++){
    if(l1_cache_blocks[id+i].tag==l1_tagint){
       temp_set=l1_cache_blocks[id+i].rp_value; 
       promocion=l1_assoc-1;
       l1_cache_blocks[id+i].rp_value=promocion;
       if(loadstore==true){
           l1_result->dirty_eviction=true;
      }
      if(loadstore==false){
           l1_result->dirty_eviction=false;
      }
    }
 } 
 for(int i=0;i<l1_assoc;i++){ 
    if(l1_cache_blocks[id+i].tag!=l1_tagint && l1_cache_blocks[id+i].rp_value>temp_set){
       l1_cache_blocks[id+i].rp_value= l1_cache_blocks[id+i].rp_value-1;
    }
 }
} // FIN Hit_Store || HIT_LOAD en l1
    
//MISS_STORE || MISS_LOAD L1
if(l1_estado==MISS_STORE || l1_estado==MISS_LOAD){
    
    //HIT_STORE l2 || HIT_LOAD l2
    if(l2_estado==HIT_STORE || l2_estado==HIT_LOAD){
       for(int i=0;i<l2_assoc;i++){
           if(l2_cache_blocks[l2_id+i].tag==l2_tagint){
              temp_set=l2_cache_blocks[id+i].rp_value; 
              promocion=l2_assoc-1;
              l2_cache_blocks[l2_id+i].rp_value=promocion;
              }
       }       
       if(loadstore==true){
          l2_result->dirty_eviction=true;
          }    
       if(loadstore==false){
          l2_result->dirty_eviction=false;
          }
       for(int i=0;i<l2_assoc;i++){  //cambia valore rp_value l2
           if(l2_cache_blocks[l2_id+i].tag!=l2_tagint && l2_cache_blocks[l2_id+i].rp_value>temp_set){
              l2_cache_blocks[l2_id+i].rp_value= l2_cache_blocks[l2_id+i].rp_value-1; 
              }
       }
       for(int i=0;i<l1_assoc;i++){ //determina el way a sustituir en l1
           if(l1_cache_blocks[id+i].rp_value==false){
              temp_evicted_address=id+i;
             // l1_result->evicted_address=temp_evicted_address; 
              }
           }
       //escribe los valores en l1    
       l1_cache_blocks[temp_evicted_address].valid=true;
       l1_cache_blocks[temp_evicted_address].dirty=true;
       l1_cache_blocks[temp_evicted_address].tag=l1_tagint;
       l1_cache_blocks[temp_evicted_address].rp_value=l1_assoc-1; 
       
       for(int i=0;i<l1_assoc;i++){ ////cambia valore rp_value l1
           if(l1_cache_blocks[id+i].tag!= l1_tagint){
              l1_cache_blocks[id+i].rp_value = l1_cache_blocks[id+i].rp_value-1;
              }
       }
    } //FIN HIT_STORE l2 || HIT_LOAD l2  
    
    //MISS_STORE || MISS_LOAD L2
    if(l2_estado==MISS_STORE || l2_estado==MISS_LOAD){    
        
       for(int i=0;i<l1_assoc;i++){ //invalida dato l1
           if(l1_cache_blocks[id+i].rp_value==0){
              l1_cache_blocks[id+i].valid=false;
              l1_temp_evicted_address=id+i; //bloque victimiza l1
              }
           }
           
       //determina bloque victimizar en l2 
       for(int i=0;i<l2_assoc;i++){
            if(l2_cache_blocks[l2_id+i].rp_value==false){
               temp_evicted_address= l2_id+i;
              }
            }
        //escribe nuevos valores en l2    
        l2_cache_blocks[temp_evicted_address].valid=true;
        if(loadstore==true){l2_cache_blocks[temp_evicted_address].dirty=true;}
        if(loadstore==false){l2_cache_blocks[temp_evicted_address].dirty=false;}
        l2_cache_blocks[temp_evicted_address].tag=l2_tagint;
        l2_cache_blocks[temp_evicted_address].rp_value=l2_assoc-1; 
       
       for(int i=0;i<l2_assoc;i++){ ////cambia valores rp_value l2
           if(l2_cache_blocks[l2_id+i].tag != l2_tagint){
              l2_cache_blocks[l2_id+i].rp_value = l2_cache_blocks[l2_id+i].rp_value-1;
              }
           }
       
       //escribe nuevos valores en l1    
       l1_cache_blocks[l1_temp_evicted_address].valid=true;
       if(loadstore==true){l1_cache_blocks[l1_temp_evicted_address].dirty=true;}
       if(loadstore==false){l1_cache_blocks[l1_temp_evicted_address].dirty=false;}
       l1_cache_blocks[l1_temp_evicted_address].tag=l1_tagint;
       l1_cache_blocks[l1_temp_evicted_address].rp_value=l1_assoc-1;
       
       for(int i=0;i<l1_assoc;i++){ ////cambia valores rp_value l1
           if(l1_cache_blocks[id+i].tag != l1_tagint){
              l1_cache_blocks[id+i].rp_value = l1_cache_blocks[id+i].rp_value-1;
              }
           }   
    }       
    
}//Fin Miss Store l1 
   
	return OK;
}
