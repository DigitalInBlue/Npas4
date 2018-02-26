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

namespace npas4
{
	///
	/// Swap file + Physical RAM
	///
	NPAS4_EXPORT uint64_t GetTotalSystemRAM();	

	///
	/// Swap file + Physical RAM
	///
	NPAS4_EXPORT uint64_t GetTotalSystemRAMAvailable();

	///
	/// Swap file + Physical RAM
	///
	NPAS4_EXPORT uint64_t GetTotalSystemRAMUsed();

	NPAS4_EXPORT uint64_t GetTotalSystemRAMUsedByCurrentProcess();

	NPAS4_EXPORT uint64_t GetTotalPhysicalRAM();

	NPAS4_EXPORT uint64_t GetTotalPhysicalRAMAvailable();

	NPAS4_EXPORT uint64_t GetTotalPhysicalRAMUsed();

	NPAS4_EXPORT uint64_t GetTotalPhysicalRAMUsedByCurrentProcess();
}

#endif
