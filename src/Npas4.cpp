///
/// \author	John Farrier
///
/// \copyright Copyright 2014-2018 John Farrier 
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// 
/// http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

#include <npas4/Npas4.h>
#include <sstream>

#ifdef WIN32
	#include <Windows.h>
	#include <Psapi.h>
#else
	#include <sys/types.h>
	#include <sys/sysinfo.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
#endif

const int64_t Kilobytes2Bytes = 1024;

#ifdef WIN32
#else
namespace npas4
{
	namespace impl
	{
		int ParseLine(char* line)
		{
			const auto i = strlen(line);
			
			while(*line < '0' || *line > '9') 
			{
				line++;
			}

			line[i-3] = '\0';
			return atoi(line);
		};
	}
}
#endif

npas4::RAMReport::operator std::string()
{
	std::stringstream ss;
	
	ss << "System Total:                  " << this->RamSystemTotal << std::endl;
	ss << "System Available:              " << this->RamSystemAvailable << std::endl;
	ss << "System Used:                   " << this->RamSystemUsed << std::endl;
	ss << "System UsedByCurrentProcess:   " << this->RamSystemUsedByCurrentProcess << std::endl;
	ss << "Physical Total:                " << this->RamPhysicalTotal << std::endl;
	ss << "Physical Available:            " << this->RamPhysicalAvailable << std::endl;
	ss << "Physical Used:                 " << this->RamPhysicalUsed << std::endl;
	ss << "Physical UsedByCurrentProcess: " << this->RamPhysicalUsedByCurrentProcess << std::endl;
	ss << "Virtual Total:                 " << this->RamVirtualTotal << std::endl;
	ss << "Virtual Available:             " << this->RamVirtualAvailable << std::endl;
	ss << "Virtual Used:                  " << this->RamVirtualUsed << std::endl;
	ss << "Virtual UsedByCurrentProcess:  " << this->RamVirtualUsedByCurrentProcess << std::endl;

	return ss.str();
}

npas4::RAMReport npas4::RAMReport::operator-(const RAMReport& x)
{
	npas4::RAMReport r;
	r.RamSystemTotal = this->RamSystemTotal - x.RamSystemTotal;
	r.RamSystemAvailable = this->RamSystemAvailable - x.RamSystemAvailable;
	r.RamSystemUsed = this->RamSystemUsed - x.RamSystemUsed;
	r.RamSystemUsedByCurrentProcess = this->RamSystemUsedByCurrentProcess - x.RamSystemUsedByCurrentProcess;
	r.RamPhysicalTotal = this->RamPhysicalTotal - x.RamPhysicalTotal;
	r.RamPhysicalAvailable = this->RamPhysicalAvailable - x.RamPhysicalAvailable;
	r.RamPhysicalUsed = this->RamPhysicalUsed - x.RamPhysicalUsed;
	r.RamPhysicalUsedByCurrentProcess = this->RamPhysicalUsedByCurrentProcess - x.RamPhysicalUsedByCurrentProcess;
	r.RamVirtualTotal = this->RamVirtualTotal - x.RamVirtualTotal;
	r.RamVirtualAvailable = this->RamVirtualAvailable - x.RamVirtualAvailable;
	r.RamVirtualUsed = this->RamVirtualUsed - x.RamVirtualUsed;
	r.RamVirtualUsedByCurrentProcess = this->RamVirtualUsedByCurrentProcess - x.RamVirtualUsedByCurrentProcess;

	return r;
}

int64_t npas4::GetRAMSystemTotal()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullTotalPageFile);
	#else
		struct sysinfo memInfo;
		sysinfo (&memInfo);
		int64_t total = memInfo.totalram;

		// Add other values in next statement to avoid int overflow on right hand side...
		total += memInfo.totalswap;
		total += memInfo.totalhigh;
		total *= memInfo.mem_unit;
		return static_cast<int64_t>(total);
	#endif
}

int64_t npas4::GetRAMSystemAvailable()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullAvailPageFile);
	#else
		return npas4::GetRAMSystemTotal() - npas4::GetRAMSystemUsed();
	#endif
}

int64_t npas4::GetRAMSystemUsed()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
	#else
		struct sysinfo memInfo;
		sysinfo(&memInfo);
		int64_t total = memInfo.totalram - memInfo.freeram;
		total += memInfo.totalswap - memInfo.freeswap;
		total += memInfo.totalhigh - memInfo.freehigh;
		total *= memInfo.mem_unit;

		return static_cast<int64_t>(total);
	#endif
}

int64_t npas4::GetRAMSystemUsedByCurrentProcess()
{
	#ifdef WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
		return static_cast<int64_t>(pmc.PrivateUsage);
	#else
		constexpr int BufferSize{128};
		auto file = fopen("/proc/self/status", "r");
		int64_t result = 0;
		char line[BufferSize];

		while(fgets(line, BufferSize, file) != nullptr)
		{
			// total program size
			if(strncmp(line, "VmSize:", 7) == 0)
			{
				result += npas4::impl::ParseLine(line);
			}

			// locked memory size
			if(strncmp(line, "VmLck:", 7) == 0)
			{
				result += npas4::impl::ParseLine(line);
			}

			// pinned memory size
			if(strncmp(line, "VmPin:", 7) == 0)
			{
				result += npas4::impl::ParseLine(line);
			}
		}

		fclose(file);
		return static_cast<int64_t>(result) * Kilobytes2Bytes;
	#endif
}

int64_t npas4::GetRAMPhysicalTotal()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullTotalPhys);
	#else
		struct sysinfo memInfo;
		sysinfo(&memInfo);

		int64_t total = memInfo.totalram;
		total *= memInfo.mem_unit;
		return total;
	#endif
}

int64_t npas4::GetRAMPhysicalAvailable()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullAvailPhys);
	#else
		return npas4::GetRAMPhysicalTotal() - npas4::GetRAMPhysicalUsed();
	#endif
}

int64_t npas4::GetRAMPhysicalUsed()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return static_cast<int64_t>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
	#else
		struct sysinfo memInfo;
		sysinfo(&memInfo);

		int64_t total = memInfo.totalram - memInfo.freeram;

		// Multiply in next statement to avoid int overflow on right hand side...
		total *= memInfo.mem_unit;
		return total;
	#endif
}

int64_t npas4::GetRAMPhysicalUsedByCurrentProcess()
{
	#ifdef WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
		return static_cast<int64_t>(pmc.WorkingSetSize);
	#else
		constexpr int BufferSize{128};
		auto file = fopen("/proc/self/status", "r");
		int64_t result = 0;
		char line[BufferSize];

		while(fgets(line, BufferSize, file) != nullptr)
		{
			if(strncmp(line, "VmRSS:", 6) == 0)
			{
				result += npas4::impl::ParseLine(line);
			}
		}

		fclose(file);
		return static_cast<int64_t>(result) * Kilobytes2Bytes;
	#endif
}

int64_t npas4::GetRAMVirtualTotal()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullTotalPageFile;
	#else
		struct sysinfo memInfo;
		sysinfo (&memInfo);

		auto totalVirtualMem = memInfo.totalram;

		//Add other values in next statement to avoid int overflow on right hand side...
		totalVirtualMem += memInfo.totalswap;
		totalVirtualMem *= memInfo.mem_unit;

		return totalVirtualMem;
	#endif
}

int64_t npas4::GetRAMVirtualAvailable()
{
	#ifdef WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullTotalPageFile;
	#else
		return npas4::GetRAMVirtualTotal() - npas4::GetRAMVirtualUsed();
	#endif
}

int64_t npas4::GetRAMVirtualUsed()
{
	#ifdef WIN32
		return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	#else
		struct sysinfo memInfo;
		sysinfo (&memInfo);

		auto virtualMemUsed = memInfo.totalram - memInfo.freeram;
		
		//Add other values in next statement to avoid int overflow on right hand side...
		virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
		virtualMemUsed *= memInfo.mem_unit;

		return virtualMemUsed;
	#endif
}

int64_t npas4::GetRAMVirtualUsedByCurrentProcess()
{
	#ifdef WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.PrivateUsage;
	#else
		FILE* file = fopen("/proc/self/status", "r");
	    int64_t result = 0;
		constexpr int BufferSize{128};
		char line[BufferSize];

		while(fgets(line, BufferSize, file) != NULL)
	    {
			if(strncmp(line, "VmSize:", 7) == 0)
	        {
				result = npas4::impl::ParseLine(line);
				break;
	        }
	    }

	    fclose(file);
	    return result;
	#endif
}

npas4::RAMReport npas4::GetRAMReport()
{
	npas4::RAMReport r;
	r.RamSystemTotal = GetRAMSystemTotal();
	r.RamSystemAvailable = GetRAMSystemAvailable();
	r.RamSystemUsed = GetRAMSystemUsed();
	r.RamSystemUsedByCurrentProcess = GetRAMSystemUsedByCurrentProcess();
	r.RamPhysicalTotal = GetRAMPhysicalTotal();
	r.RamPhysicalAvailable = GetRAMPhysicalAvailable();
	r.RamPhysicalUsed = GetRAMPhysicalUsed();
	r.RamPhysicalUsedByCurrentProcess = GetRAMPhysicalUsedByCurrentProcess();
	r.RamVirtualTotal = GetRAMVirtualTotal();
	r.RamVirtualAvailable = GetRAMVirtualAvailable();
	r.RamVirtualUsed = GetRAMVirtualUsed();
	r.RamVirtualUsedByCurrentProcess = GetRAMVirtualUsedByCurrentProcess();
	return r;
}
