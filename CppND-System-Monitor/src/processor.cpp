#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
  float CpuUtil;
  long Total=0;
  //std::stringstream temp;
  int tempint=0;
  std::vector <std::string> CPU_Data {LinuxParser::CpuUtilization()};
  //CPU_Data = LinuxParser::CpuUtilization() ;
  for (std::string strtemp: CPU_Data)
  {
  	std::stringstream(strtemp)>> tempint;
    Total += tempint;
  }
  std::stringstream(CPU_Data[0])>> this->Uptime;
  std::stringstream(CPU_Data[1])>> this->user;
  std::stringstream(CPU_Data[2])>> this->nice;
  std::stringstream(CPU_Data[4])>> this->system;
  std::stringstream(CPU_Data[5])>> this->iowait;
  std::stringstream(CPU_Data[3])>> this->Idle;
  this->Uptime = LinuxParser::UpTime();
  this->NumCpus = LinuxParser::NUMOFCPUs();
  
  CpuUtil = (float)((Total-Idle)/Total);
  return CpuUtil;
}