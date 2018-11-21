
pts2xodr: pts2xodr.o odRoad.o
	g++ -o $@ $^ -l xml2 -l m

odRoad.o: odRoad.cpp odRoad.h
	g++ -Wall -Wextra -c -I /usr/include/libxml2 $<

 
pts2xodr.o: pts2xodr.cpp odRoad.h
	g++ -Wall -Wextra -c -I /usr/include/libxml2 $<
