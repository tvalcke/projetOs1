#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 100

int main() {
    char cmd[MAX_CMD_LENGTH];
    FILE *file;
    char choice;
    int counter = 0; //Variable qui va compter le nbr de cmds entrées par l'user

    file = fopen("cmds.txt", "a");

    do {
        printf("Entrez une commande: ");
        fgets(cmd, MAX_CMD_LENGTH, stdin);

        if (strlen(cmd) > 0 && cmd[strlen(cmd) - 1] == '\n') {
            cmd[strlen(cmd) - 1] = '\0';
        }

        printf("Commande saisie: %s\n", cmd);

        pid_t pid = fork();

        if (pid == 0) {
            fprintf(file, "%s\n", cmd); // Écrit la commande dans le fichier
            system(cmd);
            exit(0);
        } else if (pid > 0) {
            wait(NULL);
            counter +=1; // incrémente le counter de commandes
        } else {
            fprintf(stderr, "Erreur lors de la création du processus fils\n");
            exit(1);
        }

        printf("Voulez-vous continuer ? (O/N) ");
        scanf(" %c", &choice);
        getchar();
    } while (choice != 'N' && choice != 'n');

    fclose(file);

    printf("le nombre de commandes entrée(s) est de: ");
    printf("%d", counter);
    printf("\n");
    return 0;
}
