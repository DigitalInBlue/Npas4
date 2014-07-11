// www.helleboreconsulting.com

///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
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
#include <gtest/gtest.h>

#include <thread>

TEST(npas4, Report)
{
	EXPECT_NE(0, npas4::GetTotalSystemRAM());
	EXPECT_NE(0, npas4::GetTotalSystemRAMAvailable());
	EXPECT_NE(0, npas4::GetTotalSystemRAMUsed());
	EXPECT_NE(0, npas4::GetTotalSystemRAMUsedByCurrentProcess());
	EXPECT_NE(0, npas4::GetTotalPhysicalRAM());
	EXPECT_NE(0, npas4::GetTotalPhysicalRAMAvailable());
	EXPECT_NE(0, npas4::GetTotalPhysicalRAMUsed());
	EXPECT_NE(0, npas4::GetTotalPhysicalRAMUsedByCurrentProcess());

	// Sanity check that my system reports about 32GB of physical RAM.
	const size_t ThirtyTwoGBOfPhysicalRam = 34340065280;
	EXPECT_EQ(ThirtyTwoGBOfPhysicalRam, npas4::GetTotalPhysicalRAM());
}

TEST(npas4, Alloc)
{
	auto start0 = npas4::GetTotalSystemRAM();
	auto start1 = npas4::GetTotalSystemRAMAvailable();
	auto start2 = npas4::GetTotalSystemRAMUsed();
	auto start3 = npas4::GetTotalSystemRAMUsedByCurrentProcess();
	auto start4 = npas4::GetTotalPhysicalRAM();
	auto start5 = npas4::GetTotalPhysicalRAMAvailable();
	auto start6 = npas4::GetTotalPhysicalRAMUsed();
	auto start7 = npas4::GetTotalPhysicalRAMUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const size_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(start0, npas4::GetTotalSystemRAM());
		EXPECT_NE(start1, npas4::GetTotalSystemRAMAvailable());
		EXPECT_NE(start2, npas4::GetTotalSystemRAMUsed());
		EXPECT_LT(start3 + allocAmmount, npas4::GetTotalSystemRAMUsedByCurrentProcess());
		EXPECT_EQ(start4, npas4::GetTotalPhysicalRAM());
		EXPECT_NE(start5, npas4::GetTotalPhysicalRAMAvailable());
		EXPECT_NE(start6, npas4::GetTotalPhysicalRAMUsed());
		EXPECT_LT(start7 + allocAmmount, npas4::GetTotalPhysicalRAMUsedByCurrentProcess());

		// Constant found experimentally.
		const size_t allocationConstant = 4096;

		// Assume we are not swapping out to disk
		auto memoryDelta = npas4::GetTotalSystemRAMUsedByCurrentProcess() - start3;
		EXPECT_EQ(npas4::GetTotalPhysicalRAMUsedByCurrentProcess() - start7, memoryDelta + allocationConstant);
	
		delete [] megabyte;
	}
}
