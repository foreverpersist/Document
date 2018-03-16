#Author: persist
#Abstract: how to use `define` to simplify your codes

# Multilines

	Use \ at the end of each line except the last line

# Variable Arguments

	Use __VA_ARGS__ to replace the varaible arguments. Normally, use ##__VA_ARGS__ to let compiler to automatically remove useless comma(,) when __VA_ARGS__ is actually empty (, such as: func(var1, var2, ##__VA_ARGS);).

# Convert variable to string

	Suppose the name of `var` is `var_name` Use #var to convert `var` to "var_name"

```
#define delegate(func, handle, ...) \
	//do something \
	func = dlsym(handle, #func) \
	//do somethind else \
	return func(__VA_ARGS)
```
