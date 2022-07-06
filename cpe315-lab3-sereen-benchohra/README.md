# cpe315-matmul
## Sereen Benchohra, Fernando Valdivia

### To build:
```shell
make clean; make
```
### To run:
```shell
./mm > p.out
```
### To test:
```shell
diff p.out outputs/P.N.out
```
Note that N above is one of:
* 16
* 64
* 256
* 1024

### To modify the size:
1. edit the matmul.h file
2. edit the line with: 
```C
#define MATRIX_SIZE
```
3. Save the file
4. Rebuild
