#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_ALPHABET_SIZE 26
#define MAX_STATES 100

typedef struct {
    char alphabet[MAX_ALPHABET_SIZE]; // A
    int alphabetSize;                // ||A||
    int *states;                     // S
    int statesNumber;                // ||S||
    int initialState;                // s0
    int *finalStates;                // F
    int finalStatesNumber;           // ||F||
    int transitions[MAX_STATES][MAX_ALPHABET_SIZE]; // S(3)_A(1)_S'(3)
} FiniteAutomata;

void markReachableStates(FiniteAutomata automaton, bool *reachable, int currentState) {
    reachable[currentState] = true;

    for (int i = 0; i < automaton.alphabetSize; i++) {
        int nextState = automaton.transitions[currentState][i];
        if (nextState != -1 && !reachable[nextState]) {
            markReachableStates(automaton, reachable, nextState);
        }
    }
}

void detectUnreachableStates(FiniteAutomata automaton, int *unreachableStates) {
    bool reachable[MAX_STATES] = {false};
    markReachableStates(automaton, reachable, automaton.initialState);

    printf("Unreachable states: ");
    for (int i = 0; i < automaton.statesNumber; i++) {
        if (!reachable[i]) {
            unreachableStates[i] = automaton.states[i];
        }
    }
    printf("\n");
}

void detectDeadEndStates(FiniteAutomata automaton, int *deadEndStates) {
    bool hasOutgoingTransition[MAX_STATES] = {false};

    for (int i = 0; i < automaton.statesNumber; i++) {
        for (int j = 0; j < automaton.alphabetSize; j++) {
            int nextState = automaton.transitions[i][j];
            if (nextState != -1) {
                hasOutgoingTransition[i] = true;
                break;
            }
        }
    }

    printf("Dead end states: ");
    for (int i = 0; i < automaton.statesNumber; i++) {
        if (!hasOutgoingTransition[i]) {
            deadEndStates[i] = automaton.states[i];
        }
    }
    printf("\n");
}

int isWordValid(const char *word) {
    if (strlen(word) == 1) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    char englishAlphabet[] = "abcdefghijklmnopqrstuvwxyz";

    char filename[260] = "C://Work//mi31//system_programming//lab2//lab2_var2_text.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", filename);
        perror("fopen");
        exit(1);
    } else {
        printf("File opened successfully.\n");
    }

    FiniteAutomata automaton;

    int alphabetSizeBuffer, statesNumberBuffer, initialStateBuffer, finalStatesNumberBuffer;

    if (fscanf(file, "%d", &alphabetSizeBuffer) != 1 || alphabetSizeBuffer > MAX_ALPHABET_SIZE || alphabetSizeBuffer <= 0) {
        printf("Entered alphabet size is either in incorrect format, > 26, or <= 0.\n");
        exit(1);
    } else {
        automaton.alphabetSize = alphabetSizeBuffer;
    }

    for (int i = 0; i < alphabetSizeBuffer; i++) {
        automaton.alphabet[i] = englishAlphabet[i];
    }

    if (fscanf(file, "%d", &statesNumberBuffer) != 1 || statesNumberBuffer > MAX_STATES || statesNumberBuffer < 0) {
        printf("Entered state number is either in incorrect format, > 100, or < 0.\n");
        exit(1);
    } else {
        automaton.statesNumber = statesNumberBuffer;
    }

    automaton.states = (int *)malloc(sizeof(int) * automaton.statesNumber);
    if (automaton.states == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < automaton.statesNumber; i++) {
        automaton.states[i] = i;
    }

    if (fscanf(file, "%d", &initialStateBuffer) != 1 || initialStateBuffer > automaton.statesNumber || initialStateBuffer < 0) {
        printf("Entered initial state is either in incorrect format, bigger than state number, or < 0.\n");
        exit(1);
    } else {
        automaton.initialState = initialStateBuffer;
    }

    if (fscanf(file, "%d", &finalStatesNumberBuffer) != 1 || finalStatesNumberBuffer > automaton.statesNumber || finalStatesNumberBuffer < 0) {
        printf("Entered final states number is either in incorrect format, bigger than states number, or < 0.\n");
        exit(1);
    } else {
        automaton.finalStatesNumber = finalStatesNumberBuffer;
    }

    automaton.finalStates = (int *)malloc(sizeof(int) * automaton.finalStatesNumber);
    if (automaton.finalStates == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < automaton.finalStatesNumber; i++) {
        int finalStateBuffer;
        if (fscanf(file, "%d", &finalStateBuffer) != 1 || finalStateBuffer > automaton.statesNumber || finalStateBuffer < 0) {
            printf("Entered final state is either in incorrect format, bigger than final states number, or < 0.\n");
            exit(1);
        } else {
            automaton.finalStates[i] = finalStateBuffer;
        }
    }

    // transitions
    // Initialize transitions with -1
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_ALPHABET_SIZE; j++) {
            automaton.transitions[i][j] = -1;
        }
    }

    while (!feof(file)) {
        int stateBuffer, nextStateBuffer;
        char letterBuffer[3];
        if (fscanf(file, "%d %c %d", &stateBuffer, letterBuffer, &nextStateBuffer) != 3) {
            printf("Incorrect transition format.\n");
            exit(1);
        }

        if (strlen(letterBuffer) != 1 || !isalpha(letterBuffer[0])) {
            printf("Incorrect letter format: %s\n", letterBuffer);
            exit(1);
        }

        char letter = tolower(letterBuffer[0]);

        bool letterInAlphabet = false;
        for (int i = 0; i < automaton.alphabetSize; i++)
            if (letter == automaton.alphabet[i])
                letterInAlphabet = true;

        if (!letterInAlphabet) {
            printf("Entered transition is incorrect. Unknown letter: %c\n", letter);
            exit(1);
        }

        int alphabetIndex = 0;
        if (stateBuffer > automaton.statesNumber || stateBuffer < 0 || nextStateBuffer > automaton.statesNumber || nextStateBuffer < 0) {
            printf("Entered transition is incorrect.\n");
            exit(1);
        }
        for (; alphabetIndex < automaton.alphabetSize; alphabetIndex++) {
            if (automaton.alphabet[alphabetIndex] == letter) {
                break;
            }
        }

        automaton.transitions[stateBuffer][alphabetIndex] = nextStateBuffer;
    }
    fclose(file);
    printf("\n");
    int unreachableStates[automaton.statesNumber];
    for (int i = 0; i < automaton.statesNumber; i++) {
        unreachableStates[i] = -1;
    }

    detectUnreachableStates(automaton, unreachableStates);
    for (int i = 0; i < automaton.statesNumber; i++) {
        if (unreachableStates[i] != -1)
            printf("%d ", unreachableStates[i]);
    }
    printf("\n");
    printf("\n");

    int deadEndStates[automaton.statesNumber];
    for (int i = 0; i < automaton.statesNumber; i++) {
        deadEndStates[i] = -1;
    }

    detectDeadEndStates(automaton, deadEndStates);

    for (int i = 0; i < automaton.statesNumber; i++) {
        if (deadEndStates[i] != -1) {
            bool isUnreachable = false;
            bool isFinal = false;

            for (int j = 0; j < automaton.statesNumber; j++) {
                if (deadEndStates[i] == unreachableStates[j]) {
                    isUnreachable = true;
                    break;
                }
                if (deadEndStates[i] == automaton.finalStates[j]) {
                    isFinal = true;
                    break;
                }
            }

            if (!isUnreachable && !isFinal) {
                printf("%d ", deadEndStates[i]);
            }
        }
    }
    printf("\n");
    free(automaton.states);
    free(automaton.finalStates);

    return 0;
}

