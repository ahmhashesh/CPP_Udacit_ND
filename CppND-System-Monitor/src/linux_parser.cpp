#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os,version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{	
  int MemTotal , MemFree;
  float MemUtil=0.0;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          std::replace(value.begin(), value.end(), 'k', ' ');
          std::replace(value.begin(), value.end(), 'B', ' ');
          std::stringstream temp(value); 
          temp >> MemTotal ;
          break;
        }
        if (key == "MemFree") {
          std::replace(value.begin(), value.end(), 'k', ' ');
          std::replace(value.begin(), value.end(), 'B', ' ');
          std::stringstream temp(value); 
          temp >> MemFree ;
          break;
        }
      }
    }
  }
  MemUtil = ((float)(MemTotal - MemFree)/MemTotal);
  return MemUtil; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  float UpTime,IdleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> UpTime >> IdleTime;
  }
  return (long)UpTime;
}
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }


string LinuxParser::readLineToken(string line, int position) {
  // use iterators to scan the line and return the
  // requested token at position
  std::istringstream iss(line);
  vector<string> results(
     std::istream_iterator<string>{iss},
     std::istream_iterator<string>()
   );
  return results[position];
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string p = std::to_string(pid);
  std::ifstream stream(kProcDirectory + p + kStatFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      string jiffies = readLineToken(line, 21);
      return std::stoi(jiffies);
    }
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
vector<string> cpu_stat = LinuxParser::CpuUtilization();
  float non_idle = std::stof(cpu_stat[0]) + std::stof(cpu_stat[1]) +
    std::stof(cpu_stat[2]);
  return non_idle;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  float idle = std::stof(cpu_stat[3]) + std::stof(cpu_stat[4]);
  return idle; 
}



int LinuxParser::NUMOFCPUs()
{
  int CPUNum;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kCpuinfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), ' ', '_');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "cpu_cores") {
          std::replace(value.begin(), value.end(), '_', ' ');
          std::stringstream temp(value); 
          temp >> CPUNum ;
          return CPUNum;
        }
      }
    }
  }
  return CPUNum;
}
// TODO: Read and return CPU utilization
/*
	The meanings of the columns are as follows, from left to right:

    user: normal processes executing in user mode
    nice: niced processes executing in user mode
    system: processes executing in kernel mode
    idle: twiddling thumbs
    iowait: waiting for I/O to complete
    irq: servicing interrupts
    softirq: servicing softirqs

*/
vector<string> LinuxParser::CpuUtilization() 
{
  string user , nice,system,idle,iowait,irq,softirq;
  vector<string> CpuUtil;
  string line;
  string key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq) {
        if (key == "cpu") {
          CpuUtil.push_back(user);
          CpuUtil.push_back(nice);
          CpuUtil.push_back(system);
          CpuUtil.push_back(idle);
          CpuUtil.push_back(iowait);
          CpuUtil.push_back(irq);
          CpuUtil.push_back(softirq);
          return CpuUtil;
        }
      }
    }
  }
  return CpuUtil; 
}
// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  int TotalProcessesNum = 0;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          std::stringstream temp(value); 
          temp >> TotalProcessesNum ;
          return TotalProcessesNum;
        }
      }
    }
  }
  return TotalProcessesNum; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
  { 
  int procs_running = 0;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          std::stringstream temp(value); 
          temp >> procs_running ;
          return procs_running;
        }
      }
    }
  }
  return procs_running; 
}
// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid)
{ 
  string command;
  string line;
  std::ifstream stream(kProcDirectory +"/"+to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      	std::replace(line.begin(), line.end(), '\0', ' ');
    	command +=line;
    }
      return command;
  }
  return command; 
  
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string Ram;
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory +"/"+to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'k', ' ');
      std::replace(line.begin(), line.end(), 'B', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          std::stringstream temp(value); 
          temp >> Ram ;
          return Ram;
    		}
   		}
   	}
  }
  return Ram;  
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{
  string Uid;
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory +"/"+to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          std::stringstream temp(value); 
          temp >> Uid ;
          return Uid;
    		}
   		}
   	}
  }
  return Uid; 
}
float LinuxParser::CpuUtilization(int pid) {
  string p = std::to_string(pid);
  float total_time,idle_time, CPU_Util;
  std::ifstream stream(kProcDirectory + p + kStatFilename);
if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
    	total_time = (std::stoi(LinuxParser::readLineToken(line, 13))
                        + std::stoi(LinuxParser::readLineToken(line, 14))
                        + std::stoi(LinuxParser::readLineToken(line, 15))
                        + std::stoi(LinuxParser::readLineToken(line, 16))) ;
      idle_time = (std::stoi(LinuxParser::readLineToken(line, 16)) 
        + std::stoi(LinuxParser::readLineToken(line, 15)));
    }}
  if(total_time !=0)
  {
  	CPU_Util = (total_time - idle_time)/total_time;
  }
  
  return CPU_Util;
}
// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid)
{
  string UName,x;
  string Uid ;
  string line;
  string key,value;
  Uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (value == Uid) {
          std::stringstream temp(key); 
          temp >> UName ;
          return UName;
    		}
   		}
   	}
  }
  return UName; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{
  return LinuxParser::UpTime() - (LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK));
}