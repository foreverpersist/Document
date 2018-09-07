#Author: persist
#Abstract: how to use some keywords in C

# const

## Decorate basic type

	There is no difference between following 2 lines.
	Decorating variable with const means that it is a constantant which can not be assigned again.

``
	const int n = 1;
	int const n = 1; 
``

## Decorate pointer

	There are two different cases when decorating pointer.

### Constant pointer

	Using `const` in front of `*` like following example means that it is a constant pointer. The value pointed by the pointer can not be directly changed by it, but the pointer can be assigned again.

```
	int a = 1;
	int b = 2;
	const int *n = &a;
	// Error: *n = b;
	n = &b;
```

## Pointer constant

	Using `const` behind `*` like following example means that is is a pointer constant. The pointer can not be assigned again, but the value pointed by the pointer can be changed.

```
	int a = 1;
	int b = 2;
	int * const n = &a;
	// Error: n = &b;
	*n = b;
```

## Constant pointer pointed at constant

	Using double `const` like following example.

```
	const int * const p;
```
