#include <unity.h>

void setUp(void)
{
}

void tearDown(void)
{
}

// Trivial smoke test — verifies the Unity harness compiles and runs.
// Replace or extend with real tests as interpreter modules are implemented.
void test_harness_is_alive(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_harness_is_alive);
    return UNITY_END();
}
