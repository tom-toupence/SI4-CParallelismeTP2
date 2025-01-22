#include <omp.h>
#include <unistd.h> 
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


void fillArray(long *table, int n, int maxexclu){
  int i ; 
  for (i=0;i<n;i++) { 
    //table[i]=rand()%maxexclu;
    *table=rand()%maxexclu;// ou autre valeur;
    table++; //pour aller à element suivant
  }  
}

void square(long *table, int n) {
  int i;
  long total = 0; 
  int nthreads = omp_get_num_threads ();
  fprintf(stdout, "Square avec Number of threads %i \n", nthreads);
  //omp_set_num_threads (10); à 10 threads par ex, ou setter la valeur via la var d'environnement OMP_NUM_THREADS, ou avoir le nb threads dispos par défaut
  
  int tid; 
#pragma omp parallel //private(total) // si total est private, chaque thread en a sa copie, donc fait en fait juste son sous total mais la var globale total reste inchangée!
  //shared(table,n) private (i) , c'est ainsi par defaut donc, pas forcément besoin de le préciser 
  { //total=0; // si private, il faut penser alors à initialiser total à qq chose , sinon, val initiale est hasardeuse/inconnue/dépend de la mémoire à l'adresse où est total
    //Si on déclare private(total) dans ligne pragma for, on n'a pas la possibilité d'initialiser total de chaque thread à 0. 
    // Si total est shared ne pas laisser =0 ici, car l'initialisation à 0 sera faite en parallele de l'instruction for puisque on est dans un bloc parallel,  
    // et donc, certaines des itérations de la boucle for auront pu  commencer à incrémenter total, mais une tâche démarrée plus en retard pourra remettre total à 0, écrasant ainsi les 
    // précédentes opérations qui cumulent dans total
#pragma  omp  for \
  reduction(+:total) // cas a) ici, total devient automatiquement privee et initialisee à 0 qui est l'operande neutre de +, et une somme (+) de ces copies de total 
 // sera faite automatiquement en sortie du for et ajoutée au total global (donc sa val initiale est importante à bien fixer).
 // La preuve etant que si on remplace le + par un *, alors la val initiale des vars. total privées est l'elt neutre de * = 1, puis, sera multipliée 
 // par la val initiale du total global, à la  sortie de boucle for. Il y a moins de contention d'accès exclusif au total global que pour le cas b), 
 // les perfs sont un peu meilleures
    for (i=0;i<n;i++) { 
      if (i==0)  {
	nthreads = omp_get_num_threads ();
	tid = omp_get_thread_num();
	fprintf(stdout, "Dans le for du Square avec Num of threads %i et nbr threads %i \n", tid,nthreads);
      }   
      table[i]*=table[i];
      if (i==2) // par exemple pour la 2eme itération
	fprintf(stdout,"A la %ieme  iteration, pour voir ce que la thread  %d a calcule comme sous total: %li\n",i, omp_get_thread_num(), total);
      //#pragma omp critical   
      total=total+table[i]; // cas b): sans protection, risque d'entrelacement. Donc mettre en critial section et laisser en shared (ce qui est le cas par défaut)      
      if (i==2) // ce test , pour eviter que toutes les threads fassent le print
	fprintf(stdout,"A la %ieme  iteration, pour voir ce que la thread  %d a calcule comme sous total: %li\n",i, omp_get_thread_num(), total);
    } //fin for, donc fin pragma for qui ne contient que cette instruction for
} // fin pragma parallel
  
  fprintf(stdout, "Total %ld \n", total);
}


void printArray(long *tableau, int n) {
  int i;
  for(i=0;i<n;i++) {
    fprintf(stdout, "%ld ", tableau[i]);
  }
  fprintf(stdout, "\n");
}
/*********** rappels de C **************************/
void fonction(int t[]) {int i; for (i=0;i<10;i++) printf("-%d",t[i]);} //! on va sortir "de la borne" sup, mais, sans gravité
void fonction2(int *t) {int i; for (i=0;i<2;i++) printf("=%d",t[i]);}
void fonction3(int t[]) {int i; for (i=0;i<2;i++) {printf("+%d",*t);t++;}}// t passe à l'adresse de l'entier suivant 
/*************************************************/

int main( int argc, char *argv[] ) {
  /**************rappels de C ***********************/
  //int tabint[2]={1,2};//ici alloué sur la stack, taille max limitée par la taille
  //de la stack, et si déborde, on a un stack overflow
  int* tabint=malloc(3*sizeof(int));//avec malloc, alloué sur le tas, avantage
  //est que malloc demanderait de l'espace mémoire en + à l'OS si plus assez de place pour
  //faire le malloc
  fonction(tabint);fonction2(tabint);fonction3(tabint);printf("\n");
  printf("\t Taille de tab (faux!) :%d\n",sizeof(tabint));//c'est pas la taille
  // de tabint, mais la taille de la variable pointeur tabint, qui est une adr sur 8 bytes
  /*************************************************/
  if (argc<2) {
    fprintf(stderr, "Usage: %s taille for the array, with by default values < %d\n", argv[0],1000);
    return -1;
  }
  int size = atoi(argv[1]);
 	
  fprintf(stdout, "Running for size %i\n", size);
  long *table;
  table= malloc(size*sizeof(long));
  fillArray(table, size, 1000);
  printArray(table, size);
  double d = omp_get_wtime();
  square(table, size);
  double d2 = omp_get_wtime()-d;
  fprintf(stdout, "Elapsed time 1 %f\n", d);
  fprintf(stdout, "Elapsed time 2 %f\n", d2);
 
}

