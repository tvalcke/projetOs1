#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_LEN 1024	// Définis la taille maximale pour les chaines de caractères

int main(int argc, char *argv[]) {
    int f, p, s, c = 0;
    int pipefd[2];	// 0 c'est la lecture et 1 c'est l'écriture
    char cmd[MAX_LEN];	// ch de caract qui permet de stocker les commandes
    char buf[MAX_LEN];	//idem
    char ch = 'O';

    f = open("cmds.sh", O_WRONLY | O_CREAT | O_TRUNC, 0755); // fichier et permissions  0, 7 pour lecture ecriture et execution ( proprio du fichier), 5 pour lecture et execution
    if (f == -1) {
        printf("Erreur ouverture fichier\n");
        exit(1);
    }

    // On écris l'en tete du file shell
    char header[] = "#!/bin/bash\n";
    if (write(f, header, strlen(header)) == -1){
        printf("Erreur écriture en-tête du script\n");
        exit(1);
    }

    if (pipe(pipefd) == -1) {	//pipe est le tube de communication entre le fls et le père
        perror("pipe");
        exit(1);
    }

    if (argc > 1) { // verifier si on a bien recu au moins un argument lors de l'appel
        // concaténation des arguments (par exemple 'mkdir' + 'dossier1')
        strcpy(cmd, argv[1]);	//écrit le premier arg lors de l'appel
        for (int i = 2; i < argc; i++) {	// concatène les autres arg en mettant un espace entre à chaque fois
            strcat(cmd, " ");
            strcat(cmd, argv[i]);
        }
        printf("Commande passée en argument : %s\n", cmd);
        c++;	// incrément du nbr de cmds pasées 
        if (write(f, cmd, strlen(cmd)) == -1 || write(f, "\n", 1) == -1) { //ecriture dans le .sh
            printf("Erreur écriture commande dans le fichier\n");
            exit(1);
        }
    }

    do {

        if (argc <= 1 || ch != 'N' && ch != 'n') {
            printf("Entrez une commande: ");
            gets(cmd, MAX_LEN, stdin);	// gets récupère la commande
            cmd[strcspn(cmd, "\n")] = 0;
        }

        printf("Commande: %s\n", cmd);

        p = fork();		//le père crée un process fils grace à fork

        if (p == -1) {
            printf("Erreur création processus fils\n");
            exit(1);
        } else if (p == 0) {
            // Processus fils
            close(pipefd[0]);	// on ferme la lecture pcq on en a pas besoin
            dup2(pipefd[1], STDOUT_FILENO);	// redirige la sortie du fils vers le coté écriture du tube
            close(pipefd[1]);
            execlp("/bin/sh", "/bin/sh", "-c", cmd, NULL);	// on pourrait commenter ces 2 lignes pour éviter les conflits lors de l'appel ultérieur du fichier avec les instructions
            perror("exec");
            exit(1);
        } else {
            // process père
            close(pipefd[1]);
            ssize_t nbytes;
            while ((nbytes = read(pipefd[0], buf, MAX_LEN)) > 0) { // lecture du pipe	avec read 
                write(STDOUT_FILENO, buf, nbytes);
            }
            close(pipefd[0]);

            wait(NULL);	//attendre que le processus fils soit fini avant de continuer

            if (write(f, cmd, strlen(cmd)) == -1 || write(f, "\n", 1) == -1) { // écriture dans le .sh + vérification erreur
                printf("Erreur écriture commande dans le fichier\n");
                exit(1);
            }
            c++;
        }

        if (argc <= 1 || ch != 'N' && ch != 'n') {
            printf("Continuer ? (O/N) ");
            scanf(" %c", &ch);
            getchar();
        }

    } while (ch != 'N' && ch != 'n');

    close(f);

    printf("Nombre de commandes: %d\n", c);
    return 0;
}















