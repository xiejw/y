### Check Dynamic Lib
```
# Print Dynamic libs
otool -L a.out

# Print rpath
otool -l a.out | grep PATH -A 2
```

### Paths
```
/etc/paths
/etc/paths.d/
```
