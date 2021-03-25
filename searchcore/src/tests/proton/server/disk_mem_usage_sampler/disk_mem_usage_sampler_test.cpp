// Copyright Verizon Media. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include <vespa/searchcore/proton/common/hw_info.h>
#include <vespa/searchcore/proton/common/i_transient_resource_usage_provider.h>
#include <vespa/searchcore/proton/server/disk_mem_usage_sampler.h>
#include <vespa/vespalib/gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace proton;
using namespace std::chrono_literals;

constexpr uint64_t disk_size_bytes = 200000;
constexpr uint64_t memory_size_bytes = 100000;

HwInfo
make_hw_info()
{
    return HwInfo(HwInfo::Disk(disk_size_bytes, false, true),
                  HwInfo::Memory(memory_size_bytes),
                  HwInfo::Cpu(1));
}

class MyProvider : public ITransientResourceUsageProvider {
private:
    size_t _memory_usage;
    size_t _disk_usage;

public:
    MyProvider(size_t memory_usage, size_t disk_usage) noexcept
        : _memory_usage(memory_usage),
          _disk_usage(disk_usage)
    {}
    size_t get_transient_memory_usage() const override { return _memory_usage; }
    size_t get_transient_disk_usage() const override { return _disk_usage; }
};

struct DiskMemUsageSamplerTest : public ::testing::Test {
    DiskMemUsageSampler sampler;
    DiskMemUsageSamplerTest():
        sampler(".",
                DiskMemUsageSampler::Config(0.8, 0.8,
                                            50ms, make_hw_info()))
    {
        sampler.add_transient_usage_provider(std::make_shared<MyProvider>(99, 200));
        sampler.add_transient_usage_provider(std::make_shared<MyProvider>(100, 199));
    }
    const DiskMemUsageFilter& filter() const { return sampler.writeFilter(); }
};

TEST_F(DiskMemUsageSamplerTest, resource_usage_is_sampled)
{
    std::this_thread::sleep_for(100ms);
    EXPECT_GT(filter().getMemoryStats().getAnonymousRss(), 0);
    EXPECT_GT(filter().getDiskUsedSize(), 0);
    EXPECT_EQ(100, filter().get_transient_memory_usage());
    EXPECT_EQ(100.0 / memory_size_bytes, filter().get_relative_transient_memory_usage());
    EXPECT_EQ(200, filter().get_transient_disk_usage());
    EXPECT_EQ(200.0 / disk_size_bytes, filter().get_relative_transient_disk_usage());
}

GTEST_MAIN_RUN_ALL_TESTS()

