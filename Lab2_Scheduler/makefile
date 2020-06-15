sched: Source.o DESLayer.o FCFSScheduler.o LCFSScheduler.o PREPRIOScheduler.o PRIOScheduler.o Process.o RRScheduler.o Scheduler.o SRTFScheduler.o
	g++ -Wall Source.o DESLayer.o FCFSScheduler.o LCFSScheduler.o PREPRIOScheduler.o PRIOScheduler.o Process.o RRScheduler.o Scheduler.o SRTFScheduler.o -o sched
		
Source.o: Source.cpp Process.h DESLayer.h DESLayer.cpp FCFSScheduler.h LCFSScheduler.h SRTFScheduler.h RRScheduler.h PRIOScheduler.h PREPRIOScheduler.h
	g++ -std=c++11 -c Source.cpp 

DESLayer.o: DESLayer.cpp DESLayer.h
	g++ -std=c++11 -c DESLayer.cpp 

FCFSScheduler.o: FCFSScheduler.cpp FCFSScheduler.h
	g++ -std=c++11 -c FCFSScheduler.cpp 

LCFSScheduler.o: LCFSScheduler.cpp LCFSScheduler.h
	g++ -std=c++11 -c LCFSScheduler.cpp 

PREPRIOScheduler.o: PREPRIOScheduler.cpp PREPRIOScheduler.h
	g++ -std=c++11 -c PREPRIOScheduler.cpp 

PRIOScheduler.o: PRIOScheduler.cpp PRIOScheduler.h
	g++ -std=c++11 -c PRIOScheduler.cpp 

Process.o: Process.cpp Process.h
	g++ -std=c++11 -c Process.cpp 

RRScheduler.o: RRScheduler.cpp RRScheduler.h
	g++ -std=c++11 -c RRScheduler.cpp 

Scheduler.o: Scheduler.cpp Scheduler.h
	g++ -std=c++11 -c Scheduler.cpp 

SRTFScheduler.o: SRTFScheduler.cpp SRTFScheduler.h
	g++ -std=c++11 -c SRTFScheduler.cpp 

clean:
	rm -f *.o sched
