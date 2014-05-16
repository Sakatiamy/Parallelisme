// ----------------------------------------------------------

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>

using namespace std;

// ----------------------------------------------------------

#include "clutils.h"

// ----------------------------------------------------------

#define N (1 << 8)
#define G 64

// ----------------------------------------------------------

/**
 * Programmation Dynamique 2D, Algorithme du PDF
 * @param str0
 * @param str1
 * @return lcss
 */
int lcss2D(const char *str0, const char *str1) {
    int sizeN = strlen(str0) + 1;
    int sizeM = strlen(str1) + 1;
    int dynamicProg[sizeN][sizeM];
    if (sizeN == 0 || sizeM == 0) {
        return 0;
    }
    for (int j = 0; j < sizeM; j++) {
        dynamicProg[0][j] = 0;
    }
    for (int i = 1; i < sizeN; i++) {
        dynamicProg[i][0] = 0;
        for (int j = 1; j < sizeM; j++) {
            if (str0[i - 1] == str1[j - 1]) {
                dynamicProg[i][j] = 1 + dynamicProg[i - 1][j - 1];
            } else {
                dynamicProg[i][j] = max(dynamicProg[i - 1][j], dynamicProg[i][j - 1]);
            }
        }
    }
    return dynamicProg[sizeN - 1][sizeM - 1];
}
/**
 * Programmation Dynamique 1D, Algorithme du PDF
 * @param str0
 * @param str1
 * @return lcss
 */
//Colonne_Major

int lcss1D(const char *str0, const char *str1) {
    int sizeN = strlen(str0) + 1;
    int sizeM = strlen(str1) + 1;
    int dynamicProg[sizeN * sizeM];
    if (sizeN == 0 || sizeM == 0) {
        return 0;
    }
    for (int i = 0; i < sizeN; i++) {
        dynamicProg[i] = 0;
        for (int j = 0; j < sizeM; j++) {
            dynamicProg[i + j * sizeN] = 0;
        }
    }
    for (int i = 1; i < sizeN; i++) {
        for (int j = 1; j < sizeM; j++) {
            if (str0[i - 1] == str1[j - 1]) {
                dynamicProg[i + j * sizeN] = 1 + dynamicProg[(i - 1) + (j - 1) * sizeN];
            } else {
                dynamicProg[i + j * sizeN] = max(dynamicProg[(i - 1) + j * sizeN], dynamicProg[i + (j - 1) * sizeN]);
            }
        }
    }
    return dynamicProg[(sizeN - 1)+(sizeM - 1) * sizeN];
}

/**
 * Programmation Dynamique 2D, Algorithme du PDF Inversée
 * @param str0
 * @param str1
 * @return lcss
 */
int lcssBest(string str0, string str1) {
    int m = str0.size();
    int n = str1.size();
    int LCS[m + 1][n + 1];
    for (int i = 0; i <= m; i++) {
        LCS[i][n] = 0;
    }
    for (int j = 0; j <= n; j++) {
        LCS[m][j] = 0;
    }
    for (int i = m - 1; i >= 0; i--) {
        for (int j = n - 1; j >= 0; j--) {
            LCS[i][j] = LCS[i + 1][j + 1];
            if (str0[i] == str1[j]) {
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
    return LCS[0][0];
}

/**
 * Permet de tester les valeurs des lcss suivant les différents algorithmes
 * @param str0
 * @param str1
 */
void testLCSS(string str0, string str1) {
    const char * c1 = str0.c_str();
    const char * c2 = str1.c_str();
    cerr << "Test lcssBest entre : " << str0 << " et " << str1 << endl;

    int lccs_2D = lcss2D(c1, c2);
    cerr << "LCCS 2D: " << lccs_2D << endl;

    int lccs_1D = lcss1D(c1, c2);
    cerr << "LCCS 1D: " << lccs_1D << endl;

    int lcss_Best = lcssBest(str0, str1);
    cerr << "LCCS Best : " << lcss_Best << endl;
}

// ----------------------------------------------------------

int main(int argc, char **argv) {
    const char *g_File = SRC_PATH "exam-dm.cl";

    char *query;
    if (argc != 2) {
        query = " default";
        cerr << "You can specify the query string on the command line" << endl;
    } else {
        query = new char[strlen(argv[1]) + 2];
        query[0] = ' ';
        strcpy(query + 1, argv[1]);
    }

    // init OpenCL
    cluInit();
    cl::Program *prg = cluLoadProgram(g_File);

    cerr << "Query: '" << query << '\'' << endl;

    // load dictionary
    vector<string> words;
    //    ifstream dict(SRC_PATH "american.txt");
    ifstream dict(SRC_PATH "test.txt");
    char str[1024];
    int longest = 0;
    while (dict) {
        dict.getline(str, 1024);
        words.push_back(" " + string(str));
        longest = max(longest, (int) words.back().length());
    }

    cerr << "Dictionary contains " << words.size() << " entries, longest is " << longest << " chars." << endl;
    // Cela permet d'avoir une valeur compatible avec les puissances de 2.

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

    //// Ex1: TODO
    cerr << "\n********************DEBUT PARTIE 1********************" << endl;
    cl_int errHistograms;
    cl::Kernel *krn_histograms = cluLoadKernel(prg, "histograms");
    cl::Event eventHistograms;

    int *bufferChar = new int[N];
    int *bufferLength = new int[longest];

    for (int i = 0; i < N; i++) {
        bufferChar[i] = 0;
    }

    for (int i = 0; i < longest; i++) {
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
            sizeof (int)*longest,
            bufferLength);


    //Buffer d'entree qui contient tous les caracteres de tous les mots du dictionnaire. On remarquera le type int au lieu de char. 
    //En effet le codage ascii permet de facilité les calculs.
    cl::Buffer tableWords_GPU(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (char)*size,
            allwords);

    double time_Histograms = 0;
    //Mise en place des Arguments
    krn_histograms->setArg(0, tableWords_GPU);
    krn_histograms->setArg(1, bufferHistogramsChar);
    krn_histograms->setArg(2, bufferHistogramsLength);
    krn_histograms->setArg(3, longest);

    errHistograms = clu_Queue->enqueueNDRangeKernel(*krn_histograms, cl::NullRange, cl::NDRange(size), cl::NDRange(G), NULL, &eventHistograms);
    cluCheckError(errHistograms, "enqueueNDRangeKernel_Histograms");
    eventHistograms.wait();
    time_Histograms = time_Histograms + cluEventMilliseconds(eventHistograms);
    //Permet de récuperer le temps d'execution du kernel a partir de l'event
    clu_Queue->enqueueReadBuffer(bufferHistogramsChar, false, 0, sizeof (int)*N, bufferChar);
    clu_Queue->enqueueReadBuffer(bufferHistogramsLength, false, 0, sizeof (int)*longest, bufferLength);
    clu_Queue->finish();

    int s = 0;
    int non_alphabetique = 0;
    for (int i = 0; i < N; i++) {
        s += bufferChar[i];
        if (i < 65 || i > 122) {
            non_alphabetique += bufferChar[i];
        }
        cerr << "Occurence caractere, " << i << " : " << bufferChar[i] << endl;
    }
    int alphabetique = s - non_alphabetique;
    cerr << "\nNombre de caracteres total =  " << s << endl;
    cerr << "Nombre de caracteres alphabetique =  " << alphabetique << endl;
    cerr << "===================================" << endl;
    int l = 0;
    for (int i = 0; i < longest; i++) {
        l += i * bufferLength[i];
        cerr << "Occurence Longueur, " << i << " : " << bufferLength[i] << endl;
    }
    int longueur_alphabetique = l - bufferChar[13];
    // 32 = code_ascii du retour chariot '\n', et 13 code_ascii de l'espace.
    cerr << "\nLongueur totale " << l << endl;
    cerr << "Longueur alphabetique = Nombre caracteres ? " << longueur_alphabetique << " = " << alphabetique << endl;
    cerr << "\nLes différences entre la longueur totale et la longueur alphabetique viennent de la présence de : " << endl;
    cerr << "\t - \"retour chariot\" (code_ascii = 13)" << endl;
    cerr << "Ainsi, si on soustrait son nombre d'occurences, on doit (normalement ^^) obtenir la même valeur pour la longueur alphabetique et le nombre de caractères alphabetique !" << endl;
    cerr << "Temps d'execution GPU Exo 1 : " << time_Histograms << " ms" << endl;
    cerr << "********************FIN PARTIE 1********************\n" << endl;

    //// Ex2: TODO
    cerr << "********************DEBUT PARTIE 2********************" << endl;
    cl_int errLCSS;
    cl::Kernel *krn_lcss = cluLoadKernel(prg, "lcss");
//    cl::Kernel *krn_lcss = cluLoadKernel(prg, "lcss1D");
    cl::Event eventLCSS;

    int querySize = strlen(query) + 1;
    int wordsSize = words.size() - 1;
    //On fait -1 pour enlever la derniere ligne du fichier
    //Cette-dernière est une ligne vide, et fausse la valeur du lcss minimum

    int *bufferLCSS_CPU = new int[wordsSize];

    for (int i = 0; i < wordsSize; i++) {
        bufferLCSS_CPU[i] = 0;
    }

    //Buffer pour le query
    cl::Buffer bufferQuery_kernel(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (char)*querySize,
            query);

    //Buffer pour correlation  Mot - lcss
    cl::Buffer bufferLCSS_Kernel(
            *clu_Context,
            CL_MEM_WRITE_ONLY,
            sizeof (int)*wordsSize,
            NULL);

    double time_LCSS = 0;
    //Mise en place des Arguments
    krn_lcss->setArg(0, tableWords_GPU);
    krn_lcss->setArg(1, bufferQuery_kernel);
    krn_lcss->setArg(2, bufferLCSS_Kernel);
    krn_lcss->setArg(3, querySize);
    krn_lcss->setArg(4, longest);

    errLCSS = clu_Queue->enqueueNDRangeKernel(*krn_lcss, cl::NullRange, cl::NDRange(size), cl::NDRange(G), NULL, &eventLCSS);
    cluCheckError(errLCSS, "enqueueNDRangeKernel_LCSS");
    eventLCSS.wait();
    //    time_LCSS = time_LCSS + cluEventMilliseconds(eventLCSS);

    //    Permet de récuperer le temps d'execution du kernel a partir de l'event
    clu_Queue->enqueueReadBuffer(bufferLCSS_Kernel, false, 0, sizeof (int)*wordsSize, bufferLCSS_CPU);
    clu_Queue->finish();

    cerr << "------------------------DEBUT QUESTION 1------------------------" << endl;
    testLCSS("BACBAD", "ABAZDC");
    cerr << "------------------------FIN QUESTION 1------------------------\n" << endl;

    cerr << "------------------------DEBUT QUESTION 2------------------------" << endl;
    int lcssMinimum = 1024;
    for (int i = 0; i < wordsSize; i++) {
        lcssMinimum = min(lcssMinimum, bufferLCSS_CPU[i]);
        cerr << "LCSS, " << i << " : " << bufferLCSS_CPU[i] << endl;
    }
    cerr << "\nLe LCSS Minimum est : " << lcssMinimum << endl;
    cerr << "===========================================================" << endl;
    cerr << "Valeur de la query : " << query << ", de taille : " << querySize << endl;
    cerr << "Nombre de Mots a tester : " << wordsSize << "\n" << endl;
    int compteurErreur = 0;
    for (int i = 0; i < wordsSize; i++) {
        const char * word = words[i].c_str();
        int lcss_Best = lcssBest(query, word);
        int lcss_2D = lcss2D(query, word);
        int lcss_1D = lcss1D(query, word);
        if (lcss_Best != bufferLCSS_CPU[i]) {
            compteurErreur++;
            cerr << "Numero de ligne et Mot qui provoque une erreur : " << i << ", " << words[i].c_str() << endl;
            cerr << "lcss 2D: " << lcss_2D << endl;
            cerr << "lcss 1D: " << lcss_1D << endl;
            cerr << "lcss Best : " << lcss_Best << endl;
            cerr << "lcss Dyna GPU : " << bufferLCSS_CPU[i] << "\n" << endl;
        }
        //        cerr << "Numero de ligne et Mot qui provoque une erreur : " << i << ", " << words[i].c_str() << endl;
        //        cerr << "lcss Best : " << lcss_Best << endl;
        //        cerr << "lcss 2D: " << lcss_2D << endl;
        //        cerr << "lcss 1D: " << lcss_1D << endl;
        //        cerr << "lcss Dyna GPU : " << bufferLCSS_CPU[i] << "\n" << endl;
    }
    double tauxErreur = (compteurErreur / wordsSize)*100;
    cerr << "Nombre d'erreurs : " << compteurErreur << "\n";
    cerr << "Taux d'erreurs : " << tauxErreur << "%\n";
    cerr << "Temps d'execution GPU Exo 2 : " << time_LCSS << " ms" << endl;
    cerr << "------------------------FIN QUESTION 2------------------------\n" << endl;

    cerr << "------------------------DEBUT QUESTION 3------------------------" << endl;
    cl_int errList;
    cl::Kernel *krn_list = cluLoadKernel(prg, "list");
    cl::Event eventList;

    int *tabString = new int[wordsSize];

    for (int i = 0; i < wordsSize; i++) {
        tabString[i] = 0;
    }

    //Buffer Bis pour correlation  Mot - lcss
    //On aurait pu reutiliser le buffer bufferLCSS_Kernel de la Question 2, mais je prefere en recreer un autre afin de délimiter chaque question
    cl::Buffer bufferLCSS_KernelBis(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (int)*wordsSize,
            bufferLCSS_CPU);

    cl::Buffer bufferList(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (int) * wordsSize,
            tabString);

    double time_List = 0;
    //Mise en place des Arguments
    krn_list->setArg(0, tableWords_GPU);
    krn_list->setArg(1, bufferLCSS_KernelBis);
    krn_list->setArg(2, bufferList);
    krn_list->setArg(3, lcssMinimum);
    krn_list->setArg(4, longest);

    errList = clu_Queue->enqueueNDRangeKernel(*krn_list, cl::NullRange, cl::NDRange(size), cl::NDRange(G), NULL, &eventList);
    cluCheckError(errList, "enqueueNDRangeKernel_List");
    eventList.wait();
    //    time_List = time_List + cluEventMilliseconds(eventList);

    //Permet de récuperer le temps d'execution du kernel a partir de l'event
    clu_Queue->enqueueReadBuffer(bufferList, false, 0, sizeof (int) * wordsSize, tabString);
    clu_Queue->finish();

    int tailleList = 0;
    for (int i = 0; i < wordsSize; i++) {
        if (tabString[i] == 1) {
            tailleList++;
            cerr << i << ", Flag = " << tabString[i] << " --->" << words[i] << endl;
        }
    }
    //Attention aux indices elles commencent a 0 et non a 1, 
    //donc faire +1 par rapport à l'indice affiché.
    cerr << "\nTaille de la liste : " << tailleList << endl;
    cerr << "Temps d'execution GPU Exo 3 : " << time_List << " ms" << endl;
    cerr << "------------------------FIN QUESTION 3------------------------" << endl;
    cerr << "********************FIN PARTIE 2********************" << endl;

    cerr << "------------------------TEST MINIMUM------------------------" << endl;
    /**
     * Le calcul du lcss minimum se fait dans le CPU.
     * Une autre possibilité aurait été de le retrouver avec du parallélisme via une réduction GPU.
     * N'ayant pas réussi à le faire, je suis resté sur la méthode CPU.
     * 
     * Concernant la liste des mots pour la question 3 de l'exercice 2,
     * j'ai decidé d'utiliser des flags (0 et 1) pour determiner
     * si le lcss du mot est égal au lcss minimum. 
     * C'est le CPU qui se charge d'afficher le mot (via une boucle dans la liste de mots 'words') si le flag = 1.
     * 
     * Encore une fois, attention au indice :D . 
     * 
     * Par ailleurs, j'ai implémenté différents algorithmes pour le calcul du lcss (tableau 1D, et 2D).
     * Cela est dû au fait que les résultats obtenus, avec l'algorithme que vous proposez, n'étaient pas corrects.
     * Ex : le test avec les string "BACBAD" et 'ABAZCD" renvoyait 3, alors que le resultat attendu est 4 !
     * (Mais il se peut que je n'ai pas correctement implémenté l'algorithme au départ).
     * Ainsi, je me suis basé sur l'algorithme de la fonction lcssBest (toujours juste) pour corriger l'algorithme proposé.
     * C'est pourquoi, on observe un -1 lors du test des 2 caractères.
     * 
     * Enfin, dans le kernel lcss, opencl n'accepte pas l'instanciation de tableau avec des tailles "variables". 
     * '-> on observe le message d'erreur suivant : "openCL does not support variable length arrays".
     * La taille du tableau dynamique dans le kernel est fixe. 
     * La valeur a été choisie de manière subjective à '64', impliquant la relation : longest < 64.
     * Suivant la configuration des PC, il est possible d'augmenter cette valeur (sans tout faire planter) 
     * agrandissant ainsi la taille du plus long mot du dictionnaire. 
     * Ceci n'est malheureusement pas optimale, mais je n'ai pas trouver le moyen de contourner le problème pour utiliser la valeur de la variable longest. 
     *  
     * P.S : Chose que je n'arrive pas à comprendre : 
     * Pourquoi un code qui provoque des bugs incompréhensibles fonctionne correctement sur les PC de la Fac ?
     * Cela vient-il des versions OpenCl installées ?
     * 
     * ex : error -9999 sur ma machine apparait lorsque je veux calculer le temps d'execution d'un kernel,
     * tandis qu'à la fac il n'y a aucun soucis !
     * Un autre bug : Ma machine (ayant pourtant une meilleure configuration) n'arrive pas à gerer un dictionnaire dont le nombre de mots est important, ie > 350 mots, 
     * alors qu'à la fac aucun soucis !
     * 
     * Je prends donc les machines de la fac pour référence !
     * 
     * Merci.
     */

    cl_int errMin;
    cl::Kernel *krn_min = cluLoadKernel(prg, "minim");
    cl::Event eventMin;

    int *lcssMin = new int[1];
    lcssMin[0] = INT16_MAX;

    //Buffer pour le lcss minimum
    cl::Buffer bufferLCSS_MIN_kernel(
            *clu_Context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof (int),
            lcssMin);

    krn_min->setArg(0, bufferLCSS_Kernel);
    krn_min->setArg(1, bufferLCSS_MIN_kernel);

    errMin = clu_Queue->enqueueNDRangeKernel(*krn_min, cl::NullRange, cl::NDRange(size), cl::NDRange(G), NULL, &eventMin);
    cluCheckError(errMin, "enqueueNDRangeKernel_Min");
    eventMin.wait();
    //    time_List = time_List + cluEventMilliseconds(eventList);

    cerr << "LCSS min : " << lcssMin[0] << endl;
    //Permet de récuperer le temps d'execution du kernel a partir de l'event
    clu_Queue->enqueueReadBuffer(bufferLCSS_MIN_kernel, false, 0, sizeof (int), lcssMin);
    clu_Queue->finish();
    cerr << "------------------------TEST MINIMUM------------------------" << endl;

    cerr << "********************DEBUT PARTIE 3********************" << endl;
    /**
     * Après etude du document lcss.pdf, et plus particulièrement les diapositives 7 et 8,
     * on s'aperçoit que le calcul du lcss d'une case necessite la connaissance de 3 autres.
     * Ainsi, pour calculer LCSS(S(3), Q(2)), on doit connaitre :
     *  -   LCSS(S(2), Q(1))
     *  -   LCSS(S(3), Q(1))
     *  -   LCSS(S(2), Q(2))
     * Or, si on observe bien, on remarque qu'elles appartiennent aux 2 diagonales précédant la case voulue.
     * C'est pourquoi, une implémentation possible consisterait à paralléliser le calcul de chacune des cases d'une diagonale. 
     * En effet, le calcul de chaque case d'une diagonale ne dépend pas du tout des autres cases (de cette diagonale).
     * Il nous faudrait alors une synchronisation globale car le calcul d'une diagonale necéssite de connaitre les 2 précédantes 
     * (conflits possibles de lecture/ecriture entre des threds de groupes différents)
     * Nous aurions donc, un pseudo algorithme du type :
     *  (Cf Diapo 8)
     *  -   Calculer LCSS(S(1), Q(1))
     *  Synchronisation Globale, ie boucle au niveau de enqueueNDRangeKernel(...).
     *  -   Calculer LCSS(S(1), Q(2)), LCSS(S(2), Q(1))
     *  Synchronisation Globale
     *  -   Calculer LCSS(S(1), Q(3)), LCSS(S(2), Q(2)), LCSS(S(3), Q(1))
     *  Synchronisation Globale
     * 
     * 
     * Plus facile à dire qu'à faire.
     */

    cerr << "********************FIN PARTIE 3********************" << endl;


    return 0;
}

// ----------------------------------------------------------

