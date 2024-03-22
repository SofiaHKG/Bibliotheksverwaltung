#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum genre_type
{
    HORROR,
    ABENTEUER,
    ROMANTIK,
    SACHBUCH
} genre_type;

typedef struct book
{
    char title[32];
    genre_type genre;
    int year;
    int amount;
} book;

typedef struct bookNode
{
    book content;
    struct bookNode* next;
} bookNode;

typedef struct borrowNode
{
    char title[32];
    char name[32];
    struct borrowNode* next;
} borrowNode;

void errorMessage();
void insertBook(bookNode** inventory);
void borrowBook(bookNode* inventory, borrowNode** borrowed);
void returnBook(bookNode** inventory, borrowNode** borrowed);
char* printGenre(genre_type genre);
void printList(bookNode* inventory);
void printListInOrder(bookNode* inventory);
void freeMemory(bookNode** inventory, borrowNode** borrowed);

int main()
{
    bookNode* inventory = NULL;
    borrowNode* borrowed = NULL;

    char option;

    while (true) {
        printf("\nNeues Buch eingeben (n), Buch ausleihen (b), Buch zurueckgeben (r), Buecher auflisten (l), Buecher sortieren (s), Programm beenden (x)");
        printf("\nAuswahl: ");
        scanf(" %c", &option);

        if (option == 'x')
            break;
        else {
            switch (option) {
            case 'n':
                insertBook(&inventory);
                break;
            case 'b':
                borrowBook(inventory, &borrowed);
                break;
            case 'r':
                returnBook(&inventory, &borrowed);
                break;
            case 'l':
                printList(inventory);
                break;
            case 's':
                printListInOrder(inventory);
                break;
            default:
                errorMessage();
            }
        }
    }

    freeMemory(&inventory, &borrowed);

    return 0;
}

void errorMessage()
{
    printf("\nUngueltige Eingabe!");
}

void insertBook(bookNode** inventory)
{
    int genre;
    bookNode* newBookNode = (bookNode*)malloc(sizeof(bookNode));

    printf("\nGeben Sie den Titel ein: ");
    scanf("%s", newBookNode->content.title);

    while (true) {
        printf("\nGeben Sie das Genre ein. Horror (1), Abenteuer (2), Romantik (3), Sachbuch (4): ");
        scanf("%d", &genre);
        if (genre < 1 || genre > 4) {
            errorMessage();
            continue;
        } else {
            newBookNode->content.genre = (genre_type)(genre - 1);
            break;
        }
    }

    while (true) {
        printf("\nGeben Sie das Erscheinungsjahr ein: ");
        scanf("%d", &newBookNode->content.year);
        if (newBookNode->content.year < 1) {
            errorMessage();
            continue;
        } else {
            break;
        }
    }

    while (true) {
        printf("\nGeben Sie ein wieviele Exemplare vorhanden sind: ");
        scanf("%d", &newBookNode->content.amount);
        if (newBookNode->content.amount < 1) {
            errorMessage();
            continue;
        } else {
            break;
        }
    }

    newBookNode->next = *inventory;
    *inventory = newBookNode;
}

void borrowBook(bookNode* inventory, borrowNode** borrowed)
{
    printList(inventory);

    int choice;
    int valid = false;
    int count = 0;
    bookNode* selectedBook = NULL;

    if (inventory == NULL) {
        printf("\nEs sind keine Buecher im Inventar vorhanden.");
        return;
    }

    for (bookNode* current = inventory; current != NULL; current = current->next) {
        count++;
    }

    while (!valid) {
        printf("\nWelchen Titel moechten Sie leihen? (1-%d): ", count);
        scanf("%d", &choice);

        int count = 0;
        for (bookNode* current = inventory; current != NULL; current = current->next) {
            count++;
            if (count == choice) {
                selectedBook = current;
                valid = true;
                break;
            }
        }

        if (!valid) {
            errorMessage();
        }
    }

    char borrowersName[32];
    printf("\nGeben Sie Ihren Namen ein: ");
    scanf("%s", borrowersName);

    // check if a copy of the book is available
    if (selectedBook->content.amount <= 0) {
        printf("\nBereits alle Exemplare ausgeliehen!");
        return;
    }

    // check if the same person has already borrowed the same book
    for (borrowNode* current = *borrowed; current != NULL; current = current->next) {
        if (strcmp(current->name, borrowersName) == 0 && strcmp(current->title, selectedBook->content.title) == 0) {
            printf("\nSie haben diesen Titel bereits ausgeliehen!");
            return;
        }
    }

    // borrowing of the book
    borrowNode* newBorrowNode = (borrowNode*)malloc(sizeof(borrowNode));
    strcpy(newBorrowNode->title, selectedBook->content.title);
    strcpy(newBorrowNode->name, borrowersName);
    newBorrowNode->next = *borrowed;
    *borrowed = newBorrowNode;
    selectedBook->content.amount--;
}

void returnBook(bookNode** inventory, borrowNode** borrowed)
{
    borrowNode* current = *borrowed;
    int index = 1;
    int choice;

    if (current == NULL) {
        printf("\n\nEs sind keine Titel ausgeliehen!");
        return;
    }

    while (current != NULL) {
        printf("\n%d: %s geliehen von %s", index, current->title, current->name);
        current = current->next;
        index++;
    }

    printf("\n\nWelchen Titel moechten Sie retournieren? (1-%d): ", index - 1);
    scanf("%d", &choice);

    if (choice < 1 || choice >= index) {
        errorMessage();
        return;
    }

    // find selected book in list and remove
    borrowNode* previous = NULL;
    current = *borrowed;

    for (int i = 1; i < choice && current != NULL; i++) {
        previous = current;
        current = current->next;
    }

    char returnedTitle[32];
    strcpy(returnedTitle, current->title);

    // remove node
    if (previous == NULL) {
        *borrowed = current->next;
    } else {
        previous->next = current->next;
    }

    // free memory of current node
    free(current);

    // search inventory for returned book and update amount
    for (bookNode* bookCurrent = *inventory; bookCurrent != NULL; bookCurrent = bookCurrent->next) {
        if (strcmp(bookCurrent->content.title, returnedTitle) == 0) {
            bookCurrent->content.amount++;
            break;
        }
    }
}

char* printGenre(genre_type genre)
{
    static char* printString[] = { "Horror", "Abenteuer", "Romantik", "Sachbuch" };
    return printString[genre];
}

void printList(bookNode* inventory)
{
    bookNode* current = inventory;
    int index = 1;

    while (current != NULL) {
        printf("\n%d: %s, %s (%d)", index, current->content.title, printGenre(current->content.genre), current->content.year);
        current = current->next;
        index++;
    }
    printf("\n");
}

void printListInOrder(bookNode* inventory)
{
    bookNode* current = inventory;

    // count number of titles in list
    int count = 0;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    bookNode* array[count];
    current = inventory;

    for (int i = 0; i < count; i++) {
        array[i] = current;
        current = current->next;
    }

    // bubble sort
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (array[j]->content.year > array[j + 1]->content.year) {
                bookNode* tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }

    // print sorted list
    for (int i = 0; i < count; i++) {
        printf("\n%d: %s, %s (%d)", i + 1, array[i]->content.title, printGenre(array[i]->content.genre), array[i]->content.year);
    }
    printf("\n");
}

void freeMemory(bookNode** inventory, borrowNode** borrowed)
{
    bookNode* current = *inventory;

    while (current != NULL) {
        bookNode* tmp = current;
        current = current->next;
        free(tmp);
    }

    borrowNode* currentBorrowed = *borrowed;

    while (currentBorrowed != NULL) {
        borrowNode* tmp = currentBorrowed;
        currentBorrowed = currentBorrowed->next;
        free(tmp);
    }
}
