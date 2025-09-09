#include <iostream>
#include <fstream>
#include <vector>

void buildComp(std::ofstream &myfile, std::vector<std::string> ports, int mem_size);
void buildControl(std::ofstream &myfile, std::vector<std::vector<std::string>> io_port, std::vector<std::vector<std::string>> mem_port, 
std::string *parameter, std::string *input, std::string *logic, int mem_index);
void buildRam(std::ofstream &myfile, int mem_num, std::string *parameter, std::string *input, std::string *logic, int mem_size);
void buildHybridHeader(std::ofstream &header, std::vector<std::string> memPort, std::vector<std::string> ioPort);
bool IsPowerOfTwo(int x);