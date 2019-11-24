                            /* Parse argruments */

/*------ CODIGO PRUEBAS ------*/
cout << endl ;
cout << "cachesize_kb " << cachesize_kb << "  " << 2+cachesize_kb << endl;
cout << "Associativity " << Associativity << "  " << 2*Associativity << endl;
cout << "Blocksize_Bytes " << Blocksize_Bytes << "  " << 3+Blocksize_Bytes << endl;
string str = "esto es un string ";
cout << "tipo de cache " << str + Tipo_Cache << endl; 
/*------ FIN CODIGO PRUEBAS ------*/

//imprime input trace
cout << Simbolo_Numeral << "  " << Temp_Loadstore << "  " << hex << Address << dec << "  " << Numero_Instrucciones << endl;

  cout << hex << l1_idx << endl;
  cout << l1_tag << endl;
  cout << l2_idx << endl;
  cout << l2_tag << endl;
