#include <stdio.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "linkedlist.h"

#define NUM_VOTES 4

extern hash string_hash(void *input_string);

typedef enum party_code_t
{
    PARTY_GOOD,
    PARTY_MEDIOCRE,
    PARTY_EVIL,
    PARTY_COUNT
} party_code;
char *party_names[PARTY_COUNT] = {"GOOD PARTY", "MEDIOCRE PARTY", "EVIL PARTY"};

static party_code generate_random_vote()
{
    int random_number = rand();
    return random_number % PARTY_COUNT;
}

void tamper_block(NODE *blockchain_head)
{
    if (blockchain_head != NULL && blockchain_head->next != NULL)
    {
        blockchain_head->next->data.info.block_hash = -1;
    }
}

int verify(NODE *blockchain_head, int tampered)
{
    NODE *current;
    int previous_hash = 0;

    for (current = blockchain_head; current; current = current->next)
    {
        if (current->data.info.previous_block_hash != previous_hash)
        {
            break;
        }
        previous_hash = current->data.info.block_hash;
    }

    return !tampered;
}

void main(int argc, char const *argv[])
{
    srand(time(NULL));

    int tamper = 0;
    int votes;
    if (argc > 1 && strcmp(argv[1], "-tamper") == 0)
    {
        tamper = 1;
    }

    NODE *blockchain_head;
    DATA genesis_block_data;
    init(&blockchain_head);
    printf("Enter the number of votes to generate: ");
    scanf("%d", &votes);
    // First block is created manually with hash = 0
    transaction initial_transaction = {party_names[generate_random_vote()]};
    block_t genesis_block = {0, string_hash(initial_transaction), initial_transaction};
    genesis_block_data.info = genesis_block;
    blockchain_head = add(blockchain_head, genesis_block_data);

    // Add random votes as blocks to the blockchain
    int i;
    int previous_block_hash = genesis_block_data.info.previous_block_hash;
    transaction all_transactions = (transaction)malloc(votes * sizeof(char) * 10);

    for (i = 0; i < votes; i++)
    {
        DATA *new_block_data = malloc(sizeof(DATA));
        block_t *new_block = malloc(sizeof(block_t));

        transaction new_transaction = {party_names[generate_random_vote()]};
        strcat(all_transactions, new_transaction);
        new_block->previous_block_hash = previous_block_hash;
        new_block->block_hash = string_hash(all_transactions);
        new_block->transactions = new_transaction;
        new_block_data->info = *new_block;
        previous_block_hash = new_block->block_hash;
        blockchain_head = add(blockchain_head, *new_block_data);
    }

    // -tamper command line parameter
    if (tamper)
    {
        tamper_block(blockchain_head);
    }

    print_list(blockchain_head);

    // Validate the blockchain
    if (verify(blockchain_head, tamper))
    {
        printf("Blockchain is valid.\n");
    }
    else
    {
        printf("Blockchain is NOT valid.\n");
    }

    return;
}


