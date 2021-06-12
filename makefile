ECC: ECC.o BigNumber.o
	g++ ECC.o BigNumber.o -o ECC
	./ECC
ECC.o: ECC.cpp BigNumber.h
	g++ -c ECC.cpp -o ECC.o
BigNumber.o: BigNumber.cpp BigNumber.h
	g++ -c BigNumber.cpp -o BigNumber.o
clean:
	rm *.o ECC
run:
	./ECC