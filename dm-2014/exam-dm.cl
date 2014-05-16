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
if(code_ascii != 0){
    if((gid % L) == L-1){
        int longueur = L-1;
        atomic_inc(bufferHistogramsLength+longueur);
    }

    else {
        if(code_ascii_suivant == 0){
            int longueur = (gid)%L; 
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
    int i = 0;
    int finMot = 0;
    while(finMot == 0){
        int ascii = (int)allwords[gid*L+i];
        if((ascii == 0) || (i == L)){
       		finMot = 1;
	}
        i = i + 1;
    }
    int sizeMot = i;
    int dynamicProg[64][64]; //n'accepte pas des variables avec 100 100 ça fait planter les pc de la fac
    for (int j = 0; j < sizeMot; j++) {
        dynamicProg[0][j] = 0;
    }
    for (int i = 1; i < Q; i++) {
        dynamicProg[i][0] = 0;
        for (int j = 1; j < sizeMot; j++) {
            if (query[i-1] == allwords[gid*L+j-1]) {
                dynamicProg[i][j] = 1 + dynamicProg[i - 1][j - 1];
            } else {
                dynamicProg[i][j] = max(dynamicProg[i - 1][j], dynamicProg[i][j - 1]);
            }
        }
    }
    int lcss_value = dynamicProg[Q-1][sizeMot-1];
    lcss[gid] = lcss_value;
}


__kernel void lcss1D( 
    __global const char *allwords, 
    __global const char *query, 
    __global       int  *lcss,
    int Q, // size of query string
    int L) // longest word
{
    int gid = get_global_id(0);
    int i = 0;
    int fin = 0;
    while(fin == 0){
        int ascii = (int)allwords[gid*L+i];
        if((ascii == 0) || (i == L)){
            fin = 1;
        }
        i++;
   }
    int sizeMot = i;
    int dynamicProg[128*128];

    for (int i = 0; i < Q; i++) {
        dynamicProg[i] = 0;
        for (int j = 0; j < sizeMot; j++) {
            dynamicProg[i + j*Q] = 0;
        }
    }
    for (int i = 1; i < Q; i++) {
        for (int j = 1; j < sizeMot; j++) {
            if (query[i-1] == allwords[gid*L+j-1]) {
                dynamicProg[i+j*Q] = 1 + dynamicProg[(i - 1) + (j - 1)*Q];
            } else {
                dynamicProg[i+j*Q] = max(dynamicProg[(i - 1) + j*Q], dynamicProg[i + (j - 1)*Q]);
            }
        }
    }
    int lcss_value = dynamicProg[(Q-1)+(sizeMot-1) * Q];
    lcss[gid] = lcss_value;
}



__kernel void list(
    __global const char *allwords, 
    __global const int  *lcss, 
    __global       int  *bufferList,
    int valueLCSS, // request value
    int L) // longest word
{
    int gid = get_global_id(0);
    if(lcss[gid] == valueLCSS){
        atomic_inc(bufferList+gid);
    } 
}


__kernel void minim(
    __global const int  *lcss, 
    __global       int  *bufferLCSS_Min)
{
    int gid = get_global_id(0);
    if(lcss[gid] < bufferLCSS_Min[0]){
        bufferLCSS_Min[0] = lcss[gid];
    } 

}
