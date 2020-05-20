Person : main.o Person.o sp.o
	g++ -o $@ $^
%o : %.cpp
	g++ -o $@ -c $<
clean :
	rm -rf *.o Person