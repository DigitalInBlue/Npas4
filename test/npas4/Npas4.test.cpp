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
#include <iomanip>

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
	EXPECT_GT(npas4::GetRAMSystemTotal(), npas4::GetRAMSystemAvailable());
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
	const auto startUsedByCurrentProcess = npas4::GetRAMPhysicalUsedByCurrentProcess();

	volatile uint8_t* megabyte = nullptr;

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672 * rand()%128;
		megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, npas4::GetRAMPhysicalTotal());
		EXPECT_LT(startUsedByCurrentProcess + allocAmmount, npas4::GetRAMSystemUsedByCurrentProcess()) << "We did an allocation, but are using less RAM.";

		// Assume we are not swapping out to disk
		ASSERT_GT(npas4::GetRAMSystemUsedByCurrentProcess(), startUsedByCurrentProcess);

		const auto memoryDelta = npas4::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess;
		EXPECT_EQ(npas4::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess, memoryDelta + AllocationConstant);
	
		delete [] megabyte;
	}
	else
	{
		const int64_t allocAmmount = 2052672;
		megabyte = new uint8_t[allocAmmount];
        delete megabyte;
	}
}

TEST(npas4, AllocAll)
{
	const auto start3 = npas4::GetRAMSystemUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		// Assume we are not swapping out to disk
		auto memoryDelta = npas4::GetRAMSystemUsedByCurrentProcess() - start3;
		EXPECT_NEAR(allocAmmount, memoryDelta, 4096);
	
		delete [] megabyte;
	}
}

TEST(npas4, ForceAllocateVirtual)
{
	const auto ramAvailableTotal = npas4::GetRAMSystemAvailable();
	EXPECT_GT(ramAvailableTotal, 0);

	const auto ramSystemUsed = npas4::GetRAMSystemUsed();
	EXPECT_GT(ramSystemUsed, 0);

	const auto ramUsedByCurrentProcess = npas4::GetRAMSystemUsedByCurrentProcess();
	EXPECT_GT(ramUsedByCurrentProcess, 0);

	const auto ramAvailablePhysical = npas4::GetRAMPhysicalAvailable();
	EXPECT_GT(ramAvailablePhysical, 0);

	const auto ramAvailableVirtual = npas4::GetRAMVirtualAvailable();
	EXPECT_GT(ramAvailableVirtual, 0);

	const auto allocAmmountPhysical = ramAvailablePhysical;
	const auto allocAmmountVirtual = static_cast<decltype(allocAmmountPhysical)>(static_cast<double>(ramAvailableVirtual)*0.01);
	const auto allocAmmount = allocAmmountPhysical + allocAmmountVirtual;

	EXPECT_GT(ramAvailableTotal, allocAmmount);
	EXPECT_GT(allocAmmount, ramAvailablePhysical);

	if(std::this_thread::get_id() == std::this_thread::get_id())
	{
		std::vector<uint8_t> buffer(allocAmmount);
		ASSERT_NO_THROW(buffer.reserve(allocAmmount));

		// We should be using more RAM across the board now.
		EXPECT_GT(npas4::GetRAMSystemUsed(), ramSystemUsed);
		EXPECT_GT(npas4::GetRAMSystemUsedByCurrentProcess(), ramUsedByCurrentProcess);

		EXPECT_LT(npas4::GetRAMSystemAvailable(), ramAvailableTotal);
		EXPECT_LT(npas4::GetRAMPhysicalAvailable(), ramAvailablePhysical);
		EXPECT_LT(npas4::GetRAMVirtualAvailable(), ramAvailableVirtual);

		EXPECT_GT(npas4::GetRAMSystemUsedByCurrentProcess(), allocAmmount);
		EXPECT_GT(npas4::GetRAMPhysicalUsedByCurrentProcess(), allocAmmountPhysical);
		EXPECT_GT(npas4::GetRAMVirtualUsedByCurrentProcess(), allocAmmountVirtual) << std::setprecision(4)
			<< "Alloc Ammount: " << allocAmmountVirtual/Gigabyte << " GB, Used: " 
			<< npas4::GetRAMVirtualUsedByCurrentProcess()/Gigabyte << " GB.";
	}
}
