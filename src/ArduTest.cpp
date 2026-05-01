#include "ArduTest.h"

ArduTestRunner ArduTest;

ArduTestRegistrar::ArduTestRegistrar(ArduTestCase *testCase, const char *name, ArduTestFunction function)
{
    testCase->name = name;
    testCase->function = function;
    testCase->next = nullptr;
    ArduTest.registerTest(testCase);
}

ArduTestRequirementRegistrar::ArduTestRequirementRegistrar(ArduTestRequirement *requirement, const char *testName, const char *name, bool config)
{
    requirement->testName = testName;
    requirement->name = name;
    requirement->config = config;
    requirement->next = nullptr;
    ArduTest.registerRequirement(requirement);
}

void ArduTestRunner::begin()
{
    begin(Serial);
}

void ArduTestRunner::begin(Stream &stream)
{
    stream_ = &stream;
    started_ = true;
    commandLength_ = 0;
    clearConfigs();

    if (stream_)
    {
        stream_->setTimeout(10);
        stream_->println(F("AT < READY"));
    }
}

void ArduTestRunner::poll()
{
    if (!started_)
    {
        return;
    }

    handleInput();
}

void ArduTestRunner::registerTest(ArduTestCase *testCase)
{
    if (!testCase)
    {
        return;
    }

    if (testCase->name && hasDuplicateName(testCase->name, testCase))
    {
        duplicateNames_ = true;
    }

    if (!tests_)
    {
        tests_ = testCase;
        return;
    }

    ArduTestCase *tail = tests_;
    while (tail->next)
    {
        tail = tail->next;
    }
    tail->next = testCase;
}

void ArduTestRunner::registerRequirement(ArduTestRequirement *requirement)
{
    if (!requirement)
    {
        return;
    }

    if (!requirements_)
    {
        requirements_ = requirement;
        return;
    }

    ArduTestRequirement *tail = requirements_;
    while (tail->next)
    {
        tail = tail->next;
    }
    tail->next = requirement;
}

size_t ArduTestRunner::testCount() const
{
    size_t count = 0;
    for (ArduTestCase *it = tests_; it; it = it->next)
    {
        ++count;
    }
    return count;
}

void ArduTestRunner::handleInput()
{
    if (!stream_)
    {
        return;
    }

    while (stream_->available() > 0)
    {
        char ch = (char)stream_->read();
        if (ch == '\r')
        {
            continue;
        }

        if (ch == '\n')
        {
            commandBuffer_[commandLength_] = '\0';
            if (commandLength_ > 0)
            {
                handleCommand(commandBuffer_);
            }
            commandLength_ = 0;
            continue;
        }

        if (commandLength_ + 1 >= sizeof(commandBuffer_))
        {
            commandLength_ = 0;
            printError("unknown_command", "line_too_long");
            continue;
        }

        commandBuffer_[commandLength_++] = ch;
    }
}

void ArduTestRunner::handleCommand(const char *line)
{
    if (!stream_ || !line)
    {
        return;
    }

    if (strcmp(line, "AT > HELLO 1") == 0)
    {
        stream_->print(F("AT < HELLO 1 ArduTest "));
        stream_->println(F(ARDUTEST_VERSION_STR));
        return;
    }

    if (strcmp(line, "AT > LIST") == 0)
    {
        listProtocolTests();
        return;
    }

    if (startsWith(line, "AT > SET_CONFIG "))
    {
        handleSetConfig(line + 16);
        return;
    }

    if (strcmp(line, "AT > CLEAR_CONFIG") == 0)
    {
        clearConfigs();
        return;
    }

    if (strcmp(line, "AT > RESET_STATE") == 0)
    {
        resetState();
        return;
    }

    if (startsWith(line, "AT > RUN "))
    {
        runProtocol(line + 9);
        return;
    }

    if (strcmp(line, "AT > RUN_ALL") == 0)
    {
        for (ArduTestCase *it = tests_; it; it = it->next)
        {
            runProtocol(it->name);
        }
        return;
    }

    printError("unknown_command", line);
}

bool ArduTestRunner::startsWith(const char *text, const char *prefix) const
{
    return strncmp(text, prefix, strlen(prefix)) == 0;
}

void ArduTestRunner::resetState()
{
    current_ = nullptr;
    failed_ = false;
    protocolMode_ = false;
}

void ArduTestRunner::clearConfigs()
{
    for (size_t i = 0; i < ARDUTEST_MAX_CONFIGS; ++i)
    {
        configs_[i].name[0] = '\0';
        configs_[i].value[0] = '\0';
        configs_[i].used = false;
    }
}

bool ArduTestRunner::setConfig(const char *name, const char *value)
{
    if (!name || !value || name[0] == '\0' || strlen(name) >= sizeof(configs_[0].name))
    {
        return false;
    }

    if (strlen(value) >= sizeof(configs_[0].value))
    {
        return false;
    }

    ArduTestConfigEntry *slot = nullptr;
    for (size_t i = 0; i < ARDUTEST_MAX_CONFIGS; ++i)
    {
        if (configs_[i].used && strcmp(configs_[i].name, name) == 0)
        {
            slot = &configs_[i];
            break;
        }
        if (!configs_[i].used && !slot)
        {
            slot = &configs_[i];
        }
    }

    if (!slot)
    {
        return false;
    }

    strncpy(slot->name, name, sizeof(slot->name) - 1);
    slot->name[sizeof(slot->name) - 1] = '\0';
    strncpy(slot->value, value, sizeof(slot->value) - 1);
    slot->value[sizeof(slot->value) - 1] = '\0';
    slot->used = true;
    return true;
}

const ArduTestConfigEntry *ArduTestRunner::findConfig(const char *name) const
{
    if (!name)
    {
        return nullptr;
    }

    for (size_t i = 0; i < ARDUTEST_MAX_CONFIGS; ++i)
    {
        if (configs_[i].used && strcmp(configs_[i].name, name) == 0)
        {
            return &configs_[i];
        }
    }
    return nullptr;
}

void ArduTestRunner::handleSetConfig(const char *fields)
{
    if (!stream_ || !fields)
    {
        return;
    }

    const char *separator = strchr(fields, ' ');
    if (!separator)
    {
        printError("invalid_config", "missing_length");
        return;
    }

    char name[ARDUTEST_CONFIG_NAME_LENGTH + 1];
    size_t nameLength = separator - fields;
    if (nameLength == 0 || nameLength >= sizeof(name))
    {
        printError("invalid_config", "invalid_name");
        return;
    }
    memcpy(name, fields, nameLength);
    name[nameLength] = '\0';

    char *end = nullptr;
    unsigned long length = strtoul(separator + 1, &end, 10);
    if (!end || *end != '\0')
    {
        printError("invalid_config", "invalid_length");
        return;
    }
    if (length >= sizeof(configs_[0].value))
    {
        drainBytes((size_t)length);
        printError("invalid_config", "value_too_large");
        return;
    }

    char value[ARDUTEST_CONFIG_VALUE_LENGTH + 1];
    size_t received = stream_->readBytes(value, (size_t)length);
    value[received] = '\0';
    if (received != (size_t)length)
    {
        printError("invalid_config", "payload_timeout");
        return;
    }

    if (!setConfig(name, value))
    {
        printError("invalid_config", "store_full");
    }
}

void ArduTestRunner::drainBytes(size_t length)
{
    if (!stream_)
    {
        return;
    }

    while (length > 0)
    {
        char discarded[16];
        size_t chunk = length < sizeof(discarded) ? length : sizeof(discarded);
        size_t received = stream_->readBytes(discarded, chunk);
        if (received == 0)
        {
            return;
        }
        length -= received;
    }
}

void ArduTestRunner::printError(const char *code, const char *message)
{
    if (!stream_)
    {
        return;
    }

    stream_->print(F("AT < ERROR "));
    stream_->print(code ? code : "internal_error");
    stream_->print(F(" "));
    stream_->println(message ? message : "");
}

void ArduTestRunner::listProtocolTests()
{
    if (!stream_)
    {
        return;
    }

    for (ArduTestCase *it = tests_; it; it = it->next)
    {
        stream_->print(F("AT < TEST "));
        stream_->println(it->name);
        listProtocolRequirements(it->name);
    }
    stream_->println(F("AT < END_LIST"));
}

void ArduTestRunner::listProtocolRequirements(const char *testName)
{
    for (ArduTestRequirement *it = requirements_; it; it = it->next)
    {
        if (strcmp(it->testName, testName) != 0)
        {
            continue;
        }

        stream_->print(it->config ? F("AT < REQUIRE_CONFIG ") : F("AT < REQUIRE "));
        stream_->print(testName);
        stream_->print(F(" "));
        stream_->println(it->name);
    }
}

bool ArduTestRunner::runProtocol(const char *name)
{
    for (ArduTestCase *it = tests_; it; it = it->next)
    {
        if (strcmp(it->name, name) == 0)
        {
            current_ = it;
            failed_ = false;
            protocolMode_ = true;

            if (stream_)
            {
                stream_->print(F("AT < RUNNING "));
                stream_->println(it->name);
            }

            it->function();
            printProtocolResult(*it, failed_ ? Failed : Passed);
            protocolMode_ = false;
            current_ = nullptr;
            return !failed_;
        }
    }

    if (stream_)
    {
        stream_->println(F("AT < ERROR unknown_test 0"));
    }
    return false;
}

void ArduTestRunner::log(const char *message)
{
    printPrefix("LOG");
    if (stream_)
    {
        stream_->println(message ? message : "");
    }
}

void ArduTestRunner::attachText(const char *name, const char *text)
{
    printPrefix("ARTIFACT_TEXT");
    if (stream_)
    {
        stream_->print(name ? name : "");
        stream_->print(F(" "));
        stream_->println(text ? text : "");
    }
}

void ArduTestRunner::reportMetric(const char *name, int value)
{
    reportMetric(name, (long)value);
}

void ArduTestRunner::reportMetric(const char *name, long value)
{
    printPrefix("METRIC");
    if (stream_)
    {
        stream_->print(name ? name : "");
        stream_->print(F(" "));
        stream_->println(value);
    }
}

void ArduTestRunner::reportMetric(const char *name, double value)
{
    printPrefix("METRIC");
    if (stream_)
    {
        stream_->print(name ? name : "");
        stream_->print(F(" "));
        stream_->println(value);
    }
}

const char *ArduTestRunner::config(const char *name, const char *defaultValue) const
{
    const ArduTestConfigEntry *entry = findConfig(name);
    if (entry)
    {
        return entry->value;
    }
    return defaultValue ? defaultValue : "";
}

int ArduTestRunner::configInt(const char *name, int defaultValue) const
{
    const ArduTestConfigEntry *entry = findConfig(name);
    if (entry)
    {
        return atoi(entry->value);
    }
    return defaultValue;
}

bool ArduTestRunner::configBool(const char *name, bool defaultValue) const
{
    const ArduTestConfigEntry *entry = findConfig(name);
    if (!entry)
    {
        return defaultValue;
    }

    if (strcmp(entry->value, "1") == 0 || strcmp(entry->value, "true") == 0 || strcmp(entry->value, "yes") == 0 || strcmp(entry->value, "on") == 0)
    {
        return true;
    }
    if (strcmp(entry->value, "0") == 0 || strcmp(entry->value, "false") == 0 || strcmp(entry->value, "no") == 0 || strcmp(entry->value, "off") == 0)
    {
        return false;
    }
    return defaultValue;
}

void ArduTestRunner::require(const char *name)
{
    printPrefix("REQUIRE");
    if (stream_)
    {
        stream_->println(name ? name : "");
    }
}

void ArduTestRunner::requireConfig(const char *name)
{
    printPrefix("REQUIRE_CONFIG");
    if (stream_)
    {
        stream_->println(name ? name : "");
    }
}

void ArduTestRunner::fail(const char *expr, const char *file, int line)
{
    failed_ = true;
    printPrefix("FAIL");
    if (stream_)
    {
        stream_->print(file ? file : "");
        stream_->print(F(":"));
        stream_->print(line);
        stream_->print(F(" "));
        stream_->println(expr ? expr : "");
    }
}

void ArduTestRunner::failEqual(const char *expectedExpr, const char *actualExpr, long expected, long actual, const char *file, int line)
{
    failed_ = true;
    printPrefix("FAIL_EQ");
    if (stream_)
    {
        stream_->print(file ? file : "");
        stream_->print(F(":"));
        stream_->print(line);
        stream_->print(F(" "));
        stream_->print(expectedExpr ? expectedExpr : "");
        stream_->print(F("="));
        stream_->print(expected);
        stream_->print(F(" "));
        stream_->print(actualExpr ? actualExpr : "");
        stream_->print(F("="));
        stream_->println(actual);
    }
}

bool ArduTestRunner::hasFailed() const
{
    return failed_;
}

bool ArduTestRunner::hasDuplicateName(const char *name, const ArduTestCase *ignore) const
{
    for (ArduTestCase *it = tests_; it; it = it->next)
    {
        if (it != ignore && it->name && strcmp(it->name, name) == 0)
        {
            return true;
        }
    }
    return false;
}

void ArduTestRunner::printPrefix(const char *kind)
{
    if (!stream_)
    {
        return;
    }

    if (protocolMode_)
    {
        stream_->print(F("AT < "));
        stream_->print(kind);
        stream_->print(F(" "));
        if (current_)
        {
            stream_->print(current_->name);
            stream_->print(F(" "));
        }
        return;
    }

    stream_->print(F("AT < "));
    stream_->print(kind);
    stream_->print(F(" "));
    if (current_)
    {
        stream_->print(current_->name);
        stream_->print(F(" "));
    }
}

void ArduTestRunner::printProtocolResult(const ArduTestCase &testCase, Result result)
{
    if (!stream_)
    {
        return;
    }

    stream_->print(F("AT < RESULT "));
    stream_->print(testCase.name);
    stream_->print(F(" "));
    switch (result)
    {
    case Passed:
        stream_->println(F("passed"));
        break;
    case Failed:
        stream_->println(F("failed"));
        break;
    default:
        stream_->println(F("error"));
        break;
    }
}
