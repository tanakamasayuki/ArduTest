#pragma once

#include <Arduino.h>
#include "ardutest_version.h"

#ifndef ARDUTEST_MAX_CONFIGS
#define ARDUTEST_MAX_CONFIGS 4
#endif

#ifndef ARDUTEST_CONFIG_NAME_LENGTH
#define ARDUTEST_CONFIG_NAME_LENGTH 31
#endif

#ifndef ARDUTEST_CONFIG_VALUE_LENGTH
#define ARDUTEST_CONFIG_VALUE_LENGTH 48
#endif

typedef void (*ArduTestFunction)();

struct ArduTestCase
{
    const char *name;
    ArduTestFunction function;
    ArduTestCase *next;
};

struct ArduTestRequirement
{
    const char *testName;
    const char *name;
    bool config;
    ArduTestRequirement *next;
};

struct ArduTestConfigEntry
{
    char name[ARDUTEST_CONFIG_NAME_LENGTH + 1];
    char value[ARDUTEST_CONFIG_VALUE_LENGTH + 1];
    bool used;
};

class ArduTestRunner
{
public:
    enum Result : uint8_t
    {
        Passed,
        Failed,
        Error
    };

    void begin();
    void begin(Stream &stream);
    void poll();

    void registerTest(ArduTestCase *testCase);
    void registerRequirement(ArduTestRequirement *requirement);
    size_t testCount() const;

    void log(const char *message);
    void attachText(const char *name, const char *text);
    void attachBinary(const char *name, const char *contentType, const uint8_t *data, size_t length);
    void reportMetric(const char *name, int value);
    void reportMetric(const char *name, long value);
    void reportMetric(const char *name, double value);

    const char *config(const char *name, const char *defaultValue = "") const;
    int configInt(const char *name, int defaultValue = 0) const;
    bool configBool(const char *name, bool defaultValue = false) const;

    void require(const char *name);
    void requireConfig(const char *name);

    void fail(const char *expr, const char *file, int line);
    void failEqual(const char *expectedExpr, const char *actualExpr, long expected, long actual, const char *file, int line);
    bool hasFailed() const;

private:
    Stream *stream_;
    ArduTestCase *tests_;
    ArduTestRequirement *requirements_;
    ArduTestCase *current_;
    bool failed_;
    bool duplicateNames_;
    const char *duplicateName_;
    bool started_;
    bool protocolMode_;
    char commandBuffer_[80];
    size_t commandLength_;
    ArduTestConfigEntry configs_[ARDUTEST_MAX_CONFIGS];

    bool hasDuplicateName(const char *name, const ArduTestCase *ignore) const;
    void handleInput();
    void handleCommand(const char *line);
    bool startsWith(const char *text, const char *prefix) const;
    void resetState();
    void clearConfigs();
    bool setConfig(const char *name, const char *value);
    const ArduTestConfigEntry *findConfig(const char *name) const;
    void handleSetConfig(const char *fields);
    void drainBytes(size_t length);
    void printError(const char *code, const char *message);
    void printPayloadEvent(const char *kind, const char *testName, const char *payload);
    void listProtocolTests();
    void listProtocolRequirements(const char *testName);
    bool runProtocol(const char *name);
    void printProtocolResult(const ArduTestCase &testCase, Result result);
    void printPrefix(const char *kind);
};

class ArduTestRegistrar
{
public:
    ArduTestRegistrar(ArduTestCase *testCase, const char *name, ArduTestFunction function);
};

class ArduTestRequirementRegistrar
{
public:
    ArduTestRequirementRegistrar(ArduTestRequirement *requirement, const char *testName, const char *name, bool config);
};

extern ArduTestRunner ArduTest;

#define ARDUTEST_JOIN_INNER(a, b) a##b
#define ARDUTEST_JOIN(a, b) ARDUTEST_JOIN_INNER(a, b)

#define TEST_CASE(name) \
    static void name(); \
    static ArduTestCase ARDUTEST_JOIN(_ardutest_case_, name); \
    static ArduTestRegistrar ARDUTEST_JOIN(_ardutest_registrar_, name)(&ARDUTEST_JOIN(_ardutest_case_, name), #name, name); \
    static void name()

#define ARDUTEST_REQUIRE(test_name, requirement_name) \
    static ArduTestRequirement ARDUTEST_JOIN(_ardutest_requirement_, __LINE__); \
    static ArduTestRequirementRegistrar ARDUTEST_JOIN(_ardutest_requirement_registrar_, __LINE__)(&ARDUTEST_JOIN(_ardutest_requirement_, __LINE__), #test_name, requirement_name, false)

#define ARDUTEST_REQUIRE_CONFIG(test_name, config_name) \
    static ArduTestRequirement ARDUTEST_JOIN(_ardutest_requirement_, __LINE__); \
    static ArduTestRequirementRegistrar ARDUTEST_JOIN(_ardutest_requirement_registrar_, __LINE__)(&ARDUTEST_JOIN(_ardutest_requirement_, __LINE__), #test_name, config_name, true)

#define ASSERT_TRUE(condition) \
    do \
    { \
        if (!(condition)) \
        { \
            ArduTest.fail(#condition, __FILE__, __LINE__); \
            return; \
        } \
    } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) \
    do \
    { \
        long _ardutest_expected = (long)(expected); \
        long _ardutest_actual = (long)(actual); \
        if (_ardutest_expected != _ardutest_actual) \
        { \
            ArduTest.failEqual(#expected, #actual, _ardutest_expected, _ardutest_actual, __FILE__, __LINE__); \
            return; \
        } \
    } while (0)

#define ASSERT_NE(expected, actual) \
    do \
    { \
        long _ardutest_expected = (long)(expected); \
        long _ardutest_actual = (long)(actual); \
        if (_ardutest_expected == _ardutest_actual) \
        { \
            ArduTest.fail(#expected " != " #actual, __FILE__, __LINE__); \
            return; \
        } \
    } while (0)

#define REQUIRE(name) ArduTest.require(name)
#define REQUIRE_CONFIG(name) ArduTest.requireConfig(name)
