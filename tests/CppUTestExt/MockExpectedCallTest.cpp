/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockCheckedExpectedCall.h"
#include "CppUTestExt/MockFailure.h"
#include "MockFailureTest.h"

class TypeForTestingExpectedFunctionCall
{
public:
    TypeForTestingExpectedFunctionCall(int val) : value(val) {}
    int value;
};

class TypeForTestingExpectedFunctionCallComparator : public MockNamedValueComparator
{
public:
    TypeForTestingExpectedFunctionCallComparator() {}
    virtual ~TypeForTestingExpectedFunctionCallComparator() {}

    virtual bool isEqual(const void* object1, const void* object2)
    {
        return ((TypeForTestingExpectedFunctionCall*)object1)->value == ((TypeForTestingExpectedFunctionCall*)object2)->value;
    }
    virtual SimpleString valueToString(const void* object)
    {
        return StringFrom(((TypeForTestingExpectedFunctionCall*)object)->value);
    }
};

TEST_GROUP(MockNamedValueComparatorRepository)
{
    void teardown()
    {
        CHECK_NO_MOCK_FAILURE();
    }
};

TEST(MockNamedValueComparatorRepository, getComparatorForNonExistingName)
{
    MockNamedValueComparatorRepository repository;
    POINTERS_EQUAL(NULL, repository.getComparatorForType("typeName"));
}

TEST(MockNamedValueComparatorRepository, installComparator)
{
    TypeForTestingExpectedFunctionCallComparator comparator;
    MockNamedValueComparatorRepository repository;
    repository.installComparator("typeName", comparator);
    POINTERS_EQUAL(&comparator, repository.getComparatorForType("typeName"));
}

TEST(MockNamedValueComparatorRepository, installMultipleComparator)
{
    TypeForTestingExpectedFunctionCallComparator comparator1, comparator2, comparator3;
    MockNamedValueComparatorRepository repository;
    repository.installComparator("type1", comparator1);
    repository.installComparator("type2", comparator2);
    repository.installComparator("type3", comparator3);
    POINTERS_EQUAL(&comparator3, repository.getComparatorForType("type3"));
    POINTERS_EQUAL(&comparator2, repository.getComparatorForType("type2"));
    POINTERS_EQUAL(&comparator1, repository.getComparatorForType("type1"));
}

TEST_GROUP(MockExpectedCall)
{
    MockCheckedExpectedCall* call;
    void setup()
    {
        call = new MockCheckedExpectedCall;
    }
    void teardown()
    {
        delete call;
        CHECK_NO_MOCK_FAILURE();
    }
};

TEST(MockExpectedCall, callWithoutParameterSetOrNotFound)
{
    STRCMP_EQUAL("", call->getInputParameterType("nonexisting").asCharString());
    LONGS_EQUAL(0, call->getInputParameter("nonexisting").getIntValue());
    CHECK(!call->hasInputParameterWithName("nonexisting"));
}

TEST(MockExpectedCall, callWithUnsignedIntegerParameter)
{
    const SimpleString name = "unsigned integer";
    unsigned int value = 777;
    call->withParameter(name, value);
    STRCMP_EQUAL("unsigned int", call->getInputParameterType(name).asCharString());
    LONGS_EQUAL(value, call->getInputParameter(name).getUnsignedIntValue());
    CHECK(call->hasInputParameterWithName(name));
}

TEST(MockExpectedCall, callWithIntegerParameter)
{
    call->withParameter("integer", 1);
    STRCMP_EQUAL("int", call->getInputParameterType("integer").asCharString());
    LONGS_EQUAL(1, call->getInputParameter("integer").getIntValue());
    CHECK(call->hasInputParameterWithName("integer"));
}

TEST(MockExpectedCall, callWithUnsignedLongIntegerParameter)
{
    const SimpleString name = "unsigned long integer";
    unsigned long value = 777;
    call->withParameter(name, value);
    STRCMP_EQUAL("unsigned long int", call->getInputParameterType(name).asCharString());
    LONGS_EQUAL(value, call->getInputParameter(name).getUnsignedLongIntValue());
    CHECK(call->hasInputParameterWithName(name));
}

TEST(MockExpectedCall, callWithLongIntegerParameter)
{
    const SimpleString name = "long integer";
    long value = 777;
    call->withParameter(name, value);
    STRCMP_EQUAL("long int", call->getInputParameterType(name).asCharString());
    LONGS_EQUAL(value, call->getInputParameter(name).getLongIntValue());
    CHECK(call->hasInputParameterWithName(name));
}

TEST(MockExpectedCall, callWithDoubleParameter)
{
    call->withParameter("double", 1.2);
    STRCMP_EQUAL("double", call->getInputParameterType("double").asCharString());
    DOUBLES_EQUAL(1.2, call->getInputParameter("double").getDoubleValue(), 0.05);
}

TEST(MockExpectedCall, callWithStringParameter)
{
    call->withParameter("string", "hello world");
    STRCMP_EQUAL("const char*", call->getInputParameterType("string").asCharString());
    STRCMP_EQUAL("hello world", call->getInputParameter("string").getStringValue());
}

TEST(MockExpectedCall, callWithPointerParameter)
{
    void* ptr = (void*) 0x123;
    call->withParameter("pointer", ptr);
    STRCMP_EQUAL("void*", call->getInputParameterType("pointer").asCharString());
    POINTERS_EQUAL(ptr, call->getInputParameter("pointer").getPointerValue());
}

TEST(MockExpectedCall, callWithConstPointerParameter)
{
    const void* ptr = (const void*) 0x345;
    call->withParameter("constPointer", ptr);
    STRCMP_EQUAL("const void*", call->getInputParameterType("constPointer").asCharString());
    POINTERS_EQUAL(ptr, call->getInputParameter("constPointer").getConstPointerValue());
}

TEST(MockExpectedCall, callWithObjectParameter)
{
    void* ptr = (void*) 0x123;
    call->withParameterOfType("class", "object", ptr);
    POINTERS_EQUAL(ptr, call->getInputParameter("object").getObjectPointer());
    STRCMP_EQUAL("class", call->getInputParameterType("object").asCharString());
}

TEST(MockExpectedCall, callWithObjectParameterUnequalComparison)
{
    TypeForTestingExpectedFunctionCall type(1), unequalType(2);
    MockNamedValue parameter("name");
    parameter.setObjectPointer("type", &unequalType);
    call->withParameterOfType("type", "name", &type);
    CHECK(!call->hasInputParameter(parameter));
}

TEST(MockExpectedCall, callWithObjectParameterEqualComparisonButFailsWithoutRepository)
{
    TypeForTestingExpectedFunctionCall type(1), equalType(1);
    MockNamedValue parameter("name");
    parameter.setObjectPointer("type", &equalType);
    call->withParameterOfType("type", "name", &type);
    CHECK(!call->hasInputParameter(parameter));
}

TEST(MockExpectedCall, callWithObjectParameterEqualComparisonButFailsWithoutComparator)
{
    MockNamedValueComparatorRepository repository;
    MockNamedValue::setDefaultComparatorRepository(&repository);

    TypeForTestingExpectedFunctionCall type(1), equalType(1);
    MockNamedValue parameter("name");
    parameter.setObjectPointer("type", &equalType);
    call->withParameterOfType("type", "name", &type);
    CHECK(!call->hasInputParameter(parameter));
}

TEST(MockExpectedCall, callWithObjectParameterEqualComparison)
{
    TypeForTestingExpectedFunctionCallComparator comparator;
    MockNamedValueComparatorRepository repository;
    MockNamedValue::setDefaultComparatorRepository(&repository);
    repository.installComparator("type", comparator);

    TypeForTestingExpectedFunctionCall type(1), equalType(1);
    MockNamedValue parameter("name");
    parameter.setObjectPointer("type", &equalType);

    call->withParameterOfType("type", "name", &type);
    CHECK(call->hasInputParameter(parameter));
}

TEST(MockExpectedCall, getParameterValueOfObjectType)
{
    TypeForTestingExpectedFunctionCallComparator comparator;
    MockNamedValueComparatorRepository repository;
    MockNamedValue::setDefaultComparatorRepository(&repository);
    repository.installComparator("type", comparator);

    TypeForTestingExpectedFunctionCall type(1);
    call->withParameterOfType("type", "name", &type);
    POINTERS_EQUAL(&type, call->getInputParameter("name").getObjectPointer());
    STRCMP_EQUAL("1", call->getInputParameterValueString("name").asCharString());
}

TEST(MockExpectedCall, getParameterValueOfObjectTypeWithoutRepository)
{
    TypeForTestingExpectedFunctionCall type(1);
    call->withParameterOfType("type", "name", &type);
    STRCMP_EQUAL("No comparator found for type: \"type\"", call->getInputParameterValueString("name").asCharString());
}

TEST(MockExpectedCall, getParameterValueOfObjectTypeWithoutComparator)
{
    TypeForTestingExpectedFunctionCall type(1);
    MockNamedValueComparatorRepository repository;
    MockNamedValue::setDefaultComparatorRepository(&repository);
    call->withParameterOfType("type", "name", &type);
    STRCMP_EQUAL("No comparator found for type: \"type\"", call->getInputParameterValueString("name").asCharString());
}

TEST(MockExpectedCall, callWithTwoUnsignedIntegerParameter)
{
    unsigned int expected_value = 1;
    unsigned int another_expected_value = 2;

    call->withParameter("unsigned-integer1", expected_value);
    call->withParameter("unsigned-integer2", another_expected_value);
    STRCMP_EQUAL("unsigned int", call->getInputParameterType("unsigned-integer1").asCharString());
    STRCMP_EQUAL("unsigned int", call->getInputParameterType("unsigned-integer2").asCharString());
    LONGS_EQUAL(expected_value, call->getInputParameter("unsigned-integer1").getUnsignedIntValue());
    LONGS_EQUAL(another_expected_value, call->getInputParameter("unsigned-integer2").getUnsignedIntValue());
}

TEST(MockExpectedCall, callWithTwoIntegerParameter)
{
    int expected_value = 1;
    int another_expected_value = -1;

    call->withParameter("integer1", expected_value);
    call->withParameter("integer2", another_expected_value);
    STRCMP_EQUAL("int", call->getInputParameterType("integer1").asCharString());
    STRCMP_EQUAL("int", call->getInputParameterType("integer2").asCharString());
    LONGS_EQUAL(expected_value, call->getInputParameter("integer1").getIntValue());
    LONGS_EQUAL(another_expected_value, call->getInputParameter("integer2").getIntValue());
}

TEST(MockExpectedCall, callWithThreeDifferentParameter)
{
    call->withParameter("integer", 1);
    call->withParameter("string", "hello world");
    call->withParameter("double", 0.12);
    STRCMP_EQUAL("int", call->getInputParameterType("integer").asCharString());
    STRCMP_EQUAL("const char*", call->getInputParameterType("string").asCharString());
    STRCMP_EQUAL("double", call->getInputParameterType("double").asCharString());
    LONGS_EQUAL(1, call->getInputParameter("integer").getIntValue());
    STRCMP_EQUAL("hello world", call->getInputParameter("string").getStringValue());
    DOUBLES_EQUAL(0.12, call->getInputParameter("double").getDoubleValue(), 0.05);
}

TEST(MockExpectedCall, withoutANameItsFulfilled)
{
    CHECK(call->isFulfilled());
}

TEST(MockExpectedCall, withANameItsNotFulfilled)
{
    call->withName("name");
    CHECK(!call->isFulfilled());
}

TEST(MockExpectedCall, afterSettingCallFulfilledItsFulFilled)
{
    call->withName("name");
    call->callWasMade(1);
    CHECK(call->isFulfilled());
}

TEST(MockExpectedCall, calledButNotWithParameterIsNotFulFilled)
{
    call->withName("name").withParameter("para", 1);
    call->callWasMade(1);
    CHECK(!call->isFulfilled());
}

TEST(MockExpectedCall, calledAndParametersAreFulfilled)
{
    call->withName("name").withParameter("para", 1);
    call->callWasMade(1);
    call->inputParameterWasPassed("para");
    CHECK(call->isFulfilled());
}

TEST(MockExpectedCall, calledButNotAllParametersAreFulfilled)
{
    call->withName("name").withParameter("para", 1).withParameter("two", 2);
    call->callWasMade(1);
    call->inputParameterWasPassed("para");
    CHECK(!call->isFulfilled());
}

TEST(MockExpectedCall, toStringForNoParameters)
{
    call->withName("name");
    STRCMP_EQUAL("name -> no parameters", call->callToString().asCharString());
}

TEST(MockExpectedCall, toStringForIgnoredParameters)
{
    call->withName("name");
    call->ignoreOtherParameters();
    STRCMP_EQUAL("name -> all parameters ignored", call->callToString().asCharString());
}

TEST(MockExpectedCall, toStringForMultipleParameters)
{
    int int_value = 10;
    unsigned int uint_value = 7;

    call->withName("name");
    call->withParameter("string", "value");
    call->withParameter("integer", int_value);
    call->withParameter("unsigned-integer", uint_value);
    STRCMP_EQUAL("name -> const char* string: <value>, int integer: <10>, unsigned int unsigned-integer: <         7 (0x00000007)>", call->callToString().asCharString());
}

TEST(MockExpectedCall, toStringForParameterAndIgnored)
{
    call->withName("name");
    call->withParameter("string", "value");
    call->ignoreOtherParameters();
    STRCMP_EQUAL("name -> const char* string: <value>, other parameters are ignored", call->callToString().asCharString());
}

TEST(MockExpectedCall, toStringForCallOrder)
{
    call->withName("name");
    call->withCallOrder(2);
    STRCMP_EQUAL("name -> expected call order: <2> -> no parameters", call->callToString().asCharString());
}

TEST(MockExpectedCall, callOrderIsNotFulfilledWithWrongOrder)
{
    call->withName("name");
    call->withCallOrder(2);
    call->callWasMade(1);
    CHECK(call->isFulfilled());
    CHECK(call->isOutOfOrder());
}

TEST(MockExpectedCall, callOrderIsFulfilled)
{
    call->withName("name");
    call->withCallOrder(1);
    call->callWasMade(1);
    CHECK(call->isFulfilled());
    CHECK_FALSE(call->isOutOfOrder());
}

TEST(MockExpectedCall, hasOutputParameter)
{
    const int value = 1;
    call->withOutputParameterReturning("foo", &value, sizeof(value));
    MockNamedValue foo("foo");
    foo.setValue(&value);
    CHECK(call->hasOutputParameter(foo));
}

TEST(MockExpectedCall, hasNoOutputParameter)
{
    call->withIntParameter("foo", (int)1);
    MockNamedValue foo("foo");
    foo.setValue((int)1);
    CHECK_FALSE(call->hasOutputParameter(foo));
}

static MockExpectedCallComposite composite;

TEST_GROUP(MockExpectedCallComposite)
{
    MockCheckedExpectedCall call;

    void setup() _override
    {
        composite.add(call);
        composite.withName("name");
    }

    void teardown() _override
    {
        CHECK_NO_MOCK_FAILURE();
        composite.clear();
    }
};

TEST(MockExpectedCallComposite, hasLongIntParameter)
{
    composite.withParameter("param", (long int) -1);
    STRCMP_EQUAL("name -> long int param: <-1>", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasUnsignedLongIntParameter)
{
    composite.withParameter("param", (unsigned long int) 5);
    STRCMP_EQUAL("name -> unsigned long int param: <5 (0x5)>", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasPointerParameter)
{
    composite.withParameter("param", (void*) 0);
    STRCMP_EQUAL("name -> void* param: <0x0>", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasConstPointerParameter)
{
    composite.withParameter("param", (const void*) 0);
    STRCMP_EQUAL("name -> const void* param: <0x0>", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasParameterOfType)
{
    composite.withParameterOfType("type", "param", (const void*) 0);
    STRCMP_EQUAL("name -> type param: <No comparator found for type: \"type\">", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasOutputParameterReturning)
{
    composite.withOutputParameterReturning("out", (const void*) 0, 1);
    STRCMP_EQUAL("name -> const void* out: <output>", call.callToString().asCharString());
}

TEST(MockExpectedCallComposite, hasUnsignedIntReturnValue)
{
    composite.andReturnValue((unsigned int) 2);
    STRCMP_EQUAL("unsigned int", call.returnValue().getType().asCharString());
    LONGS_EQUAL(2, call.returnValue().getUnsignedIntValue());
}

TEST(MockExpectedCallComposite, hasIntReturnValue)
{
    composite.andReturnValue((int) -5);
    STRCMP_EQUAL("int", call.returnValue().getType().asCharString());
    LONGS_EQUAL(-5, call.returnValue().getIntValue());
}

TEST(MockExpectedCallComposite, hasLongIntReturnValue)
{
    composite.andReturnValue((long int) -17);
    STRCMP_EQUAL("long int", call.returnValue().getType().asCharString());
    LONGS_EQUAL(-17, call.returnValue().getLongIntValue());
}

TEST(MockExpectedCallComposite, hasUnsignedLongIntReturnValue)
{
    composite.andReturnValue((unsigned long int) 6);
    STRCMP_EQUAL("unsigned long int", call.returnValue().getType().asCharString());
    LONGS_EQUAL(6, call.returnValue().getUnsignedLongIntValue());
}

TEST(MockExpectedCallComposite, hasDoubleReturnValue)
{
    composite.andReturnValue((double) 3.005);
    STRCMP_EQUAL("double", call.returnValue().getType().asCharString());
    DOUBLES_EQUAL(3.005, call.returnValue().getDoubleValue(), 0.0001);
}

TEST(MockExpectedCallComposite, hasStringReturnValue)
{
    composite.andReturnValue("hello");
    STRCMP_EQUAL("const char*", call.returnValue().getType().asCharString());
    STRCMP_EQUAL("hello", call.returnValue().getStringValue());
}

TEST(MockExpectedCallComposite, hasPointerReturnValue)
{
    composite.andReturnValue((void*) 0);
    STRCMP_EQUAL("void*", call.returnValue().getType().asCharString());
    POINTERS_EQUAL((void*) 0, call.returnValue().getPointerValue());
}

TEST(MockExpectedCallComposite, hasConstPointerReturnValue)
{
    composite.andReturnValue((const void*) 0);
    STRCMP_EQUAL("const void*", call.returnValue().getType().asCharString());
    POINTERS_EQUAL((const void*) 0, call.returnValue().getConstPointerValue());
}

TEST(MockExpectedCallComposite, isOnObject)
{
    composite.onObject(&composite);
    SimpleString info("(object address: ");
    info += StringFromFormat("%p", (void*) &composite);
    info += ")::name -> no parameters";
    STRCMP_EQUAL(info.asCharString(), call.callToString().asCharString());
}

#include "CppUTest/TestTestingFixture.h"

static void withCallOrderNotSupportedFailMethod_(void)
{
    composite.withCallOrder(5);
} // LCOV_EXCL_LINE

TEST(MockExpectedCallComposite, doesNotSupportCallOrder)
{
    TestTestingFixture fixture;
    fixture.setTestFunction(&withCallOrderNotSupportedFailMethod_);
    fixture.runAllTests();
    fixture.assertPrintContains("withCallOrder not supported for CompositeCalls");
}

TEST_GROUP(MockIgnoredExpectedCall)
{
    MockIgnoredExpectedCall ignored;
};

TEST(MockIgnoredExpectedCall, worksAsItShould)
{
    ignored.withName("func");
    ignored.withCallOrder(1);
    ignored.onObject((void*) 0);
    ignored.withIntParameter("bla", (int) 1);
    ignored.withUnsignedIntParameter("foo", (unsigned int) 1);
    ignored.withLongIntParameter("hey", (long int) 1);
    ignored.withUnsignedLongIntParameter("bah", (unsigned long int) 1);
    ignored.withDoubleParameter("hah", (double) 1.1f);
    ignored.withStringParameter("goo", "hello");
    ignored.withPointerParameter("pie", (void*) 0);
    ignored.withConstPointerParameter("woo", (const void*) 0);
    ignored.withParameterOfType("top", "mytype", (const void*) 0);
    ignored.withOutputParameterReturning("bar", (const void*) 0, 1);
    ignored.ignoreOtherParameters();
    ignored.andReturnValue((double) 1.0f);
    ignored.andReturnValue((unsigned int) 1);
    ignored.andReturnValue((int) 1);
    ignored.andReturnValue((unsigned long int) 1);
    ignored.andReturnValue((long int) 1);
    ignored.andReturnValue("boo");
    ignored.andReturnValue((void*) 0);
    ignored.andReturnValue((const void*) 0);
}
