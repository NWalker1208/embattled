/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include "custom_assertions.h"
#include "processor/instruction.h"
#include <string.h>

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone(void);
extern void test_fetchInstruction_shouldLoadRegisterA_whenOpcodeHasLayoutRegA(void);
extern void test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void);
extern void test_fetchInstruction_shouldLoad16BitImmediateValueA_whenOpcodeHasLayoutImmA16(void);
extern void test_fetchInstruction_shouldLoadRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8(void);
extern void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4(void);
extern void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC(void);
extern void test_fetchInstruction_shouldLoad8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16(void);
extern void test_fetchInstruction_shouldLoadRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16(void);
extern void test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16(void);
extern void test_fetchInstruction_shouldLoad16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16(void);
extern void test_writeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone(void);
extern void test_writeInstruction_shouldSaveRegisterA_whenOpcodeHasLayoutRegA(void);
extern void test_writeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB(void);
extern void test_writeInstruction_shouldSave16BitImmediateValueA_whenOpcodeHasLayoutImmA16(void);
extern void test_writeInstruction_shouldSaveRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8(void);
extern void test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4(void);
extern void test_writeInstruction_shouldSaveRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC(void);
extern void test_writeInstruction_shouldSave8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16(void);
extern void test_writeInstruction_shouldSaveRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16(void);
extern void test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16(void);
extern void test_writeInstruction_shouldSave16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16(void);
extern void test_writeInstruction_shouldReturnZero_whenOpcodeIsInvalid(void);


/*=======Mock Management=====*/
static void CMock_Init(void)
{
}
static void CMock_Verify(void)
{
}
static void CMock_Destroy(void)
{
}

/*=======Test Reset Options=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  CMock_Verify();
  CMock_Destroy();
  CMock_Init();
  setUp();
}
void verifyTest(void);
void verifyTest(void)
{
  CMock_Verify();
}

/*=======Test Runner Used To Run Each Test=====*/
static void run_test(UnityTestFunction func, const char* name, UNITY_LINE_TYPE line_num)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = (UNITY_UINT) line_num;
#ifdef UNITY_USE_COMMAND_LINE_ARGS
    if (!UnityTestMatches())
        return;
#endif
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    CMock_Init();
    if (TEST_PROTECT())
    {
        setUp();
        func();
    }
    if (TEST_PROTECT())
    {
        tearDown();
        CMock_Verify();
    }
    CMock_Destroy();
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}

/*=======Parameterized Test Wrappers=====*/

/*=======MAIN=====*/
int main(void)
{
  UnityBegin("./processor/tests/instruction_tests.c");
  run_test(test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone, "test_fetchInstruction_shouldLoadOpcode_whenOpcodeHasLayoutNone", 18);
  run_test(test_fetchInstruction_shouldLoadRegisterA_whenOpcodeHasLayoutRegA, "test_fetchInstruction_shouldLoadRegisterA_whenOpcodeHasLayoutRegA", 34);
  run_test(test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB, "test_fetchInstruction_shouldLoadRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB", 51);
  run_test(test_fetchInstruction_shouldLoad16BitImmediateValueA_whenOpcodeHasLayoutImmA16, "test_fetchInstruction_shouldLoad16BitImmediateValueA_whenOpcodeHasLayoutImmA16", 68);
  run_test(test_fetchInstruction_shouldLoadRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8, "test_fetchInstruction_shouldLoadRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8", 86);
  run_test(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4, "test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4", 104);
  run_test(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC, "test_fetchInstruction_shouldLoadRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC", 123);
  run_test(test_fetchInstruction_shouldLoad8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16, "test_fetchInstruction_shouldLoad8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16", 142);
  run_test(test_fetchInstruction_shouldLoadRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16, "test_fetchInstruction_shouldLoadRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16", 161);
  run_test(test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16, "test_fetchInstruction_shouldLoadRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16", 180);
  run_test(test_fetchInstruction_shouldLoad16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16, "test_fetchInstruction_shouldLoad16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16", 200);
  run_test(test_writeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone, "test_writeInstruction_shouldSaveOpcode_whenOpcodeHasLayoutNone", 224);
  run_test(test_writeInstruction_shouldSaveRegisterA_whenOpcodeHasLayoutRegA, "test_writeInstruction_shouldSaveRegisterA_whenOpcodeHasLayoutRegA", 238);
  run_test(test_writeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB, "test_writeInstruction_shouldSaveRegisterAAndRegisterB_whenOpcodeHasLayoutRegARegB", 253);
  run_test(test_writeInstruction_shouldSave16BitImmediateValueA_whenOpcodeHasLayoutImmA16, "test_writeInstruction_shouldSave16BitImmediateValueA_whenOpcodeHasLayoutImmA16", 268);
  run_test(test_writeInstruction_shouldSaveRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8, "test_writeInstruction_shouldSaveRegisterAAnd8BitImmediateValueA_whenOpcodeHasLayoutRegAImmA8", 284);
  run_test(test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4, "test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd4BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA4", 300);
  run_test(test_writeInstruction_shouldSaveRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC, "test_writeInstruction_shouldSaveRegisterAAndRegisterBAndRegisterC_whenOpcodeHasLayoutRegARegBRegC", 317);
  run_test(test_writeInstruction_shouldSave8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16, "test_writeInstruction_shouldSave8BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA8ImmB16", 334);
  run_test(test_writeInstruction_shouldSaveRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16, "test_writeInstruction_shouldSaveRegisterAAnd16BitImmediateValueA_whenOpcodeHasLayoutRegAImmA16", 351);
  run_test(test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16, "test_writeInstruction_shouldSaveRegisterAAndRegisterBAnd16BitImmediateValueA_whenOpcodeHasLayoutRegARegBImmA16", 368);
  run_test(test_writeInstruction_shouldSave16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16, "test_writeInstruction_shouldSave16BitImmediateValueAAnd16BitImmediateValueB_whenOpcodeHasLayoutImmA16ImmB16", 386);
  run_test(test_writeInstruction_shouldReturnZero_whenOpcodeIsInvalid, "test_writeInstruction_shouldReturnZero_whenOpcodeIsInvalid", 404);

  return UNITY_END();
}
