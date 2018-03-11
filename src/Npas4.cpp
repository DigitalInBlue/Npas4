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
#include <Psapi.h>
#include <Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#endif

///
/// References:
/// http://blogs.microsoft.co.il/sasha/2016/01/05/windows-process-memory-usage-demystified/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366770(v=vs.85).aspx
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684877(v=vs.85).aspx
/// http://nadeausoftware.com/articles/2012/09/c_c_tip_how_get_physical_memory_size_system
/// http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use
/// https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
/// https://stackoverflow.com/questions/2513505/how-to-get-available-memory-c-g
///

constexpr int64_t Kilobytes2Bytes{1024};

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

			line[i - 3] = '\0';
			return atoi(line);
		}
	} // namespace impl
} // namespace npas4
#endif

npas4::RAMReport::operator std::string()
{
	std::stringstream ss;

	ss << "System Total:                      " << this->RamSystemTotal << std::endl;
	ss << "System Available:                  " << this->RamSystemAvailable << std::endl;
	ss << "System Used:                       " << this->RamSystemUsed << std::endl;
	ss << "System UsedByCurrentProcess:       " << this->RamSystemUsedByCurrentProcess << std::endl;
	ss << "Physical Total:                    " << this->RamPhysicalTotal << std::endl;
	ss << "Physical Available:                " << this->RamPhysicalAvailable << std::endl;
	ss << "Physical Used:                     " << this->RamPhysicalUsed << std::endl;
	ss << "Physical UsedByCurrentProcess:     " << this->RamPhysicalUsedByCurrentProcess << std::endl;
	ss << "Physical UsedByCurrentProcessPeak: " << this->RamPhysicalUsedByCurrentProcessPeak << std::endl;
	ss << "Virtual Total:                     " << this->RamVirtualTotal << std::endl;
	ss << "Virtual Available:                 " << this->RamVirtualAvailable << std::endl;
	ss << "Virtual Used:                      " << this->RamVirtualUsed << std::endl;
	ss << "Virtual UsedByCurrentProcess:      " << this->RamVirtualUsedByCurrentProcess << std::endl;

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
	r.RamPhysicalUsedByCurrentProcessPeak = this->RamPhysicalUsedByCurrentProcessPeak - x.RamPhysicalUsedByCurrentProcessPeak;
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
	return static_cast<int64_t>(memInfo.ullTotalPhys) + static_cast<int64_t>(memInfo.ullTotalVirtual);
#else
	// Prefer sysctl() over sysconf() except sysctl() HW_REALMEM and HW_PHYSMEM
	// return static_cast<int64_t>(sysconf(_SC_PHYS_PAGES)) * static_cast<int64_t>(sysconf(_SC_PAGE_SIZE));

	struct sysinfo memInfo;
	sysinfo(&memInfo);
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
	return static_cast<int64_t>(memInfo.ullAvailPhys) + static_cast<int64_t>(memInfo.ullAvailVirtual);
#else
	return npas4::GetRAMSystemTotal() - npas4::GetRAMSystemUsed();
#endif
}

int64_t npas4::GetRAMSystemUsed()
{
#ifdef WIN32
	return npas4::GetRAMSystemTotal() - npas4::GetRAMSystemAvailable();
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
	return static_cast<int64_t>(pmc.WorkingSetSize);
#elif defined(__APPLE__) && defined(__MACH__)
	mach_task_basic_info info;
	mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

	if(task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS)
	{
		return static_cast<int64_t>(info.resident_size);
	}

	return int64_t(-1);
#else
	int64_t rss{-1};
	FILE* fp = nullptr;

	if((fp = fopen("/proc/self/statm", "r")) != nullptr)
	{
		const auto fscanfResult = fscanf(fp, "%*s%ld", &rss);
		fclose(fp);

		if(fscanfResult == 1)
		{
			rss *= static_cast<int64_t>(sysconf(_SC_PAGESIZE));
		}
		else
		{
			rss = -1;
		}
	}

	return rss;
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
	return npas4::GetRAMPhysicalTotal() - npas4::GetRAMPhysicalAvailable();
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

int64_t npas4::GetRAMPhysicalUsedByCurrentProcessPeak()
{
#if defined(_WIN32)
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return static_cast<int64_t>(pmc.PeakWorkingSetSize);
#elif defined(__APPLE__) && defined(__MACH__)
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return static_cast<int64_t>(rusage.ru_maxrss);
#else
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return static_cast<int64_t>(rusage.ru_maxrss * 1024L);
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
	sysinfo(&memInfo);

	auto totalVirtualMem = memInfo.totalram;

	// Add other values in next statement to avoid int overflow on right hand side...
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
	return npas4::GetRAMVirtualTotal() - npas4::GetRAMVirtualAvailable();
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);

	auto virtualMemUsed = memInfo.totalram - memInfo.freeram;

	// Add other values in next statement to avoid int overflow on right hand side...
	virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
	virtualMemUsed *= memInfo.mem_unit;

	return virtualMemUsed;
#endif
}

int64_t npas4::GetRAMVirtualUsedByCurrentProcess()
{
#ifdef WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
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
