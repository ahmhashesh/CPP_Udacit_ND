#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  int NumCpus;
  long Uptime;
  long user ;
  long nice ;
  long system ;
  long iowait ;
  long irq ;
  long softirq;
  long Idle ;
};

#endif