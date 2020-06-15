iosche: Source.o IORequest.o Scheduler.o SchedulerCLOOK.o SchedulerFIFO.o SchedulerFLOOK.o SchedulerLOOK.o SchedulerSSTF.o
	g++ -Wall Source.o IORequest.o Scheduler.o SchedulerCLOOK.o SchedulerFIFO.o SchedulerFLOOK.o SchedulerLOOK.o SchedulerSSTF.o -o iosche
		
Source.o: Source.cpp SchedulerFLOOK.h IORequest.h Scheduler.h SchedulerLOOK.h SchedulerFIFO.h SchedulerCLOOK.h
	g++ -std=c++11 -c Source.cpp 

SchedulerSSTF.o: SchedulerSSTF.cpp SchedulerSSTF.h
	g++ -std=c++11 -c SchedulerSSTF.cpp 
	
SchedulerFLOOK.o: SchedulerFLOOK.cpp SchedulerFLOOK.h
	g++ -std=c++11 -c SchedulerFLOOK.cpp 
	
IORequest.o: IORequest.cpp IORequest.h
	g++ -std=c++11 -c IORequest.cpp 

Scheduler.o: Scheduler.cpp Scheduler.h
	g++ -std=c++11 -c Scheduler.cpp 

SchedulerCLOOK.o: SchedulerCLOOK.cpp SchedulerCLOOK.h
	g++ -std=c++11 -c SchedulerCLOOK.cpp 

SchedulerFIFO.o: SchedulerFIFO.cpp SchedulerFIFO.h
	g++ -std=c++11 -c SchedulerFIFO.cpp 

SchedulerLOOK.o: SchedulerLOOK.cpp SchedulerLOOK.h
	g++ -std=c++11 -c SchedulerLOOK.cpp 

clean:
	rm -f *.o iosche
