// ----------------------------------------------------------

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// ----------------------------------------------------------

#include "clutils.h"

// ----------------------------------------------------------

#define N (1 << 8)
#define G 64

// ----------------------------------------------------------
int lcss(const char *str0, const char *str1) {
    int sizeN = strlen(str0);
    int sizeM = strlen(str1);    
    int dynamicProg[sizeN][sizeM];
    if(sizeN == 0 || sizeM == 0){
        return 0;
    }
    for (int j = 0; j < sizeM; j++) {
        dynamicProg[0][j] = 0;
    }
    for (int i = 1; i < sizeN; i++) {
        dynamicProg[i][0] = 0;
        for (int j = 1; j < sizeM; j++) {
            if (str0[i] == str1[j]) {
                dynamicProg[i][j] = 1 + dynamicProg[i - 1][j - 1];
            } else {
                dynamicProg[i][j] = max(dynamicProg[i - 1][j], dynamicProg[i][j - 1]);
            }
        }
    }
    /*for (int i = 0; i < sizeN; i++) {
        for (int j = 0; j < sizeM; j++) {
            cerr << dynamicProg[i][j] << " | ";
        }
        cerr << endl;
    }*/
    return dynamicProg[sizeN-1][sizeM-1];
}

//Colonne_Major
int lcss2(const char *str0, const char *str1) {
    int sizeN = strlen(str0);
    int sizeM = strlen(str1);    
    int dynamicProg[sizeN*sizeM];
    if(sizeN == 0 || sizeM == 0){
        return 0;
    }
    for (int i = 0; i < sizeN; i++) {
        dynamicProg[i] = 0;
        for (int j = 0; j < sizeM; j++) {
            dynamicProg[i + j*sizeN] = 0;
        }
    }
    for (int i = 1; i < sizeN; i++) {
        for (int j = 1; j < sizeM; j++) {
            if (str0[i] == str1[j]) {
                dynamicProg[i+j*sizeN] = 1 + dynamicProg[(i - 1) + (j - 1)*sizeN];
            } else {
                dynamicProg[i+j*sizeN] = max(dynamicProg[(i - 1) + j*sizeN], dynamicProg[i + (j - 1)*sizeN]);
            }
        }
    }
   /* for (int i = 0; i < sizeN*sizeM; i++) {
            cerr << dynamicProg[i] << " | ";
    }*/
    return dynamicProg[(sizeN-1)+(sizeM-1) * sizeN];
}

int lcs(string S, string T){
    int m = S.size();
    int n = T.size();
    int LCS[m+1][n+1];
    for(int i = 0; i <= m; i++){
        LCS[i][n] = 0;
    }
    for(int j = 0; j <= n; j++){
        LCS[m][j] = 0;
    }
    for(int i = m-1; i>=0; i--){
        for(int j = n-1; j>=0;j--){
            LCS[i][j] = LCS[i+1][j+1];
            if(S[i] == T[j]){
                LCS[i][j] = LCS[i][j] + 1;
            }
            if(LCS[i][j+1] > LCS[i][j]){
                LCS[i][j] = LCS[i][j+1];
            }
            if(LCS[i+1][j] > LCS[i][j]){
                LCS[i][j] = LCS[i+1][j];
            }
        }
    } 
    /*for (int i = 0; i < m+1; i++) {
        for (int j = 0; j < n+1; j++) {
            cerr << LCS[i][j] << " | ";
        }
        cerr << endl;
    }*/
    return LCS[0][0];
}


void testLCSS(){
  // string s1 = "ABCDEFGH"; // 8
   // string s1 = "BACBAD"; // 8
  //  string s1 = "anteropygal"; // 8
    string s1 = "AAACCGTGAGTTATTCGTTCTAGAA"; // 8
    //string s2 = "ZZCZFZH"; // 7
 // string s2 = "ABAZDC"; // 7
  //string s2 = " default"; // 7
    string s2 = "CACCCCTAAGGTACCTTTGGTTC"; // 7
    const char * c1 = s1.c_str();
    const char * c2 = s2.c_str();
    int lccs = lcss(c1, c2);
    cerr << "LCCS 2D: " << lccs << endl;

    int lccs_1D = lcss2(c1, c2);
    cerr << "LCCS 1D: " << lccs_1D << endl;

    int lcs1 = lcs(s1, s2);
    cerr << "LCCS 2 : " << lcs1 << endl;
   
    string st;
    st = c1;
    cerr << "S : " << st << endl;
    cerr << "Size c1 : " << strlen(c1) << endl;
    cerr << "Size  st : " << st.size() << endl;

}
// ----------------------------------------------------------

int main(int argc,char **argv)
{
  const char *g_File = SRC_PATH "exam-dm.cl";

  char *query;
  if (argc != 2) {
    query = " default";
    cerr << "You can specify the query string on the command line" << endl;
  } else {
    query    = new char[strlen( argv[1] ) + 2];
    query[0] = ' ';
    strcpy( query+1 , argv[1] );
  }

  // init OpenCL
  cl_int err;
  cluInit();
  cl::Program *prg = cluLoadProgram(g_File);
  
  cerr << "Query: '" << query << '\'' << endl;

  // load dictionary
  vector<string> words;
  ifstream dict(SRC_PATH "test.txt");
  char str[1024];
  int  longest = 0;
  while ( dict ) {
    dict.getline( str, 1024 );
    //words.push_back(" " + string(str)); //Choix de ma part, l'espace au debut de chaque mot me perturbe car ça rajoute 1 à la longueur
    words.push_back(string(str));
    longest = max( longest, (int)words.back().length() );
  }
  cerr << "Dictionary contains " << words.size() << " entries, longest is " << longest << " chars." << endl;

  //// Ex1: TODO
  	
    int size = words.size() * longest;
    int variation = size % G;
    if (variation != 0) {
        size = size + G - variation;
    }

    // store in a table
    char *allwords = new char[ size ];
    // -> clear with zeros
    for (int i = 0; i < size; i++) {
        allwords[i] = 0;
    }
    // -> store words
    for (int w = 0; w < words.size(); w++) {
        char *start = allwords + w * longest;
        strcpy(start, words[w].c_str());
    }

    cl::Kernel *krn_histograms = cluLoadKernel(prg, "histograms");

    //// Ex1: TODO
    int wordsSize = words.size();
    int longueurSize = longest + 1; // Redimensionnement obligatoire pour garantir l'increment dans la case d'indice "longest" du tableau,
    int *bufferChar = new int[N];
    int *bufferLength = new int[longueurSize];
    
    for (int i = 0; i < N; i++) {
        bufferChar[i] = 0;
    }

    for (int i = 0; i < longueurSize; i++) {
        bufferLength[i] = 0;
    }
        
    //Buffer pour l'histogramme des caracteres
    cl::Buffer bufferHistogramsChar(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (int)*N,
            bufferChar);

    //Buffer pour l'histogramme des longueurs des mots
    cl::Buffer bufferHistogramsLength(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (int)*longueurSize,
            bufferLength);

    //Buffer d'entree qui contient tous les caracteres de tous les mots du dictionnaire. On remarquera le type int au lieu de char. 
    //En effet le codage ascii permet de facilité les calculs.
    cl::Buffer tableWords_GPU(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (char)*size,
            allwords);
            
    cl::Event eventGlobal;
    double time = 0;
    //Mise en place des Arguments
    krn_histograms->setArg(0, tableWords_GPU);
    krn_histograms->setArg(1, bufferHistogramsChar);
    krn_histograms->setArg(2, bufferHistogramsLength);
    krn_histograms->setArg(3, longest);

    err = clu_Queue->enqueueNDRangeKernel(*krn_histograms, cl::NullRange, cl::NDRange(size), cl::NDRange(G), NULL, &eventGlobal);
    cluCheckError(err, "enqueueNDRangeKernel_Histograms");
    eventGlobal.wait();
    //Permet de récuperer le temps d'execution du kernel a partir de l'event
    clu_Queue->enqueueReadBuffer(bufferHistogramsChar, false, 0, sizeof (int)*N, bufferChar);
    clu_Queue->enqueueReadBuffer(bufferHistogramsLength, false, 0, sizeof (int)*longueurSize, bufferLength);
    clu_Queue->finish();
    cerr << "********************DEBUT EXO 1***********************"<< endl;

    int s = 0;
    int non_alphabetique = 0;
    for (int i = 0; i < N; i++) {
        s += bufferChar[i];
        if(i < 65 || i > 122){
            non_alphabetique += bufferChar[i];
        }
        cerr << i << " : " << bufferChar[i] << endl;
    }
    int alphabetique = s - non_alphabetique;
    cerr << "Nombre de caracteres total =  " << s << endl;
    cerr << "Nombre de caracteres alphabetique =  " << alphabetique << endl;
    cerr << "**************************************" << endl;
    int l = 0;
    for (int i = 0; i < longueurSize; i++) {
        l += i*bufferLength[i];
        cerr << i << " : " << bufferLength[i] << endl;
    }
    int longueur_alphabetique = l - bufferChar[32] - bufferChar[13]; // 32 = code_ascii du retour chariot '\n', et 13 code_ascii de l'espace c'est pour ça qu'au debut on avait des differences (lors de la copie, ça augmente la taille des mots de +1)
    cerr << "Longueur totale " << l << endl;  
    cerr << "Longueur alphabetique = Nombre caracteres ? " << longueur_alphabetique << " = " << alphabetique << endl;  
    cerr << "\nLes différences entre la longueur totale et la longueur alphabetique viennent de la présence de : " << endl;
    cerr << "\t - \"retour chariot\" (code_ascii = 13) ou" << endl;
    cerr << "\t - \"d'espaces\" (code_ascii = 32)." << endl;
    cerr <<"Ainsi, si on soustrait leur nombre d'occurences, on doit (normalement ^^) obtenir la même valeur pour la longueur alphabetique et le nombre de caractère alphabetique !" << endl;   
    cerr << "\nTemps d'execution GPU EXO 1 : " << time << "ms" << endl;
    cerr << "********************FIN EXO 1***********************"<< endl;

  //// Ex2: TODO

  return 0;
}

// ----------------------------------------------------------

