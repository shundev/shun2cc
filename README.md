# shun2cc

shun2cc shows how c compiler works with GUI debugger.

Works on Mac. (Works on Linux with a few changes as well.)


## Install

```
git clone git@github.com:shunsukehondo/shun2cc.git
cd shun2cc
make test
```

## Debug on GUI

Trace execution while running with VSCode + LLDB 

1. Install VSCode
2. Install C/C++ extension
3. Open shun2cc folder with VSCode
4. Open a file & Set breakpoints
5. Launch debugger (F5)

If you want to see the whole compile process, set a breakpoint on the first line inside `main` function.
