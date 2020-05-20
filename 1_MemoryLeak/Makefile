Person : main.o Person.o
	g++ -o $@ $^
%o : %.cpp
	g++ -o $@ -c $<
clean :
	rm -rf *.o Person