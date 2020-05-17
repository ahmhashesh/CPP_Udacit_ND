#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;


int Process::Pid() { return this->Processid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return LinuxParser::CpuUtilization(this->Processid); }

string Process::Command() { return LinuxParser::Command(this->Processid); }

string Process::Ram() { 
  
  int x;
  std::stringstream temp(LinuxParser::Ram(this->Processid));
   temp>>x;
  float y = (float)(x)/1000;
  std::stringstream ss;
  ss << y;
  return ss.str();
}

string Process::User() { return this->UserName; }

long int Process::UpTime() { return LinuxParser::UpTime(this->Processid);  }

bool Process::operator<(Process const& a) const 
{ 
  
  if(this->Processid < a.Processid)
  {
  	return true; 
  }else 
  {
  	return false;
  }
  
}
Process::Process(int processId, std::string Uid, std::string UserName , std::string command)
{
  this->UserId = Uid;
  this->UserName = UserName;
  this->Processid = processId;
  this->command = command;
}