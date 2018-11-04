#include <UnitTest.h>


/// Method Definitons \\\

/// Ctors \\\
    
UnitTest::UnitTest(std::function<UnitTestResult(void)>&& func) noexcept :
    mTestFunc(std::move(func))
{ }

UnitTest::UnitTest(UnitTest&& src) noexcept :
    mTestFunc(std::move(src.mTestFunc)),
    mTestResult(std::move(src.mTestResult))
{ }

// Operator Overloads

UnitTest& UnitTest::operator=(UnitTest&& src) noexcept
{
    mTestFunc = std::move(src.mTestFunc);
    mTestResult = std::move(src.mTestResult);

    return *this;
}

// Getters

const std::function<UnitTestResult(void)>& UnitTest::GetUnitTestFunction( ) const noexcept
{
    return mTestFunc;
}

const UnitTestResult& UnitTest::GetUnitTestResult( ) const noexcept
{
    return mTestResult;
}

// Setters

void UnitTest::SetUnitTestFunction(std::function<UnitTestResult(void)>&& func) noexcept
{
    mTestFunc = std::move(func);
    mTestResult = std::move(UnitTestResult( ));
}

// Public Methods

const UnitTestResult& UnitTest::RunTest( )
{
    if ( !mTestFunc )
    {
        throw std::runtime_error(__FUNCTION__": No function is associated with this UnitTest.");
    }

    return mTestResult = mTestFunc( );
}