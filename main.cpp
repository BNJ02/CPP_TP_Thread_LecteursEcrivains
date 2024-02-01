/* Ce code utilise des sémaphores pour synchroniser les threads lecteurs et écrivains 
afin d'éviter les problèmes de concurrence lors de l'accès à une ressource partagée. 
Le but est de permettre à plusieurs lecteurs d'accéder simultanément à la ressource, 
mais d'empêcher les écrivains d'y accéder lorsqu'un lecteur ou un autre écrivain est actif. 
De plus, le code intègre une gestion des écrivains en attente pour éviter une éventuelle 
coalition des lecteurs qui prive l'accès des écrivains.*/

#include <iostream>                     // Inclusion de la bibliothèque iostream pour l'entrée/sortie standard
#include <thread>                       // Inclusion de la bibliothèque thread pour la gestion des threads
#include "Semaphore.hpp"                // Inclusion du fichier Semaphore.hpp pour utiliser les sémaphores

int ressourcePartagee;                  // Variable partagée par les threads lecteurs et écrivains
Semaphore w_mutex(1);                   // Sémaphore pour l'exclusion mutuelle des écrivains
Semaphore r_mutex(1);                   // Sémaphore pour la gestion des lecteurs
Semaphore mutex_waiting_writers(1);     // Sémaphore pour l'exclusion mutuelle des accès à waiting_writers
int readers = 0;                        // Compteur pour le nombre de lecteurs actifs
int waiting_writers = 0;                // Compteur pour le nombre d'écrivains en attente

void lecteur(int numLecteur) {
  for (int i = 0; i < 4; i++) {         // Boucle pour simuler 4 accès à la ressource partagée
    r_mutex.P(numLecteur);              // P(r_mutex) : acquisition du sémaphore r_mutex
    readers++;                         // Incrémentation du compteur de lecteurs
    if (readers == 1) {                // Si c'est le premier lecteur
      mutex_waiting_writers.P(numLecteur);   // P(mutex_waiting_writers) : acquisition du sémaphore pour l'accès à waiting_writers
      waiting_writers--;              // Décrémentation du compteur d'écrivains en attente
      w_mutex.V(numLecteur);            // V(w_mutex) : libération du sémaphore w_mutex (si besoin)
      mutex_waiting_writers.V(numLecteur); // V(mutex_waiting_writers) : libération du sémaphore pour l'accès à waiting_writers
    }
    r_mutex.V(numLecteur);              // V(r_mutex) : libération du sémaphore r_mutex

    std::cout << "Lecteur n° " << numLecteur << " en cours " << endl;    // Affichage du message indiquant que le lecteur est en cours
    this_thread::sleep_for(chrono::milliseconds(rand() % 20000)); // Pause aléatoire pour simuler le temps de lecture
    std::cout << "        Valeur lue = " << ressourcePartagee << "  " << endl;  // Affichage de la valeur lue

    r_mutex.P(numLecteur);              // P(r_mutex) : acquisition du sémaphore r_mutex
    readers--;                         // Décrémentation du compteur de lecteurs
    if (readers == 0) {                // Si tous les lecteurs ont terminé
      w_mutex.V(numLecteur);            // V(w_mutex) : libération du sémaphore w_mutex (pour permettre aux écrivains d'accéder à la ressource)
    }
    r_mutex.V(numLecteur);              // V(r_mutex) : libération du sémaphore r_mutex
  }
}

void ecrivain(int numEcrivain) {
  int x;                               // Variable temporaire pour stocker la valeur à incrémenter
  for (int i = 0; i < 4; i++) {         // Boucle pour simuler 4 accès à la ressource partagée
    mutex_waiting_writers.P(numEcrivain); // P(mutex_waiting_writers) : acquisition du sémaphore pour l'accès à waiting_writers
    waiting_writers++;                // Incrémentation du compteur d'écrivains en attente
    mutex_waiting_writers.V(numEcrivain); // V(mutex_waiting_writers) : libération du sémaphore pour l'accès à waiting_writers

    w_mutex.P(numEcrivain);            // P(w_mutex) : acquisition du sémaphore w_mutex (pour l'exclusion mutuelle des écrivains)

    std::cout << "Ecrivain n° " << numEcrivain << " en cours " << endl;   // Affichage du message indiquant que l'écrivain est en cours
    x = ressourcePartagee;              // Récupération de la valeur actuelle de la ressource partagée
    this_thread::sleep_for(chrono::milliseconds(rand() % 20000)); // Pause aléatoire pour simuler le temps d'écriture
    std::cout << "valeur à incrémenter de la ressourcePartagee = " << x << "  " << endl;  // Affichage de la valeur à incrémenter
    ressourcePartagee = x + 1;          // Incrémentation de la valeur de la ressource partagée

    w_mutex.V(numEcrivain);            // V(w_mutex) : libération du sémaphore w_mutex (pour permettre aux autres écrivains/lecteurs d'accéder à la ressource)
    mutex_waiting_writers.P(numEcrivain);   // P(mutex_waiting_writers) : acquisition du sémaphore pour l'accès à waiting_writers
    waiting_writers--;                // Décrémentation du compteur d'écrivains en attente
    if (waiting_writers == 0) {       // Si aucun écrivain n'est en attente
      r_mutex.V(numEcrivain);          // V(r_mutex) : libération du sémaphore r_mutex (pour permettre aux lecteurs d'accéder à la ressource)
    }
    mutex_waiting_writers.V(numEcrivain); // V(mutex_waiting_writers) : libération du sémaphore pour l'accès à waiting_writers
  }
}

int main() {
  const int nbr = 4;                   // Nombre de threads (lecteurs et écrivains)
  std::thread r[nbr];                  // Tableau pour stocker les threads lecteurs
  std::thread w[nbr];                  // Tableau pour stocker les threads écrivains

  for (int i = 1; i < nbr; i++) {       // Lancement des threads
    r[i] = std::thread(lecteur, -i);   // Lancement d'un thread lecteur
    w[i] = std::thread(ecrivain, i);   // Lancement d'un thread écrivain
  }

  // Join des threads 
  for (int i = 1; i < nbr; i++) {       // Attente de la terminaison des threads
    r[i].join();                       // Attente de la terminaison du thread lecteur
    w[i].join();                       // Attente de la terminaison du thread écrivain
  }
  return 0;
}