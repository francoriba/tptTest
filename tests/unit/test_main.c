#include "cJSON.h"
#include "monitoreo.h"
#include "unity.h"

void setUp(void)
{
    // Setup before each test
}

void tearDown(void)
{
    // Cleanup after each test
}

void test_cjson_basic_functionality(void)
{
    cJSON* json = cJSON_CreateObject();
    TEST_ASSERT_NOT_NULL(json);

    cJSON* test_string = cJSON_CreateString("test");
    cJSON_AddItemToObject(json, "test_key", test_string);

    cJSON* retrieved = cJSON_GetObjectItem(json, "test_key");
    TEST_ASSERT_NOT_NULL(retrieved);
    TEST_ASSERT_EQUAL_STRING("test", cJSON_GetStringValue(retrieved));

    cJSON_Delete(json);
}

void test_monitoreo_constants(void)
{
    TEST_ASSERT_EQUAL_STRING("1.0.0", MONITOREO_VERSION);
    TEST_ASSERT_EQUAL_INT(5, DEFAULT_INTERVAL);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cjson_basic_functionality);
    RUN_TEST(test_monitoreo_constants);

    return UNITY_END();
}
