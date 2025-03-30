#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

#include "../SpaceBattle/CmdExecutor/Commands/StartExecutorCommand.h"
#include "../SpaceBattle/ICommand.h"
#include "../SpaceBattle/CmdExecutor/ICmdQueue.h"

class MockCommand : public ICommand {
public:
    MOCK_METHOD((void), execute, (), (override));
    ~MockCommand() override { Die(); }	// для проверки вызова деструктора
    MOCK_METHOD((void), Die, ());
};

class MockQueue : public ICmdQueue {
public:
    MOCK_METHOD((void), push, (ICommand*), (override));
    MOCK_METHOD((ICommand*), pop, (), (override));
    MOCK_METHOD((bool), isEmpty, (), (override));
};

//Написать тест, который проверяет, 
//что после команды старт поток запущен - 1балл и 4 балла - если используются условные события синхронизации.
TEST(TestStartExecutorCommand, command_starts_thread)
{
    // ---------- ARRANGE ----------
    MockCommand* mock_command = new MockCommand();
    MockQueue* mock_queue = new MockQueue();
    CmdExecutor* executor = new CmdExecutor(mock_queue);
    StartExecutorCommand start_command(executor);
    bool thread_started = false;
    bool cmd_executed = false;

    // действия при старте потока - поднятие флага старта
    executor->setActionOnStart(std::function<void(void)>([&thread_started]() {
        thread_started = true;
        }));
    // действия при завершении потока - освобождение памяти
    executor->setActionOnFinish(std::function<void(void)>([executor, mock_queue]() {
        delete mock_queue;
        delete executor;
        }));

    EXPECT_CALL(*mock_queue, isEmpty()).WillOnce(Return(true)).WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_queue, pop()).Times(1).WillOnce(Return(mock_command));    
    EXPECT_CALL(*mock_command, execute()).
        Times(1).
        WillOnce([&cmd_executed]() {
        cmd_executed = true;
            });
    EXPECT_CALL(*mock_command, Die()).Times(1);

    // ---------- ACT ----------
    start_command.execute();
    while (!cmd_executed) { ; }

    // ---------- ASSERT ----------
    EXPECT_EQ(thread_started, true);

    // завершение потока
    executor->setRunCondition(std::function < bool(void) >([]() {
        return false;
        }));
};
