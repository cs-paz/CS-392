/*******************************************************************************
 * Name        : quicksort.h
 * Author      : Christian Szablewski-Paz and Zac Schuh
 * Date        : 3/3/21
 * Description : Quicksort header.
 * Pledge      : I pledge my honor that I have abided by the Stevens honor system.
 ******************************************************************************/

int int_cmp(const void *a, const void *b);

int dbl_cmp(const void *a, const void *b);

int str_cmp(const void *a, const void *b);

void quicksort(void *array, size_t len, size_t elem_sz, int (*comp) (const void*, const void*));




