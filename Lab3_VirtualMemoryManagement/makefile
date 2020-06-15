mmu: Source.o PagerFIFO.o PagerRandom.o PagerClock.o PagerNRU.o Process.o PagerAging.o Pager.o PagerWorkingSet.o
	g++ -Wall Source.o PagerFIFO.o PagerRandom.o PagerClock.o PagerNRU.o Process.o PagerAging.o Pager.o PagerWorkingSet.o -o mmu
		
Source.o: Source.cpp Process.h PagerFIFO.h PagerRandom.h PagerWorkingSet.h PagerAging.h PagerNRU.h PagerClock.h
	g++ -std=c++11 -c Source.cpp 

Pager.o: Pager.cpp Pager.h
	g++ -std=c++11 -c Pager.cpp 
	
Process.o: Process.cpp Process.h
	g++ -std=c++11 -c Process.cpp 
	
PagerFIFO.o: PagerFIFO.cpp PagerFIFO.h
	g++ -std=c++11 -c PagerFIFO.cpp 

PagerRandom.o: PagerRandom.cpp PagerRandom.h
	g++ -std=c++11 -c PagerRandom.cpp 

PagerClock.o: PagerClock.cpp PagerClock.h
	g++ -std=c++11 -c PagerClock.cpp 

PagerNRU.o: PagerNRU.cpp PagerNRU.h
	g++ -std=c++11 -c PagerNRU.cpp 

PagerAging.o: PagerAging.cpp PagerAging.h
	g++ -std=c++11 -c PagerAging.cpp 

PagerWorkingSet.o: PagerWorkingSet.cpp PagerWorkingSet.h
	g++ -std=c++11 -c PagerWorkingSet.cpp 

clean:
	rm -f *.o mmu
