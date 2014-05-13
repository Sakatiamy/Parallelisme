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


