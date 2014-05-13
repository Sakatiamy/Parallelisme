__kernel void histograms( 
    __global const char *allwords, // mots du dictionnaires
    __global int *bufferHistogramsChar, // histogramme des occurences des caracteres
    __global int *bufferHistogramsLength, // histogramme de la longueur des mots
	uint L) // longest word
{
    int gid = get_global_id(0);
    int code_ascii = (int)allwords[gid];
    int code_ascii_suivant = (int)allwords[gid+1];
    atomic_inc(bufferHistogramsChar+code_ascii);
    int longueur = 0;
    if(code_ascii != 0){
        if((gid % L) == L-1){
            longueur = L;
            atomic_inc(bufferHistogramsLength+longueur);
        }

        else {
            if(code_ascii_suivant == 0){
                longueur = (gid+1)%L; 
                atomic_inc(bufferHistogramsLength+longueur);
            }
        }
    }
}

__kernel void lcss( 
    __global const char *allwords, 
    __global const char *query, 
    __global       int  *lcss,
    int Q, // size of query string
    int L) // longest word
{
    int gid = get_global_id(0);
    int sizeMot = 0;
    int finMot = 0;
    while(finMot == 0){
        int ascii = (int)allwords[gid*L + sizeMot];
        if((ascii == 0) || (sizeMot == L)){
            finMot = 1;
        }
        sizeMot = sizeMot + 1;
    }
    sizeMot = sizeMot - 1; 
    // On soustrait de 1 pour rester cohérent avec le code CPU 
    // puisqu'on a décidé de ne pas compter le caractère '/n'
    lcss[gid] = sizeMot;
}

