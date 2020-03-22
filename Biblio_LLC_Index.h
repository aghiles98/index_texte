/***********************************************************************************
*
*                    STRUCTURES DES LISTES CHAINEES
*
************************************************************************************/
typedef struct _str_mot
{
    char * mot;
    struct _str_mot *p_mot_next;
    struct _str_mot *p_mot_prev;
} str_mot;

typedef struct _str_lg_mot
{
    short       longueur;
    struct _str_lg_mot *p_lg_mot_next;
    struct _str_lg_mot *p_lg_mot_prev;
    str_mot     * p_mot_first;
    str_mot     * p_mot_last;
} str_lg_mot;

typedef struct _str_intervalle
{
    int intervalle[2];
    struct _str_lg_mot *p_lg_mots_first;
    struct _str_lg_mot *p_lg_mots_last;

} str_intervalle;

/***********************************************************************************
*
*                        GESTION DES INTERVALLES
*
************************************************************************************/
void init_intervalle(int nb_car_interv);
void maj_intervalle(str_intervalle *p_str_intervalle, str_lg_mot *p_str_lg_mot);
str_intervalle * chercher_intervalle( char * mot );
str_intervalle * chercher_intervalle_par_bornes(int interv_min, int interv_max );

/***********************************************************************************
 *
 *                   Fn DE GESTION DES LISTES CHAINEES DE LONGUEURS
 *
 ************************************************************************************/
 void changer_maillon_tete_longueur( str_lg_mot * p_str_lg_mot_first_new, str_lg_mot * p_str_lg_mot_first_old );
 void inserer_maillon_longueur( str_lg_mot * p_str_lg_mot, str_lg_mot * p_str_lg_mot_prev );
str_lg_mot * creer_maillon_longueur( str_lg_mot * p_str_lg_mot_prev, int lg_mot );
void supprimer_maillon_longueur( str_lg_mot * p_str_lg_mot );
str_lg_mot* chercher_maillon_longueur(str_intervalle * p_str_intervalle, int lg_mot );
str_lg_mot * trier_liste_longueurs( str_lg_mot * p_str_lg_mot );

/***********************************************************************************
 *
 *                      Fn DE GESTION DES LISTES CHAINEES DE MOTS
 *
 ************************************************************************************/
void changer_maillon_tete_mot( str_mot * p_str_mot_first_new, str_mot * p_str_mot_first_old );
void inserer_maillon_mot( str_mot * p_str_mot, str_mot * p_str_mot_prev );
str_mot * creer_maillon_mot(str_mot * p_str_mot_first, str_mot * p_str_mot_prev, char * mot );
void supprimer_maillon_mot( str_mot * p_str_mot );
str_mot * chercher_maillon_mot(str_mot * p_str_mot_start, char * mot );
str_mot * trier_liste_mots( str_mot * p_str_mot );

/***********************************************************************************
 *
 *                        Fn DE GESTION DE L'INDEX
 *
 ************************************************************************************/
int  creer_index(int nb_car_interv);
void afficher_index();
void sauvegarder_index();
int creer_index_avec_sauvegarde();
int  indexer_un_mot( char * mot );
int  supprimer_un_mot_dans_index( char * mot );
void liberer_memoire_index();
void reinit_var();

/***********************************************************************************

                             Fns UTILISATEURS

   **********************************************************************************/
void supprimer_les_mots_ko();
int chercher_mot_dans_intervalle( int lg_mot, int interv_min, int interv_max, int nb_mots );
int chercher_mot_dans_index( int lg_mot, int nb_mots );
int chercher_mot_min_max( int type_recherche );

/***********************************************************************************
 *
 *                        MENU / SAISIE
 *
 ************************************************************************************/
static int menuprincipal(void);
int afficher_menu();
void afficher_resultat_creer_index();
void afficher_resultat_supprimer_mots();
void chercher_mot_dans_intervalle_ask();
void chercher_mot_dans_index_ask();
int creer_index_ask();

void erreur_init_index();

