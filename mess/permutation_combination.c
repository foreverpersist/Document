#include <stdio.h>
#include <string.h>

int combination(int n, void (*func)(int *, int));

int permutation(int n, int m, void (*func)(int *, int));

static void swap(int *arr, int i, int j)
{
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

/*
  Generate all combinations
  Params:
    @n: number of elements
    @func: function to deal with combinations whose 
     input contains an index array and its length
*/
int combination(int n, void (*func)(int *, int))
{
	if (n <= 0)
		return 0;
	int arr[n];
	int i;
	int j;
	unsigned cnt = 0;

	/* Initialize index array */
	for (i = 0; i < n; i++)
	{
		arr[i] = i;
	}
	
	cnt++;
	if (func)
		func(arr, n);

	while (1)
	{
		/* Find max i which satisfies arr[i] < arr[i+1] */
		for (i = n-2; i >= 0; i--)
		{
			if (arr[i] < arr[i+1])
				break;
		}
		if (i < 0)
			break;

		/* Find max j which satisfies arr[j] > arr[i] */
		for (j = n-1; j > i; j--)
		{
			if (arr[j] > arr[i])
				break;
		}

		/* Swap arr[i] and arr[j] */
		swap(arr, i, j);

		/* Inverse arr[i+1], arr[i+2], ..., arr[n-1] */
		for (j = i + 1; j < (n + i + 1) / 2; j++)
		{
			swap(arr, j, n + i - j);
		}

		cnt++;

		/* Invoke function func */
		if (func)
			func(arr, n);
	}

	return cnt;
}

/*
  Generate all permutations
  Params:
    @n: number of all elements
    @m: number of needed elements
    @func: function to deal with permutations whose 
     input contains an flag array and its length
*/
int permutation(int n, int m, void (*func)(int *, int))
{
	if (n < m || m <= 0)
		return 0;
	int arr[n];
	int i;
	int j;
	int num;
	unsigned cnt = 0;

	/* Initialize flag array */
	memset(arr, 0, sizeof(int) * (n - m));
	for (i = n - m; i < n; i++)
		arr[i] = 1;

	cnt++;

	if (func)
		func(arr, n);
	
	while (1)
	{
		/* Find max i which satisfies p[i] < p[i+1]*/
		for (i = n-2; i >= 0; i--)
		{
			if (arr[i] < arr[i+1])
				break;
		}
		if (i < 0)
			break;

		/* Swap arr[i] and arr[i+1] */
		swap(arr, i, i+1);

		/* Move all 1 elements right of arr[i+1] to rightmost*/
		num = 0;
		for (j = i + 2; j < n; j++)
		{
			if (arr[j]  == 1)
				num++;	
		}
		memset(arr + i + 2, 0, sizeof(int) * (n - i - 2 - num));
		for (j = n - num; j < n; j++)
			arr[j] = 1;

		cnt++;

		/* Invoke function func */
		if (func)
			func(arr, n);
	}

	return cnt;
}

void print_combination(int *arr, int len)
{
	int i;
	printf("[");
	for (i = 0; i < len; i++)
	{
		printf("%2d, ", arr[i]);
	}
	printf("]\n");
}

void print_permutation(int *arr, int len)
{
	int i;
	printf("[");
	for (i = 0; i < len; i++)
	{
		printf("%2d, ", arr[i]);
	}
	printf("]\n");
}

int main(int argc, char *argv[])
{
	unsigned cnt;

	printf("combination 6\n");
	cnt = combination(6, print_combination);
	printf("Total: %d\n", cnt);

	printf("permutation 6, 3\n");
	cnt = permutation(6, 3, print_permutation);
	printf("Total: %d\n", cnt);
	return 0;
}
