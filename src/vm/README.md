# Original
```
e1e2
    codes for e1
    codes for e2
====================
e1|e2
    split L1, L2
    L1:
        codes for e1
        jmp L3
    L2:
        codes for e2
    L3:
====================
e?
    split L1, L2
    L1:
        codes for e
    L2:
====================
e*
    L1:
        split L2, L3
    L2:
        codes for e
        jmp L1
    L3:
====================
e+
    L1:
        codes for e
        split L1, L2
    L2

====================
e{x,y}

x * (codes for e)
y * e?

```


# References
https://swtch.com/~rsc/regexp/regexp2.html
