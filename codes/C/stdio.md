#Author: persist
#Abstract: some codes about standard input/output process

# Read a line

```
scanf("%[^\n]\n", str_line);
```

# Separate string with specific tokens

## Simply separate

	string: "abc;def ghi:jkl"
	delim: ":; "

> * abc
> * def
> * ghi
> * jkl

```
char *src = "abc;def ghi:jkl";
char *delim = ":; "

sub_str = strtok(str, delim);
printf("%s\n", sub_str);

while((sub_str = strtok(NULL, delim)) != NULL)
{
    printf("%s\n", sub_str);
}
```

## Hierarchically separe

	string: "a/bbb///cc;xxx:yyy:"
	delim: ":;"
	sub-delim: "/"

> * a/bbb///cc
> ** a
> ** bbb
> ** cc

> * xxx
> ** xxx

> * yyy
> ** yyy

```
char *str1, *str2;
char *delim, *subdelim;
char *token, *subtoken;
char *saveptr1, *saveptr2;

str1 = "a/bbb///cc;xxx:yyy:";
delim = ";:";
subdelim = "/";

while(1) 
{
    token = strtok_r(str1, delim, &saveptr1);
    if(token == NULL)
        break;
    printf("-> %s", token);

    while(1)
    {
        sub_token = strtok_r(str2, subdelim, &saveptr2);
        if(sub_token == NULL)
            break;
        printf("--> %s", sub_token);
        str2 = NULL;
    }
    str1 = NULL;
}

``` 

# Redirect standard input/output/error

## freopen

```
freopen("file1", "r", stdin);
freopen("file2", "w", stdout);
freopen("file3", "w", stderr);
```

## dup2

```
dup2(fd1, STDIN_FILENO);
dup2(fd2, STDOUT_FILENO);
dup3(fd3, STDERR_FILENO);
```
