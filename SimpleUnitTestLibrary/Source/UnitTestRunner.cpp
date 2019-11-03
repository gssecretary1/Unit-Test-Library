#include <UnitTestRunner.h>


/// Ctors \\\

UnitTestRunner::UnitTestRunner(_Inout_ std::wstring&& testName) noexcept :
    m_TestSetData(std::move(testName))
{
    testName.clear();
}

/// Operator Overloads \\\

UnitTestRunner& UnitTestRunner::operator=(_Inout_ UnitTestRunner&& src) noexcept
{
    if (this != &src)
    {
        m_UnitTests = std::move(src.m_UnitTests);

        src.ClearUnitTests();
    }

    return *this;
}

// Getters

const std::list<UnitTest>& UnitTestRunner::GetUnitTests() const noexcept
{
    return m_UnitTests;
}

UnitTestLogger& UnitTestRunner::GetLogger() const noexcept
{
    return m_Logger;
}

TestSetData& UnitTestRunner::GetTestSetData() noexcept
{
    return m_TestSetData;
}

const TestSetData& UnitTestRunner::GetTestSetData() const noexcept
{
    return m_TestSetData;
}

// Public Methods

bool UnitTestRunner::AddUnitTest(_Inout_ UnitTest&& test)
{
    try
    {
        m_UnitTests.push_back(std::move(test));
        test.Clear();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool UnitTestRunner::AddUnitTest(_Inout_ std::function<UnitTestResult(void)>&& test)
{
    try
    {
        m_UnitTests.emplace_back(std::move(test));
        test = nullptr;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool UnitTestRunner::AddUnitTests(_Inout_ std::list<UnitTest>&& tests)
{
    try
    {
        m_UnitTests.splice(m_UnitTests.cend(), std::move(tests));
        tests.clear();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool UnitTestRunner::AddUnitTests(_Inout_ std::list<std::function<UnitTestResult(void)>>&& tests)
{
    try
    {
        for (auto& f : tests)
        {
            m_UnitTests.emplace_back(std::move(f));
        }

        tests.clear();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void UnitTestRunner::ClearUnitTests() noexcept
{
    m_UnitTests.clear();
}

bool UnitTestRunner::RunUnitTests()
{
    using Clock = std::chrono::high_resolution_clock;
    using DurationMs = std::chrono::duration<int64_t, std::milli>;
    using DurationMicroseconds = std::chrono::duration<int64_t, std::micro>;
    using TimePoint = std::chrono::time_point<Clock>;

    bool ret = true;

    m_TestSetData.ResetCounters();
    m_TestSetData.SetTotalTestCount(static_cast<uint32_t>(m_UnitTests.size()));

    m_Logger.LogTestSetHeader(m_TestSetData);

    for (UnitTest& test : m_UnitTests)
    {
        TimePoint t0, t1;
        DurationMs deltaMs;
        DurationMicroseconds deltaMicroseconds;
        ResultType r = ResultType::NotRun;
        std::string eStr = "";

        t0 = Clock::now();
        try
        {
            test.RunTest();
            t1 = Clock::now();
            r = test.GetUnitTestResult().GetResult();
        }
        catch (const std::exception& e)
        {
            t1 = Clock::now();
            r = ResultType::UnhandledException;
            eStr = e.what();
        }
        catch (...)
        {
            t1 = Clock::now();
            r = ResultType::UnhandledException;
            eStr = "<Unknown Unhandled Exception>";
        }

        m_TestSetData.IncrementResultCounter(r);

        deltaMs = std::chrono::duration_cast<DurationMs, int64_t>(t1 - t0);
        deltaMicroseconds = std::chrono::duration_cast<DurationMicroseconds, int64_t>(t1 - t0);
        m_TestSetData.AddToRunDurationMs(deltaMs);
        test.GetUnitTestResult().SetTestDurationMicroseconds(deltaMicroseconds);

        if (r != ResultType::Success && r != ResultType::NotRun)
        {
            ret = false;
        }

        if (r != ResultType::UnhandledException)
        {
            m_Logger.LogUnitTestResult(test.GetUnitTestResult());
        }
        else
        {
            const std::string& funcName = test.GetUnitTestResult().GetFunctionName();
            const std::string& fileName = test.GetUnitTestResult().GetFileName();
            const uint32_t& line = test.GetUnitTestResult().GetLineNumber();

            UnitTestResult tmp(r, funcName.c_str(), funcName.size(), fileName.c_str(), fileName.size(), line, eStr);
            tmp.SetTestDurationMicroseconds(deltaMicroseconds);
            m_Logger.LogUnitTestResult(tmp);
        }

    }

    m_Logger.LogTestSetSummary(m_TestSetData);

    return ret;
}