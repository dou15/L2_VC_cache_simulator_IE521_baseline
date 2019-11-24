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
#include <Victimcache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_vc(const l1_vc_entry_info *l1_vc_info,
      	                      	 bool loadstore,
        	                    	 entry* l1_cache_blocks,
          	                  	 entry* vc_cache_blocks,
            	                	 operation_result* l1_result,
              	              	 operation_result* vc_result,
                	            	 bool debug)
{
     //Hit_Store || HIT_LOAD en l1
 /* if(l1_result->miss_hit == HIT_STORE || l1_result->miss_hit == HIT_LOAD){
    for(int i=0;i < l1_vc_info->l1_associativity; i++){
        if(l1_cache_blocks[l1_idx+i].tag == l1_vc_info->l1_tag){
            temp_set = l1_cache_blocks[l1_idx+i].rp_value; 
            l1_cache_blocks[l1_idx+i].rp_value = l1_vc_info->l1_associativity - 1;
            if(loadstore==true){
              l1_result->dirty_eviction = true;
              }
        }
    } 
  for(int i=0; i < l1_vc_info->l1_associativity; i++){ 
      if(l1_cache_blocks[l1_idx+i].tag != l1_vc_info->l1_tag && l1_cache_blocks[l1_idx+i].rp_value > temp_set){
        l1_cache_blocks[l1_idx+i].rp_value = l1_vc_info->l1_associativity - 1;
        }
      }
  } // FIN Hit_Store || HIT_LOAD en l1 */
    
   return OK;
}
