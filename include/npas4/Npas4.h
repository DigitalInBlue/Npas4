#ifndef H_NPAS4_NPAS4_H
#define H_NPAS4_NPAS4_H

///
/// \namespace npas4
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
/// Special thanks to the band "3" for providing the development soundtrack.
///
/// Reference:
/// http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
///

#ifdef NPAS4_STATIC
#define NPAS4_EXPORT
#else
#ifdef WIN32
#include <Windows.h>
#if defined NPAS4_EXPORTS
#define NPAS4_EXPORT _declspec(dllexport)
#else
#define NPAS4_EXPORT _declspec(dllimport)
#endif
#else
#define NPAS4_EXPORT
#endif
#endif

#include <cstdint>
#include <string>

namespace npas4
{
	struct RAMReport
	{
		int64_t RamSystemTotal{0};
		int64_t RamSystemAvailable{0};
		int64_t RamSystemUsed{0};
		int64_t RamSystemUsedByCurrentProcess{0};
		int64_t RamPhysicalTotal{0};
		int64_t RamPhysicalAvailable{0};
		int64_t RamPhysicalUsed{0};
		int64_t RamPhysicalUsedByCurrentProcess{0};
		int64_t RamVirtualTotal{0};
		int64_t RamVirtualAvailable{0};
		int64_t RamVirtualUsed{0};
		int64_t RamVirtualUsedByCurrentProcess{0};

		operator std::string();
		npas4::RAMReport operator-(const npas4::RAMReport& x);
	};

	// ----------------------------------------------------------------
	// Physical + Virtual Memory

	NPAS4_EXPORT int64_t GetRAMSystemTotal();
	NPAS4_EXPORT int64_t GetRAMSystemAvailable();
	NPAS4_EXPORT int64_t GetRAMSystemUsed();
	NPAS4_EXPORT int64_t GetRAMSystemUsedByCurrentProcess();

	// ----------------------------------------------------------------
	// Physical Memory

	NPAS4_EXPORT int64_t GetRAMPhysicalTotal();
	NPAS4_EXPORT int64_t GetRAMPhysicalAvailable();
	NPAS4_EXPORT int64_t GetRAMPhysicalUsed();
	NPAS4_EXPORT int64_t GetRAMPhysicalUsedByCurrentProcess();

	// ----------------------------------------------------------------
	// Virtual Memory

	NPAS4_EXPORT int64_t GetRAMVirtualTotal();
	NPAS4_EXPORT int64_t GetRAMVirtualAvailable();
	NPAS4_EXPORT int64_t GetRAMVirtualUsed();
	NPAS4_EXPORT int64_t GetRAMVirtualUsedByCurrentProcess();

	NPAS4_EXPORT npas4::RAMReport GetRAMReport();
} // namespace npas4

#endif
