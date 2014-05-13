// Sylvain Lefebvre - 2012-03-02

#pragma once

#include <iostream>

#include "CL/cl.h"
#include "CL/cl.hpp"

// ----------------------------------------------------------

extern cl::Context             *clu_Context;
extern std::vector<cl::Device>  clu_Devices;
extern cl::CommandQueue        *clu_Queue;

// ----------------------------------------------------------

void          cluInit(cl_device_type devtype = CL_DEVICE_TYPE_GPU);
cl::Program  *cluLoadProgram(const char *);
cl::Program  *cluLoadProgramFromString(const char *);
cl::Kernel   *cluLoadKernel(cl::Program *,const char *); 
void          cluCheckError(cl_int, const char *);
double        cluDisplayEventMilliseconds(const char *msg,const cl::Event& ev);
double        cluEventMilliseconds(const cl::Event& ev);
std::string   cluLoadFileIntoString(const char *file);
long long     cluCPUMilliseconds();

// ----------------------------------------------------------

