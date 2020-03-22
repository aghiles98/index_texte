/*************************************************************************************************
 *
 *
 * Objectif : Indexer un fichier texte en utilisant les listes linéaires chainées
 *
 *
 * Auteur   : IDRES Aghiles & MAHLAL Mehenna
 *
 *
 ***************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <conio.h>
#include <conio2.h>
#include "conio.h"
#include "conio2.h"
#include "Biblio_LLC_Index.h"

/***********************************************************************************
*
*                        CONSTANTES PREPROCESSEUR
*
************************************************************************************/
#define FAILURE 1
#define SUCCESS 0

#define FICHIER_A_INDEXER "C:/tmp/fichier.txt"
#define FICHIER_DUMP      "C:/tmp/dump_index.txt"

#define MAX_INTERVALLE 26
#define NB_MOTS_KO     39
#define MOT_MIN         0
#define MOT_MAX         1
#define MAX_SZ_MOT    255

/***********************************************************************************
*
*                        VARIABLES GLOBALES
*
************************************************************************************/
char tab_mots[6][11] = {"le","le","how", "are", "tou", "tod"};


char tab_mots_ko[NB_MOTS_KO][11] = {"le", "la", "les", "des", "je", "tu", "il", "nous", "vous", "ils", "a", "et", "de", "donc", "y","un","une","sur",
                                     "que","qui","quoi", "par","pour","ne","ni","plus","ce","celui","ca","ou","au","on","si","sa","se",
                                     "votre","notre","du","en"};

str_intervalle tab_intervalles[MAX_INTERVALLE];

short nb_intervalles = 0;
int nb_mots_file     = 0;
int nb_mots_doublons = 0;
int nb_mots_ko       = 0;


/***********************************************************************************
*
*                        GESTION DES INTERVALLES
*
************************************************************************************/
void init_intervalle(int nb_car_interv)
{
    int i=0,idx=0, brk=0;
    int deb, fin;

    //! Remise a 0 intervalle
    reinit_var();

    while ( !brk )
    {
        nb_intervalles++;

        if ( i+nb_car_interv-1 >= 25 )
        {
            deb = 97+i;  // 97 valeur decimale de 'a' en ASCII
            fin = 97+25; // 'z'
            brk=1;
        }
        else
        {
            deb=97+i;
            fin = 97+i+nb_car_interv-1;
        }

        /* Initialisation des valeurs du tableau */
        tab_intervalles[idx].intervalle[0]   = deb;
        tab_intervalles[idx].intervalle[1]   = fin;
        tab_intervalles[idx].p_lg_mots_first = NULL;
        tab_intervalles[idx].p_lg_mots_last  = NULL;

        /* */
        i += nb_car_interv;
        idx++;
    }

}

void maj_intervalle(str_intervalle *p_str_intervalle, str_lg_mot *p_str_lg_mot)
{
    //
    if ( p_str_intervalle->p_lg_mots_first == p_str_lg_mot )
    {
        p_str_intervalle->p_lg_mots_first = p_str_lg_mot->p_lg_mot_next;
    }

    if ( p_str_intervalle->p_lg_mots_last == p_str_lg_mot )
    {
        p_str_intervalle->p_lg_mots_last = p_str_lg_mot->p_lg_mot_prev;
    }
}

str_intervalle * chercher_intervalle( char * mot )
{
    int i;
    int first_lettre;

    first_lettre = mot[0];

    // Gestion des majuscules 06042017
    if ( first_lettre  >= 65 && first_lettre <= 90  ) // 65=A 90=Z
    {
        mot[0] = first_lettre + 32; // Majuscule to minuscule
    }
    else if ( first_lettre == 130 || first_lettre == 136 || first_lettre == 138 ) // e accent aigu, circonflexe, grave
    {
            first_lettre = 101; // lettre e
    }

    // Parcours de la table des intervalles
    for(i=0 ; i<MAX_INTERVALLE ; i++)
    {
        // le mot appartient a l intervalle
        if (  tab_intervalles[i].intervalle[0] <= first_lettre && tab_intervalles[i].intervalle[1] >= first_lettre )
        {
            return tab_intervalles+i;
        }
    }

    return (str_intervalle *)NULL;
}

str_intervalle * chercher_intervalle_par_bornes(int interv_min, int interv_max )
{
    int i;

    for(i=0 ; i<MAX_INTERVALLE ; i++)
    {
        if (  tab_intervalles[i].intervalle[0] == interv_min && tab_intervalles[i].intervalle[1] == interv_max )
            return tab_intervalles+i;
    }

    return (str_intervalle *)NULL;
}


/***********************************************************************************
*
*                        GESTION DES LISTES CHAINEES DE LONGUEURS
*
************************************************************************************/
void changer_maillon_tete_longueur( str_lg_mot * p_str_lg_mot_first_new, str_lg_mot * p_str_lg_mot_first_old )
{

    // Maj nouveau maillon first
    p_str_lg_mot_first_new->p_lg_mot_prev = NULL;
    p_str_lg_mot_first_new->p_lg_mot_next = p_str_lg_mot_first_old;
    // Maj ancien maillon first
    // p_str_lg_mot_first_new->p_lg_mot_prev = p_str_lg_mot_first_new; // correction 06042017
    if ( p_str_lg_mot_first_old )
        p_str_lg_mot_first_old->p_lg_mot_prev = p_str_lg_mot_first_new;
}

void inserer_maillon_longueur( str_lg_mot * p_str_lg_mot, str_lg_mot * p_str_lg_mot_prev )
{

    str_lg_mot * p_str_lg_mot_next = (str_lg_mot *)NULL;

    //
    if ( p_str_lg_mot_prev )
    {
        p_str_lg_mot_next = p_str_lg_mot_prev->p_lg_mot_next;
        p_str_lg_mot_prev->p_lg_mot_next = p_str_lg_mot;
    }


    p_str_lg_mot->p_lg_mot_prev = p_str_lg_mot_prev;
    p_str_lg_mot->p_lg_mot_next = p_str_lg_mot_next;

    if ( p_str_lg_mot_next )
        p_str_lg_mot_next->p_lg_mot_prev = p_str_lg_mot;

}

/* p_str_lg_mot_prev : maillon apres lequel on souhaite creer le nveau maillon */
str_lg_mot * creer_maillon_longueur( str_lg_mot * p_str_lg_mot_prev, int lg_mot )
{
    str_lg_mot     * p_str_lg_mot  = (str_lg_mot *)NULL;

    // Allocation memoire
    p_str_lg_mot = (str_lg_mot*)malloc( sizeof(str_lg_mot) );
    if ( p_str_lg_mot == (str_lg_mot*)NULL )
    {
        printf("Erreur allocation memoire STR_LG_MOT !\n");
        exit(1);
    }
    // Init maillon de la chaine des longueur de mots
    p_str_lg_mot->longueur      = lg_mot;
    p_str_lg_mot->p_lg_mot_next = (str_lg_mot*)NULL;
    p_str_lg_mot->p_lg_mot_prev = (str_lg_mot*)NULL;
    p_str_lg_mot->p_mot_first   = (str_mot *)NULL;
    p_str_lg_mot->p_mot_last    = (str_mot *)NULL;


    // 1er maillon
    if ( !p_str_lg_mot_prev )
        return p_str_lg_mot;

    inserer_maillon_longueur(p_str_lg_mot,p_str_lg_mot_prev);

    return p_str_lg_mot;

}

void supprimer_maillon_longueur( str_lg_mot * p_str_lg_mot )
{

    if ( p_str_lg_mot->p_lg_mot_prev )
        p_str_lg_mot->p_lg_mot_prev->p_lg_mot_next = p_str_lg_mot->p_lg_mot_next;

    if ( p_str_lg_mot->p_lg_mot_next )
        p_str_lg_mot->p_lg_mot_next->p_lg_mot_prev = p_str_lg_mot->p_lg_mot_prev;

    // Liberation de la memoire
    free( p_str_lg_mot );
}

str_lg_mot* chercher_maillon_longueur(str_intervalle * p_str_intervalle, int lg_mot )
{

    if ( !p_str_intervalle )
        return (str_lg_mot *)NULL;

    str_lg_mot * p_lg_mot = (str_lg_mot*)NULL;

    p_lg_mot = p_str_intervalle->p_lg_mots_first;
    while ( p_lg_mot )
    {
        // On a trouve la structure
        if ( p_lg_mot->longueur == lg_mot )
        {
            return p_lg_mot;
        }

        p_lg_mot = p_lg_mot->p_lg_mot_next;

    }

    // Pas de maillon trouve
    return (str_lg_mot*)NULL;

}

str_lg_mot * trier_liste_longueurs( str_lg_mot * p_str_lg_mot )
{

    str_lg_mot * p_str_lg_mot_cur       = NULL;
    str_lg_mot * p_str_lg_mot_next      = NULL;
    str_lg_mot * p_str_lg_mot_new_first = NULL; // La (nouvelle) tete de la liste apres tri
    str_lg_mot * p_str_lg_mot_new       = NULL;
    str_lg_mot * p_str_lg_mot_ref       = NULL;

    // Boucle sur la chaine actuelle (les maillons a trier)
    p_str_lg_mot_cur       = p_str_lg_mot;
    while ( p_str_lg_mot_cur )
    {
        // Svg next
        p_str_lg_mot_next = p_str_lg_mot_cur->p_lg_mot_next;

        // Boucle sur la chaine triee tant qu'on n'y trouve un maillon de longueur plus petite
        // que le maillon a classer
        p_str_lg_mot_new = p_str_lg_mot_new_first;
        p_str_lg_mot_ref = NULL;
        while (p_str_lg_mot_new && p_str_lg_mot_new->longueur < p_str_lg_mot_cur->longueur )
        {
            p_str_lg_mot_ref = p_str_lg_mot_new;

            p_str_lg_mot_new = p_str_lg_mot_new->p_lg_mot_next;
        }

        // Pas de maillon plus petit dans la nouvelle chaine => Le maillon a classer est le + petit de tous,
        // il sera la nouvelle tete de la chaine triee
        if ( !p_str_lg_mot_ref )
        {
            changer_maillon_tete_longueur(p_str_lg_mot_cur,p_str_lg_mot_new_first);
            p_str_lg_mot_new_first = p_str_lg_mot_cur;
        }
        // Au moins un maillon de la chaine triee est plus petit que le maillon a classer
        // le dernier maillon le plus petit etant p_str_lg_mot_ref => On place le nouveau maillon apres p_str_lg_mot_ref
        else
            inserer_maillon_longueur(p_str_lg_mot_cur,p_str_lg_mot_ref);

        p_str_lg_mot_cur = p_str_lg_mot_next;

    }

    return p_str_lg_mot_new_first;

}

/***********************************************************************************
*
*                        GESTION DES LISTES CHAINEES DE MOTS
*
************************************************************************************/
void changer_maillon_tete_mot( str_mot * p_str_mot_first_new, str_mot * p_str_mot_first_old )
{
    // Maj nouveau maillon first
    p_str_mot_first_new->p_mot_prev = NULL;
    p_str_mot_first_new->p_mot_next = p_str_mot_first_old;
    // Maj ancien maillon first
    if ( p_str_mot_first_old )
        p_str_mot_first_old->p_mot_prev = p_str_mot_first_new;
}

void inserer_maillon_mot( str_mot * p_str_mot, str_mot * p_str_mot_prev )
{

    // 1er maillon d une chaine
    if (!p_str_mot_prev)
    {
        // On initialise au cas ou
        p_str_mot->p_mot_next = NULL;
        p_str_mot->p_mot_prev = NULL;
        return;
    }

    // Maj du maillon insere
    p_str_mot->p_mot_prev = p_str_mot_prev;
    p_str_mot->p_mot_next = p_str_mot_prev->p_mot_next;
    // Maj maillon suivant
    if ( p_str_mot_prev->p_mot_next )
        p_str_mot_prev->p_mot_next->p_mot_prev = p_str_mot;
    // Maj maillon precedent
    p_str_mot_prev->p_mot_next = p_str_mot;

}

/* p_str_mot_prev : maillon apres lequel on souhaite creer le nveau maillon */
str_mot * creer_maillon_mot(str_mot * p_str_mot_first, str_mot * p_str_mot_prev, char * mot )
{
    str_mot * p_str_mot      = (str_mot *)NULL;

    // Le mot existe-t-il deja ds l index ?
    if ( chercher_maillon_mot(p_str_mot_first, mot ))
        return NULL;

    p_str_mot = (str_mot *)malloc( sizeof(str_mot) );
    if ( !p_str_mot )
    {
        printf("Erreur allocation memoire pour STR_MOT !\n");
        exit(1);
    }

    // Alloc. memoire pour le nveau mot
    p_str_mot->mot = (char *)malloc( strlen(mot) +1 );
    if ( !p_str_mot->mot )
    {
        printf("Erreur allocation memoire pour un mot !\n");
        exit(1);
    }

    // init du maillon
    strcpy(p_str_mot->mot,mot);
    p_str_mot->p_mot_next = (str_mot *)NULL;
    p_str_mot->p_mot_prev = (str_mot *)NULL;

    // 1er maillon
    if ( !p_str_mot_prev )
        return p_str_mot;

    inserer_maillon_mot( p_str_mot, p_str_mot_prev );

    return p_str_mot;

}

void supprimer_maillon_mot( str_mot * p_str_mot )
{

    if ( p_str_mot->p_mot_prev )
        p_str_mot->p_mot_prev->p_mot_next = p_str_mot->p_mot_next;

    if ( p_str_mot->p_mot_next )
        p_str_mot->p_mot_next->p_mot_prev = p_str_mot->p_mot_prev;

    // Liberation memoire
    free( p_str_mot->mot );
    free( p_str_mot );

}

/*
    Principe : Rechercher un mot en commencant la recherche a partir d'un maillon donne
*/
str_mot * chercher_maillon_mot(str_mot * p_str_mot_start, char * mot )
{

    str_mot * p_str_mot = (str_mot *)NULL;

    p_str_mot = p_str_mot_start;
    while ( p_str_mot )
    {
        // mot trouve
        if ( !strcmp(p_str_mot->mot, mot) )
            return p_str_mot;

        p_str_mot = p_str_mot->p_mot_next;
    }

    // Mot introuvable...
    return (str_mot *)NULL;

}

str_mot * trier_liste_mots( str_mot * p_str_mot )
{
    str_mot * p_str_mot_cur       = NULL;
    str_mot * p_str_mot_next      = NULL;
    str_mot * p_str_mot_new_first = NULL;
    str_mot * p_str_mot_new       = NULL;
    str_mot * p_str_mot_ref       = NULL;

    // Boucle sur la chaine actuelle (a trier)
    p_str_mot_cur = p_str_mot;
    while ( p_str_mot_cur )
    {
        // Svg next
        p_str_mot_next = p_str_mot_cur->p_mot_next;

        // Boucle sur la chaine triee tant qu'on n'y trouve un maillon de longueur plus petite
        // que le maillon a classer
        p_str_mot_new = p_str_mot_new_first;
        p_str_mot_ref = NULL;
        while ( p_str_mot_new && strcmp(p_str_mot_new->mot, p_str_mot_cur->mot) < 0 )
        {
            p_str_mot_ref = p_str_mot_new;
            p_str_mot_new = p_str_mot_new->p_mot_next;
        }

        // Pas de maillon plus petit dans la nouvelle chaine => Le maillon a classer est le + petit de tous,
        // il sera la nouvelle tete de la chaine triee
        if ( !p_str_mot_ref )
        {
            changer_maillon_tete_mot(p_str_mot_cur,p_str_mot_new_first);
            p_str_mot_new_first = p_str_mot_cur;
        }
        // Au moins un maillon de la chaine triee est plus petit que le maillon a classer
        // le dernier maillon le plus petit etant p_str_mot_ref => On place le nouveau maillon apres p_str_mot_ref
        else
            inserer_maillon_mot(p_str_mot_cur,p_str_mot_ref);

        //
        p_str_mot_cur = p_str_mot_next;

    }

    return p_str_mot_new_first;

}


/***********************************************************************************
*
*                        GESTION DE L'INDEX
*
************************************************************************************/
int indexer_un_mot( char * mot )
{

    str_intervalle * p_str_intervalle = (str_intervalle *)NULL;
    str_lg_mot     * p_str_lg_mot     =(str_lg_mot *)NULL;

    // Ds quel intervalle se trouve le mot a classer
    p_str_intervalle = chercher_intervalle( mot );
    if ( !p_str_intervalle )
    {
        return SUCCESS; //
    }
    // Recherche du maillon longueur correspondant
    p_str_lg_mot = chercher_maillon_longueur(p_str_intervalle, strlen(mot));
    // La longueur du mot n est pas encore geree pour l intervalle considere
    if( !p_str_lg_mot )
    {
        // Ajout d1 nveau maillon a la chaine des longueurs
        p_str_lg_mot = creer_maillon_longueur(p_str_intervalle->p_lg_mots_last, strlen(mot) );
        // Maj des references ds str_intervalle
        if ( p_str_intervalle->p_lg_mots_first == (str_lg_mot*)NULL )
        {
            p_str_intervalle->p_lg_mots_first = p_str_lg_mot;
            p_str_intervalle->p_lg_mots_last  = p_str_lg_mot;
        }
        else
        {
            p_str_intervalle->p_lg_mots_last = p_str_lg_mot;
        }
    }

    // Ajout d1 nveau maillon ds la chaine des mots
    str_mot * p_str_mot = (str_mot *)NULL;
    if ( (p_str_mot = creer_maillon_mot(p_str_lg_mot->p_mot_first, p_str_lg_mot->p_mot_last, mot)) == NULL )
    {
        // Mot en doublon => pas d insertion
        nb_mots_doublons++;
        return SUCCESS;
    }
    // Maj des références ds str_longueur
    if ( p_str_lg_mot->p_mot_first == (str_mot *)NULL )
    {
        p_str_lg_mot->p_mot_first   = p_str_mot;
        p_str_lg_mot->p_mot_last    = p_str_mot;
    }
    else
    {
        p_str_lg_mot->p_mot_last = p_str_mot;
    }

    return SUCCESS;

}

int creer_index( int nb_car_interv )
{

    if ( nb_car_interv <= 0 || nb_car_interv > 26 )
    {
        printf("\n\n\n       Nbre de caracteres par intervalle errone - Valeurs [1;26] !\n\n\n");
        return FAILURE;
    }

    /* */
    init_intervalle(nb_car_interv);


    FILE* fichier = NULL;
    int car = 0, i=0;
    char buf[MAX_SZ_MOT];

    fichier = fopen(FICHIER_A_INDEXER, "r");

    if (fichier != NULL)
    {
        do
        {
            car = fgetc(fichier);
            if ( car == '\'' || (car >= 0 && car <= 47) || (car>=59 && car <= 64) || (car>=123 && car <= 127) )
            {
                buf[i] = '\0';
                if ( strlen(buf) > 1 )
                {
                    nb_mots_file++;
                    indexer_un_mot(buf);
                    //printf("%s %c %d\n",buf,buf[0],buf[0]);
                }

                memset(buf,0,MAX_SZ_MOT);
                i = 0;
            }
            else
            {
                buf[i] = car;
                i++;
            }

        } while (car != EOF);

        fclose(fichier);
    }
    else
    {
        printf("Erreur ouverture fichier !\n");
        printf("Assurez que le fichier a indexer (.txt) est dans c:\\tmp\n");
        return FAILURE;
    }

    return SUCCESS;

}

int supprimer_un_mot_dans_index( char * mot )
{

    str_intervalle * p_str_intervalle = (str_intervalle *)NULL;
    str_lg_mot     * p_str_lg_mot = (str_lg_mot *)NULL;
    str_mot        * p_str_mot = (str_mot *)NULL;
    str_mot        * p_str_mot_next = (str_mot *)NULL;

    p_str_intervalle = chercher_intervalle( mot );
    if ( !p_str_intervalle)
    {
        return SUCCESS;
    }

    p_str_lg_mot = chercher_maillon_longueur(p_str_intervalle, strlen(mot));
    if ( !p_str_lg_mot )
    {
        return SUCCESS;
    }

    p_str_mot      = p_str_lg_mot->p_mot_first;
    // Tant qu on trouve le mot a supprimer ds la chaine des mots
    while ( (p_str_mot = chercher_maillon_mot(p_str_mot,mot)) )
    {
        // Svgarde du prochain maillon
        p_str_mot_next = p_str_mot->p_mot_next;

        // C est le premier maillon de la chaine des mots
        if ( p_str_mot->p_mot_prev == NULL )
        {
            // Maj de la chaine des longueurs
            p_str_lg_mot->p_mot_first = p_str_mot_next;

            // Si c est l unique mot de cette chaine de mots
            if ( p_str_lg_mot->p_mot_first == NULL )
            {
                maj_intervalle(p_str_intervalle, p_str_lg_mot);
                supprimer_maillon_longueur( p_str_lg_mot );
            }
        }

        supprimer_maillon_mot(p_str_mot);
        nb_mots_ko++;

        // Maillon suivant
        p_str_mot = p_str_mot_next;

    }

    return SUCCESS;
}

void afficher_index()
{

    int i;

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    for (i=0 ; i<nb_intervalles ; i++)
    {
        printf("\n*******************\n");
        printf("*   (%c,%c) \n",tab_intervalles[i].intervalle[0],tab_intervalles[i].intervalle[1]);
        printf("*******************\n");

        str_lg_mot * p_str_lg_mot = tab_intervalles[i].p_lg_mots_first;
        while( p_str_lg_mot )
        {
            printf("Lgueur %2d : ",p_str_lg_mot->longueur);

            str_mot * p_mot = p_str_lg_mot->p_mot_first;
            while ( p_mot )
            {
                printf("%s ",p_mot->mot);

                p_mot = p_mot->p_mot_next;

            }
            printf("\n");

            //! move to next
            p_str_lg_mot = p_str_lg_mot->p_lg_mot_next;

        }
    }
}

void trier_index()
{
    int i;
    str_lg_mot * p_str_lg_mot = NULL;
    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    for (i=0 ; i<nb_intervalles ; i++)
    {

        tab_intervalles[i].p_lg_mots_first = trier_liste_longueurs(tab_intervalles[i].p_lg_mots_first);
        p_str_lg_mot = tab_intervalles[i].p_lg_mots_first;
        while ( p_str_lg_mot)
        {
            p_str_lg_mot->p_mot_first = trier_liste_mots(p_str_lg_mot->p_mot_first);
            p_str_lg_mot = p_str_lg_mot->p_lg_mot_next;
        }

    }

    printf("\n\n\n\n                Tri de l'index termine sur Ok\n\n\n\n");
}

void sauvegarder_index()
{
    int i;
    FILE* fichier = NULL;

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    //
    fichier = fopen(FICHIER_DUMP, "w");

    //
    for (i=0 ; i<nb_intervalles ; i++)
    {
        str_lg_mot * p_str_lg_mot = NULL;
        p_str_lg_mot = tab_intervalles[i].p_lg_mots_first;
        while( p_str_lg_mot)
        {

            fprintf(fichier,"%c;%c;%d;",tab_intervalles[i].intervalle[0],tab_intervalles[i].intervalle[1],p_str_lg_mot->longueur);

            str_mot * p_str_mot = NULL;
            p_str_mot = p_str_lg_mot->p_mot_first;
            while ( p_str_mot )
            {
                fprintf(fichier,"%s;",p_str_mot->mot);
                p_str_mot = p_str_mot->p_mot_next;
            }

            p_str_lg_mot = p_str_lg_mot->p_lg_mot_next;

            fprintf(fichier,"\n");
        }
    }

    fclose(fichier);

    printf("\n\n\n\n             Fichier de sauvegarde genere : %s\n\n\n\n",FICHIER_DUMP);
}

int creer_index_avec_sauvegarde()
{

    FILE* fichier = NULL;
    int car = 0, i=0;
    char buf[MAX_SZ_MOT];

    int   num_ligne  = 0;
    int   num_info = 0;
    char  interv_min;
    char  interv_max;
    int   longueur;
    char  mot[MAX_SZ_MOT];
    int   nb_car_intervalle =0;

    fichier = fopen(FICHIER_DUMP, "r");

    if (fichier != NULL)
    {
        do
        {
            car = fgetc(fichier);
            if ( car == '\n' )
            {
                num_ligne++;
                num_info = 0;
            }
            else if ( car == ';' )
            {
                num_info++;

                if ( num_info == 1 )
                    interv_min = buf[0];
                else if (num_info == 2)
                {
                    interv_max = buf[0];
                    if ( !num_ligne )
                    {
                        nb_car_intervalle = interv_max-interv_min+1;
                        init_intervalle( nb_car_intervalle );
                    }
                }
                else if (num_info == 3)
                {
                    buf[i] = '\0';
                    longueur = atoi(buf);
                }
                else
                {
                    nb_mots_file++;
                    buf[i] = '\0';
                    strcpy(mot,buf);
                    indexer_un_mot(mot);
                }

                memset(buf,0,MAX_SZ_MOT);
                i = 0;
            }
            else
            {
                buf[i] = car;
                i++;
            }

        } while (car != EOF);

        fclose(fichier);
        return SUCCESS;
    }
    else
    {
        printf("\n\n\n\n          Erreur ouverture fichier %s !\n",FICHIER_DUMP);
        return FAILURE;
    }

}


void liberer_memoire_index()
{
    int i;
    str_lg_mot *p_lg=NULL, *p_lg_next=NULL;
    str_mot    *p_mot=NULL, *p_mot_next=NULL;

    for (i=0 ; i < nb_intervalles ; i++ )
    {
        p_lg = tab_intervalles[i].p_lg_mots_first;
        while ( p_lg )
        {
            // svg next
            p_lg_next = p_lg->p_lg_mot_next;

            p_mot = p_lg->p_mot_first;
            while ( p_mot )
            {
                // svg next
                p_mot_next = p_mot->p_mot_next;

                free( p_mot->mot );
                free(p_mot);

                //
                p_mot = p_mot_next;
            }

            //
            free(p_lg);

            //
            p_lg = p_lg_next;
        }

        tab_intervalles[i].p_lg_mots_first = (str_lg_mot *)NULL;
        tab_intervalles[i].p_lg_mots_last = (str_lg_mot *)NULL;
    }
}

void reinit_var()
{
    liberer_memoire_index();

    nb_intervalles   = 0;
    nb_mots_file     = 0;
    nb_mots_doublons = 0;
    nb_mots_ko       = 0;
}

/***********************************************************************************
*
*                             Fns UTILISATEURS
*
************************************************************************************/
void supprimer_les_mots_ko()
{
    int i;

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    for (i=0 ; i<NB_MOTS_KO ; i++)
    {
        supprimer_un_mot_dans_index( tab_mots_ko[i] );
    }
}


int chercher_mot_dans_intervalle( int lg_mot, int interv_min, int interv_max, int nb_mots )
{
    str_intervalle * p_str_intervalle = NULL;
    str_lg_mot     * p_str_lg_mot = NULL;
    str_mot        * p_str_mot = NULL;
    int            cpt = 0;

    // Recherche de l'intervalle
    p_str_intervalle = chercher_intervalle_par_bornes(interv_min, interv_max);
    if (p_str_intervalle == NULL)
    {
        printf("\n\n\n              Intervalle (%c,%c) inexistant !\n\n\n",interv_min,interv_max);
        return 0;
    }

    // Recherche du maillon longueur dans l'intervalle
    p_str_lg_mot = chercher_maillon_longueur(p_str_intervalle,lg_mot);
    if ( !p_str_lg_mot )
    {
        printf("\n\n\n              Il n'existe pas de mot de lg=%d dans l'intervalle (%c,%c)\n\n\n",lg_mot,interv_min,interv_max);
        return 0;
    }

    // Recherche de nb_mots
    printf("\n\n\n\n             Mot trouve : ");
    p_str_mot = p_str_lg_mot->p_mot_first;
    while ( p_str_mot && cpt < nb_mots)
    {
        printf("[%s] ",p_str_mot->mot);
        p_str_mot = p_str_mot->p_mot_next;
        cpt++;
    }

    printf("\n\n\n");

    return cpt;

}

int chercher_mot_dans_index( int lg_mot, int nb_mots )
{
    int i, cpt=0;

    for (i=0 ; i<nb_intervalles ; i++)
    {
        cpt += chercher_mot_dans_intervalle(lg_mot, tab_intervalles[i].intervalle[0], tab_intervalles[i].intervalle[1], nb_mots-cpt);
        if ( cpt == nb_mots )
            break;
    }

    return cpt;

}

int chercher_mot_min_max( int type_recherche )
{
    int i;
    char buf[255];

    if ( type_recherche == MOT_MIN)
    {
        printf("\n");
        printf("                ====================================================\n");
        printf("                        Recherche du mot le plus petit\n");
        printf("                ====================================================\n");
    }
    else
    {
        printf("\n");
        printf("                ====================================================\n");
        printf("                        Recherche du mot le plus grand\n");
        printf("                ====================================================\n");
    }

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return SUCCESS;
    }

    buf[0] = '\0';

    for (i=0 ; i<nb_intervalles ; i++)
    {
        str_lg_mot * p_str_lg_mot = NULL;
        p_str_lg_mot = tab_intervalles[i].p_lg_mots_first;

        while( p_str_lg_mot)
        {
            str_mot * p_str_mot = NULL;
            p_str_mot = p_str_lg_mot->p_mot_first;
            while ( p_str_mot )
            {
                if ( buf[0] == '\0'
                    || ( type_recherche == MOT_MIN && strlen(p_str_mot->mot) < strlen(buf) )
                    || ( type_recherche == MOT_MAX && strlen(p_str_mot->mot) > strlen(buf) )
                )
                {
                    strcpy(buf,p_str_mot->mot);
                }
                p_str_mot = p_str_mot->p_mot_next;
            }

            p_str_lg_mot = p_str_lg_mot->p_lg_mot_next;
        }
    }

    printf("\n\n\n\n               Mot trouve [%s] de longueur [%d]\n\n\n\n",buf,strlen(buf));

    return SUCCESS;

}

/***********************************************************************************
*
*                            GESTION MENU
*
************************************************************************************/
static int menuprincipal(void)
{
   char menu[11][50] =
      {
         "1) Indexer fichier (il doit etre dans c:\\tmp)",
         "2) Afficher l'index",
         "3) Supprimer les mots inutiles",
         "4) Trier l'index",
         "5) Chercher un mot dans un intervalle",
         "6) Chercher un mot dans l'index",
         "7) Chercher le mot le plus petit",
         "8) Chercher le mot le plus grand",
         "9) Sauvegarder l'index",
         "10) Restaurer l'index (a partir de la sauvegarde)",
         "11) Quitter le programme",
      };
   int level = 1, i;
   int key,x ;
   system("color 0f");
   printf("***********************************-{ MENU }-**********************************" );
   do
   {
      for (i = 1; i <= 11; i++)
      {
         if (level == i)
         {
            textbackground(WHITE);
            textcolor(BLACK);
         }
         else
         {
            textbackground(BLACK);
            textcolor(WHITE);
         }
         gotoxy(3, 2*i + 1);
         printf("   %s", menu[i - 1]);
         x=wherex();
         printf("\n");
         gotoxy(x,2*i+1);
      }
      key = getch();
      if (key == 0 ||key == 224)
      {
        key = getch();
        switch (key)
        {
            case KEY_DOWN:
               if (level < 11) level++;
               else level = 1;
               break;
            case KEY_UP:
               if (level > 1) level--;
               else level = 11;
               break;

         }
      }
   }
   while (key != KEY_OK);
   return level;
}
int a,j,k,h,xx;
char s[1000];
int afficher_menu()
{
    int  choix;
    clrscr();
    a=menuprincipal();
    choix=a;
    textcolor(WHITE);
    switch (choix)
    {

        case 1 :
            system("cls");
            if ( creer_index_ask() == SUCCESS )
                afficher_resultat_creer_index();
            break;
        case 2 :
            system("cls");
            afficher_index();
            break;
        case 3 :
            system("cls");
            supprimer_les_mots_ko();
            afficher_resultat_supprimer_mots();
            break;
        case 4 :
            system("cls");
            trier_index();
            break;
        case 5 :
            system("cls");
            chercher_mot_dans_intervalle_ask();
            break;
        case 6 :
            system("cls");
            chercher_mot_dans_index_ask();
            break;
        case 7:
            system("cls");
            chercher_mot_min_max(MOT_MIN);
            break;
        case 8 :
            system("cls");
            chercher_mot_min_max(MOT_MAX);
            break;
        case 9 :
            system("cls");
            sauvegarder_index();
            break;
        case 10 :
            system("cls");
            if ( creer_index_avec_sauvegarde() == SUCCESS )
                afficher_resultat_creer_index();
            break;
        default :
            system("cls");
            liberer_memoire_index(); // On libere la memoire en quittant le programme
            return 0;
    }


    system("pause");

    return 1;

}

void afficher_resultat_creer_index()
{
    system("cls");
    printf("\n\n");
    printf("          ==========================================\n");
    printf("                  Bilan creation de l'index \n");
    printf("          ==========================================\n");
    printf("\n\n");
    printf("       - Nombre total d'intervalles             : %d\n",nb_intervalles);
    printf("       - Nombre de mots traites ds le fichier   : %d\n",nb_mots_file);
    printf("       - Nombre de mots indexes (sans doublons) : %d\n",nb_mots_file-nb_mots_doublons);
    printf("\n\n");
}

void afficher_resultat_supprimer_mots()
{
    system("cls");
    printf("\n\n");
    printf("          ==========================================\n");
    printf("                  Bilan suppression mots \n");
    printf("          ==========================================\n");
    printf("\n\n");
    printf("       - Nombre de mots a supprimer             : %d\n",NB_MOTS_KO);
    printf("       - Nombre de mots supprimes               : %d\n",nb_mots_ko);
    printf("       - Nombre de mots indexes                 : %d\n",nb_mots_file-nb_mots_doublons-nb_mots_ko);
    printf("\n\n");
}


void chercher_mot_dans_intervalle_ask()
{
    char borne_inf,borne_sup;;
    int  longueur;

    printf("\n");
    printf("                ====================================================\n");
    printf("                        Recherche d'un mot dans un intervalle\n");
    printf("                ====================================================\n");

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    printf("\n\n\n             - Borne inferieure de l'intervalle : ");
    scanf("%c",&borne_inf);
    getchar();
    printf("\n             - Borne superieure de l'intervalle : ");
    scanf("%c",&borne_sup);
    getchar();
    printf("\n             - Longueur du mot                  : ");
    scanf("%d",&longueur);
    getchar();

    chercher_mot_dans_intervalle(longueur,borne_inf,borne_sup,1);

}

void chercher_mot_dans_index_ask()
{
    int  longueur;

    printf("\n");
    printf("                ====================================================\n");
    printf("                        Recherche d'un mot dans l'index\n");
    printf("                ====================================================\n");

    if ( !nb_intervalles )
    {
        erreur_init_index();
        return;
    }

    printf("\n\n\n             - Longueur du mot : ");
    scanf("%d",&longueur);
    getchar();

    chercher_mot_dans_index(longueur,1);

}

int creer_index_ask()
{

    int  longueur;

    printf("\n");
    printf("                ====================================================\n");
    printf("                        Creation de l'index\n");
    printf("                ====================================================\n");

    printf("\n\n\n             - Longueur des intervalles : ");
    scanf("%d",&longueur);
    getchar();
    return creer_index(longueur);

}


void erreur_init_index()
{
    printf("\n\n\n\n\n\n            [WARNING] : L'index doit etre cree au prealable !\n\n\n\n");

}

/***********************************************************************************
*
*                        PROGRAMME PRINCIPAL - MAIN()
*
************************************************************************************/
int main()
{
    system("cls");

    // Ecrire sur la premiére page esi

    system("color f0");
    printf("\n\n\n\n\n\t  \333\333\333\333\333\333\333\333\333\333\333\n"
    "\t \333\333\333\333\333\333\333\333\333\333\333\333\333\n"
    "\t\333\333\333\333         \333\333\n"
    "\t\333\333\333\333         \333\333\n"
    "\t\333\333\333\333\333\333\333\333\333\333\333\333\333\333\333\n"
    "\t\333\333\333\333\333\333\333\333\333\333\333\333\333\333\n"
    "\t\333\333\333\333\n"
    "\t \333\333\333\333\333\333\333\333\333\333\333\333\333\n"
    "\t  \333\333\333\333\333\333\333\333\333\333\333\n");
    gotoxy(33,6);
    printf("  \333\333\333\333\333\333\333\333\333\333\333\333\333");
    gotoxy(33,7);
    printf(" \333\333\333\333\333\333\333\333\333\333\333\333\333\333\333");
    gotoxy(33,8);
    printf("\333\333\333\333");
    gotoxy(33,9);
    printf("\333\333\333\333");
    gotoxy(33,10);
    printf("\333\333\333\333\333\333\333\333\333\333\333\333\333\333\333");
    gotoxy(33,11);
    printf(" \333\333\333\333\333\333\333\333\333\333\333\333\333\333\333");
    gotoxy(33,12);
    printf("            \333\333\333\333");
    gotoxy(33,13);
    printf("\333\333\333\333\333\333\333\333\333\333\333\333\333\333\333");
    gotoxy(33,14);
    printf(" \333\333\333\333\333\333\333\333\333\333\333\333\333");
    textcolor(9);
    textbackground(WHITE);
    gotoxy(60,6);
    printf("\333\333\333\333\333\333");
    gotoxy(60,7);
    printf("\333\333\333\333\333\333");
    textbackground(WHITE);
    textcolor(BLACK);
    gotoxy(60,9);
    printf("\333\333\333\333\333\333");
    gotoxy(60,10);
    printf("\333\333\333\333\333\333");
    gotoxy(60,11);
    printf("\333\333\333\333\333\333");
    gotoxy(60,12);
    printf("\333\333\333\333\333\333");
    gotoxy(60,13);
    printf("\333\333\333\333\333\333");
    gotoxy(60,14);
    printf("\333\333\333\333\333\333\n");
    gotoxy(1,18);
    printf("-----------------< Ecole nationale superieure d'informatique >------------------");
    gotoxy(21,22);
    system("pause");
    clrscr();
    system("color f0");

    // Faire un cadre avec des caractéres spéciaux

    gotoxy(10,1);
    wprintf(L"%lc",201);
    for (j=0;j<60;j++)
    {
        wprintf(L"%lc",205);
    }
    h=wherex();
    wprintf(L"%lc",187);
    printf("\n");
    for (j=0;j<10;j++)
    {
        gotoxy(10,j+2);
        wprintf(L"%lc",186);
    }
    a=wherey();
    for (j=0;j<10;j++)
    {
        gotoxy(h,2+j);
        wprintf(L"%lc",186);
    }
    gotoxy(h,j+2);
    wprintf(L"%lc",188);
    gotoxy(11,j+2);
    for (j=0;j<60;j++)
    {
        wprintf(L"%lc",205);
    }
    gotoxy(10,12);
    wprintf(L"%lc",200);

    // Ecrire le titre cilgnotant dans le cadre

    j=0;
    do
    {
        textbackground(WHITE);
        textcolor(j);
        delay(100);
        gotoxy(25,5);
        wprintf(L"TP-1-: Listes lin%lcaires chain%lces\n",130,130);
        gotoxy(28,7);
        printf("<<Gestion index texte>>");
        if (j!=14) j++;
        else j=0;
    }
    while(!kbhit());

    // Cesser de clignoter en appuyant sur une touche qlq
    // Ecrire les infos liés au tp

    system("color f0");
    gotoxy(25,5);
    wprintf(L"TP-1-: Listes lin%lcaires chain%lces\n",130,130);
    gotoxy(28,7);
    printf("<<Gestion index texte>>");
    gotoxy(19,15);
    delay(500);
    wprintf(L"Realis%lc par: IDRES Aghiles & MAHLAL Mehenna",130);
    gotoxy(30,17);
    delay(500);
    wprintf(L"Encadr%lc par: Mme KACET",130);
    gotoxy(17,20);
    delay(300);

    // Barre de chargement

    printf("Chargement: ");
    for (k=0;k<10;k++)
    {
        delay(100);
        printf("\333\333\333");
        xx=wherex();
        gotoxy(61,20);
        printf("%d %%",(k+1)*10);
        gotoxy(xx,20);
    }
    gotoxy(21,23);
    system("pause");
    while (1)
    {
        if ( !afficher_menu() )
        {
            clrscr();

            // Dessin a la dernière page avec contacts

            sprintf(s,"\n           (_\\     /_) ""\n"
            "             ))   (( ""\n"
            "          .-'''''''''-.  ""\n"
            "      /^\\/   _.   _.   \\/^\\ ""\n"
            "      \\(    /__\\ /__\\    )/ ""\n"
            "        \\,  \\o_/_\\o_/  ,/ ""\n"
            "          \\    (_)    / ""\n"
            "           `-.'==='.-' ""\n"
            "            __) - (__ ""\n"
            "           /  `~~~`  \\ ""\n"
            "          /  /     \\  \\ ""\n"
            "          \\ :       ; / ""\n"
            "           \\|==(*)==|/ ""\n"
            "            :       : ""\n"
            "             \\  |  / ""\n"
            "           ___)=|=(___ ""\n"
            "          {____/ \\____} \n\n\n\n        ");
            system("color 0f");
            textbackground(BLACK);
            textcolor(WHITE);
            printf("%s",s);
            gotoxy(30,5);
            printf("Pour plus d'informations, contactez:");
            gotoxy(30,8);
            printf("IDRES Aghiles: ga_idres@esi.dz 0658092840");
            gotoxy(30,10);
            printf("MAHLAL Mehenna: gm_mahlal@esi.dz 0779204569");
            gotoxy(20,20);
            system("pause");
            break;
        }
    }

    return 0;
}
