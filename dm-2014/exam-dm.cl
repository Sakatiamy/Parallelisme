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
/*
__kernel void lcss( 
    __global const char *allwords, 
    __global const char *query, 
    __global       int  *lengthWord, 
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
    lengthWord[gid] = sizeMot;

    int dynamicProg[100][100]; //n'accepte pas des variables
    for (int j = 0; j < sizeMot; j++) {
        dynamicProg[0][j] = 0;
    }
    for (int i = 1; i < Q; i++) {
        dynamicProg[i][0] = 0;
        for (int j = 1; j < sizeMot; j++) {
            if (query[i] == allwords[gid*L+j]) {
                dynamicProg[i][j] = 1 + dynamicProg[i - 1][j - 1];
            } else {
                dynamicProg[i][j] = max(dynamicProg[i - 1][j], dynamicProg[i][j - 1]);
            }
        }
    }
    int lcss_value = dynamicProg[Q-1][sizeMot-1];
    int position = gid%L;
    int index = 0;
    if(position != 0){
        int temp = L - position;
        index = gid + temp / L; 
    }
    else{
        index = gid / L;
    }
    lcss[gid] = lcss_value;
}*/


__kernel void lcss( 
    __global const char *allwords, 
    __global const char *query, 
    __global       int  *lengthWord, 
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
    lengthWord[gid] = sizeMot;


    int dynamicProg[100][100]; //n'accepte pas des variables
    int m = Q;
    int n = sizeMot;
    int LCS[200][200];
    for (int i = 0; i <= m; i++) {
        LCS[i][n] = 0;
    }
    for (int j = 0; j <= n; j++) {
        LCS[m][j] = 0;
    }
    for (int i = m - 1; i >= 0; i--) {
        for (int j = n - 1; j >= 0; j--) {
            LCS[i][j] = LCS[i + 1][j + 1];
            if (query[i] == allwords[gid*L+j]) {
                LCS[i][j] = LCS[i][j] + 1;
            }
            if (LCS[i][j + 1] > LCS[i][j]) {
                LCS[i][j] = LCS[i][j + 1];
            }
            if (LCS[i + 1][j] > LCS[i][j]) {
                LCS[i][j] = LCS[i + 1][j];
            }
        }
    }
    lcss[gid] = LCS[0][0];
}


/*
__kernel void lcss( 
    __global const char *allwords, 
    __global const char *query, 
    __global       int  *lengthWord, 
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
    lengthWord[gid] = sizeMot;
}
*/