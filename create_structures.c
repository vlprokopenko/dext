#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"

#define ALLOC_STEP 64


// initialize and free structures
int init_word(struct Word** new_word) {
    *new_word = malloc(sizeof(**new_word));
    if (!*new_word) {
        return 1;
    }
    memset(*new_word, 0, sizeof(**new_word));

    (*new_word)->characters = malloc(ALLOC_STEP * sizeof(char));
    if (!(*new_word)->characters) {
        return 1;
    }

    (*new_word)->size = 0;
    (*new_word)->allocated = ALLOC_STEP;
    return 0;
}

int allocate_memory_to_word(struct Word** word) {
    int old_alloc_count = (*word)->allocated;
    int new_alloc_count = old_alloc_count + ALLOC_STEP;
    char* new_allocation = NULL;
    new_allocation = realloc((*word)->characters,
                             new_alloc_count * sizeof(char));
    if (!new_allocation) {
        return 1;
    }

    (*word)->characters = new_allocation;
    (*word)->allocated = new_alloc_count;
    memset((*word)->characters + old_alloc_count, 0, ALLOC_STEP);

    return 0;
}

void free_word(struct Word** dead_word) {
    free((*dead_word)->characters);
    (*dead_word)->characters = NULL;
    free(*dead_word);
    *dead_word = NULL;
}

int init_line(struct Line** new_line) {
    *new_line = malloc(sizeof(**new_line));
    if (!(*new_line)) {
        return 1;
    }
    memset(*new_line, 0, sizeof(**new_line));

    (*new_line)->words = malloc(ALLOC_STEP * sizeof(struct Word*));
    if (!(*new_line)->words) {
        return 1;
    }
    memset((*new_line)->words, 0, ALLOC_STEP * sizeof(struct Word*));

    (*new_line)->size = 0;
    (*new_line)->allocated = ALLOC_STEP;

    return 0;
}

int allocate_memory_to_line(struct Line** line) {
    int old_alloc_count = (*line)->allocated;
    int new_alloc_count = old_alloc_count + ALLOC_STEP;
    struct Word** new_allocation = NULL;
    new_allocation = realloc((*line)->words,
                             new_alloc_count * sizeof(struct Word*));
    if (!new_allocation) {
        return 1;
    }

    (*line)->words = new_allocation;
    (*line)->allocated = new_alloc_count;
    memset((*line)->words + old_alloc_count, 0, ALLOC_STEP);

    return 0;
}

void free_line(struct Line** dead_line) {
    for (int i = 0; i < (*dead_line)->size; i++) {
        free_word(&(*dead_line)->words[i]);
        free((*dead_line)->words[i]);
        (*dead_line)->words[i] = NULL;
    }
    free((*dead_line)->words);
    (*dead_line)->words = NULL;
    free(*dead_line);
    *dead_line = NULL;
}

int init_paragraph(struct Paragraph** new_paragraph) {
    *new_paragraph = malloc(sizeof(**new_paragraph));
    if (!new_paragraph) {
        return 1;
    }
    memset(*new_paragraph, 0, sizeof(**new_paragraph));

    (*new_paragraph)->formatted_lines = malloc(ALLOC_STEP * sizeof(struct Line*));
    if (!(*new_paragraph)->formatted_lines) {
        return 1;
    }
    memset((*new_paragraph)->formatted_lines, 0, ALLOC_STEP * sizeof(struct Line*));

    (*new_paragraph)->size = 0;
    (*new_paragraph)->allocated = ALLOC_STEP;
    (*new_paragraph)->content = malloc(sizeof(struct Line));

    return 0;
}

int allocate_memory_to_paragraph(struct Paragraph** paragraph) {
    int old_alloc_count = (*paragraph)->allocated;
    int new_alloc_count = old_alloc_count + ALLOC_STEP;
    struct Line** new_allocation = NULL;
    new_allocation = realloc((*paragraph)->formatted_lines,
                             new_alloc_count * sizeof(struct Line*));
    if (!new_allocation) {
        return 1;
    }

    (*paragraph)->formatted_lines = new_allocation;
    (*paragraph)->allocated = new_alloc_count;
    memset((*paragraph)->formatted_lines + old_alloc_count, 0, ALLOC_STEP);

    return 0;
}

void free_paragraph(struct Paragraph** dead_paragraph) {
    for (int i = 0; i < (*dead_paragraph)->size; i++) {
        free_line(&(*dead_paragraph)->formatted_lines[i]);
        free((*dead_paragraph)->formatted_lines[i]);
        (*dead_paragraph)->formatted_lines[i] = NULL;
    }
    free((*dead_paragraph)->formatted_lines);
    (*dead_paragraph)->formatted_lines = NULL;
    if ((*dead_paragraph)->content) {
        free_line(&(*dead_paragraph)->content);
        free((*dead_paragraph)->content);
        (*dead_paragraph)->content = NULL;
    }
    free(*dead_paragraph);
    *dead_paragraph = NULL;
}

int init_body(struct Body** new_body) {
    *new_body = malloc(sizeof(**new_body));
    if (!new_body) {
        return 1;
    }
    memset(*new_body, 0, sizeof(**new_body));

    (*new_body)->paragraphs = malloc(ALLOC_STEP * sizeof(struct Paragraph*));
    if (!(*new_body)->paragraphs) {
        return 1;
    }
    memset((*new_body)->paragraphs, 0, ALLOC_STEP * sizeof(struct Paragraph*));

    (*new_body)->size = 0;
    (*new_body)->allocated = ALLOC_STEP;

    return 0;
}

int allocate_memory_to_body(struct Body** body) {
    int old_alloc_count = (*body)->allocated;
    int new_alloc_count = old_alloc_count + ALLOC_STEP;
    struct Paragraph** new_allocation = NULL;
    new_allocation = realloc((*body)->paragraphs,
                             new_alloc_count * sizeof(struct Paragraph*));
    if (!new_allocation) {
        return 1;
    }

    (*body)->paragraphs = new_allocation;
    (*body)->allocated = new_alloc_count;
    memset((*body)->paragraphs + old_alloc_count, 0, ALLOC_STEP);

    return 0;
}

void free_body(struct Body** dead_body) {
    for (int i = 0; i < (*dead_body)->size; i++) {
        free_paragraph(&(*dead_body)->paragraphs[i]);
        (*dead_body)->paragraphs[i] = NULL;
    }
    free((*dead_body)->paragraphs);
    (*dead_body)->paragraphs = NULL;
    free(*dead_body);
    *dead_body = NULL;
}


// take a buffer and split it into components
int break_into_words(struct Line** line, char* buffer, int buff_size) {
    int idx = 0;

    while (idx < buff_size && buffer[idx] != '\0' && buffer[idx] != EOF) {
        struct Word* new_word = NULL;
        init_word(&new_word);

        while (buffer[idx] != ' ' &&
               buffer[idx] != '\n' &&
               buffer[idx] != '\t' &&
               buffer[idx] != EOF &&
               buffer[idx] != '\0') {
            new_word->characters[new_word->size] = buffer[idx];
            new_word->size++;
            if (new_word->size == new_word->allocated) {
                allocate_memory_to_word(&new_word);
            }
            idx++;
        }
        idx++; // move past the space

        new_word->characters[new_word->size] = '\0';
        (*line)->words[(*line)->size] = new_word;
        (*line)->size++;
        if ((*line)->size == (*line)->allocated) {
            allocate_memory_to_line(line);
        }
    }

    return (*line)->size;
}

int break_into_paragraphs(struct Body** body, char* buffer, int buff_size) {
    int idx = 0;
    char* temp_buffer = NULL;
    temp_buffer = malloc(ALLOC_STEP * sizeof(char));
    if (!temp_buffer) {
        return -1;
    }
    int allocated_to_temp_buffer = ALLOC_STEP;

    while (idx < buff_size && buffer[idx] != '\0') {
        int temp_buffer_idx = 0;
        memset(temp_buffer, 0, allocated_to_temp_buffer);

        if (temp_buffer_idx == 0 && buffer[idx] == '\n') {
            idx++;
            continue;
        }

        struct Line* main_line = NULL;
        init_line(&main_line);
        struct Paragraph* new_paragraph = NULL;
        init_paragraph(&new_paragraph);

        while (buffer[idx] != '\n') {
            temp_buffer[temp_buffer_idx] = buffer[idx];
            temp_buffer_idx++;
            idx++;

            if (temp_buffer_idx == allocated_to_temp_buffer) {
                int new_buffsize = allocated_to_temp_buffer + ALLOC_STEP;
                char* new_allocation = realloc(temp_buffer, new_buffsize);
                if (!new_allocation) {
                    return -1;
                }
                temp_buffer = new_allocation;
                memset(temp_buffer + allocated_to_temp_buffer, 0, ALLOC_STEP);
                allocated_to_temp_buffer = new_buffsize;
            }
        }
        idx++;
        
        break_into_words(&main_line,
                         temp_buffer,
                         allocated_to_temp_buffer);
        new_paragraph->content = main_line;
        (*body)->paragraphs[(*body)->size] = new_paragraph;
        (*body)->size++;
        if ((*body)->size == (*body)->allocated) {
            allocate_memory_to_body(body);
        }
    }

    free(temp_buffer);
    return (*body)->size;
}
