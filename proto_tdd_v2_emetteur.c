/*************************************************************
* proto_tdd_v0 -  émetteur                                   *
* TRANSFERT DE DONNEES  v2                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    paquet_t paquet; /* paquet utilisé par le protocole */
    paquet_t ACKpaquet;
    int paquet_recu = 0;
    int num_paquet = 0 , nb_retransmission = 0;
    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while ( taille_msg != 0 ) {
        
        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {

            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.num_seq = num_paquet;
        paquet.somme_ctrl =  generer_controle(paquet);
        
        vers_reseau(&paquet);
        
        depart_temporisateur(0,100);

        /* renvoi tant que acquittement négatif */
        while ((paquet_recu = attendre()) != -1 && nb_retransmission < 20){

            vers_reseau(&paquet);
            depart_temporisateur(0,100);
            nb_retransmission+=1;
            
        }


        num_paquet = 1 - num_paquet;
        paquet_recu = 0;
        nb_retransmission = 0;

        arreter_temporisateur(0);

        de_reseau(&ACKpaquet);
        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);

    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
