// www.helleboreconsulting.com

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
#include <gtest/gtest.h>

#include <thread>

constexpr double Gigabyte{1073741824.0};

// Constant found experimentally.
#ifdef WIN32
	constexpr int64_t AllocationConstant = 4096;
#else
	constexpr int64_t AllocationConstant = 0;
#endif

TEST(npas4, Report)
{
	EXPECT_NE(int64_t(0), npas4::GetRAMSystemTotal());
	EXPECT_NE(int64_t(0), npas4::GetRAMSystemAvailable());
	EXPECT_NE(int64_t(0), npas4::GetRAMSystemUsed());
	EXPECT_NE(int64_t(0), npas4::GetRAMSystemUsedByCurrentProcess());
	EXPECT_NE(int64_t(0), npas4::GetRAMPhysicalTotal());
	EXPECT_NE(int64_t(0), npas4::GetRAMPhysicalAvailable());
	EXPECT_NE(int64_t(0), npas4::GetRAMPhysicalUsed());
	EXPECT_NE(int64_t(0), npas4::GetRAMPhysicalUsedByCurrentProcess());
}

// TEST(npas4, SystemHas32GB)
// {
// 	// Sanity check that my system reports about 32GB of physical RAM.
// 	const int64_t ThirtyTwoGBOfPhysicalRam = 34340065280;
// 	EXPECT_EQ(ThirtyTwoGBOfPhysicalRam, npas4::GetRAMPhysicalTotal()) << "The system reports " << npas4::GetRAMPhysicalTotal()/Gigabyte << " GB total physical RAM.";
// }

TEST(npas4, AllocSystem)
{
	const auto startTotal = npas4::GetRAMSystemTotal();
	EXPECT_GT(npas4::GetRAMSystemTotal(), int64_t(1024));

	// const auto startAvailable = npas4::GetRAMSystemAvailable();
	EXPECT_GT(npas4::GetRAMSystemTotal(), npas4::GetRAMSystemAvailable());

	// const auto startUsed = npas4::GetRAMSystemUsed();
	EXPECT_GT(npas4::GetRAMSystemTotal(), npas4::GetRAMSystemUsed());
	EXPECT_GT(npas4::GetRAMSystemAvailable(), npas4::GetRAMSystemUsed());

	const auto startUsedByCurrentProcess = npas4::GetRAMSystemUsedByCurrentProcess();
	EXPECT_GT(npas4::GetRAMSystemTotal(), npas4::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(npas4::GetRAMSystemAvailable(), npas4::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(npas4::GetRAMSystemUsed(), npas4::GetRAMSystemUsedByCurrentProcess());

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, npas4::GetRAMSystemTotal());
		EXPECT_NE(startUsedByCurrentProcess, npas4::GetRAMSystemUsedByCurrentProcess());

		EXPECT_LT(startUsedByCurrentProcess + allocAmmount, npas4::GetRAMSystemUsedByCurrentProcess()) << "We did an allocation, but are using less RAM. Start: " 
			<< startUsedByCurrentProcess << ", Alloc: " << allocAmmount;

		

		// Assume we are not swapping out to disk
		ASSERT_GT(npas4::GetRAMSystemUsedByCurrentProcess(), startUsedByCurrentProcess);

		const auto memoryDelta = npas4::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess;
		EXPECT_EQ(npas4::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess, memoryDelta + AllocationConstant) << "Memory Delta: " << memoryDelta;
	
		delete [] megabyte;
	}
}

TEST(npas4, AllocPhysical)
{
	const auto startTotal = npas4::GetRAMPhysicalTotal();
	// const auto startAvailable = npas4::GetRAMPhysicalAvailable();
	// const auto startUsed = npas4::GetRAMPhysicalUsed();
	const auto startUsedByCurrentProcess = npas4::GetRAMPhysicalUsedByCurrentProcess();

	volatile uint8_t* megabyte = nullptr;

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672;
		megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, npas4::GetRAMPhysicalTotal());
		// EXPECT_NE(startUsedByCurrentProcess, npas4::GetRAMPhysicalUsedByCurrentProcess());
		// EXPECT_LT(startUsedByCurrentProcess + allocAmmount, npas4::GetRAMPhysicalUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		// Assume we are not swapping out to disk
		ASSERT_GT(npas4::GetRAMSystemUsedByCurrentProcess(), startUsedByCurrentProcess);

		// const auto memoryDelta = npas4::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess;
		// EXPECT_EQ(npas4::GetRAMPhysicalUsedByCurrentProcess() - startUsedByCurrentProcess, memoryDelta + AllocationConstant);
	
		delete [] megabyte;
	}
	else
	{
		const int64_t allocAmmount = 2052672;
		megabyte = new uint8_t[allocAmmount];
        delete megabyte;
	}
}

TEST(npas4, AllocVirtual)
{
	const auto startTotal = npas4::GetRAMVirtualTotal();
	// const auto startAvailable = npas4::GetRAMVirtualAvailable();
	// const auto startUsed = npas4::GetRAMVirtualUsed();
	// const auto startUsedByCurrentProcess = npas4::GetRAMVirtualUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, npas4::GetRAMVirtualTotal());
		// EXPECT_NE(startAvailable, npas4::GetRAMVirtualAvailable());
		// EXPECT_NE(startUsed, npas4::GetRAMVirtualUsed());
		// EXPECT_LT(startUsedByCurrentProcess + allocAmmount, npas4::GetRAMVirtualUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		delete [] megabyte;
	}
}

TEST(npas4, AllocAll)
{
	const auto start0 = npas4::GetRAMSystemTotal();
	//const auto start1 = npas4::GetRAMSystemAvailable();
	//const auto start2 = npas4::GetRAMSystemUsed();
	const auto start3 = npas4::GetRAMSystemUsedByCurrentProcess();

	const auto start4 = npas4::GetRAMPhysicalTotal();
	//const auto start5 = npas4::GetRAMPhysicalAvailable();
	//const auto start6 = npas4::GetRAMPhysicalUsed();
	const auto start7 = npas4::GetRAMPhysicalUsedByCurrentProcess();

	const auto start8 = npas4::GetRAMVirtualTotal();
	//const auto start9 = npas4::GetRAMVirtualAvailable();
	//const auto start10 = npas4::GetRAMVirtualUsed();
	// const auto start11 = npas4::GetRAMVirtualUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(start0, npas4::GetRAMSystemTotal());
		// EXPECT_NE(start1, npas4::GetRAMSystemAvailable());
		// EXPECT_NE(start2, npas4::GetRAMSystemUsed());
		// EXPECT_LT(start3 + allocAmmount, npas4::GetRAMSystemUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		EXPECT_EQ(start4, npas4::GetRAMPhysicalTotal());
		//EXPECT_NE(start5, npas4::GetRAMPhysicalAvailable());
		//EXPECT_NE(start6, npas4::GetRAMPhysicalUsed());
		//EXPECT_LT(start7 + allocAmmount, npas4::GetRAMPhysicalUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		EXPECT_EQ(start8, npas4::GetRAMVirtualTotal());
		// EXPECT_NE(start9, npas4::GetRAMVirtualAvailable());
		// EXPECT_NE(start10, npas4::GetRAMVirtualUsed());
		// EXPECT_LT(start11 + allocAmmount, npas4::GetRAMVirtualUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		// Assume we are not swapping out to disk
		auto memoryDelta = npas4::GetRAMSystemUsedByCurrentProcess() - start3;
		EXPECT_EQ(npas4::GetRAMPhysicalUsedByCurrentProcess() - start7, memoryDelta + AllocationConstant);
	
		delete [] megabyte;
	}
}
