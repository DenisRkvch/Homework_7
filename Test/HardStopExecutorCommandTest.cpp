#include <gmock/gmock.h>
#include <gtest/gtest.h>


#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

#include "../SpaceBattle/CmdExecutor/Commands/StartExecutorCommand.h"
#include "../SpaceBattle/ICommand.h"
#include "../SpaceBattle/CmdExecutor/ICmdQueue.h"
#include "../SpaceBattle/CmdExecutor/Commands/HardStopExecutorCommand.h"

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

//Написать тест, который проверяет, что после команды hard stop, поток завершается - 1 балл
TEST(TestHardStopExecutorCommand, command_stop_thread_immediately)
{
    // ---------- ARRANGE ----------
    MockQueue* mock_queue = new MockQueue();
    CmdExecutor* executor = new CmdExecutor(mock_queue);
    StartExecutorCommand start_command(executor);
    MockCommand* mock_command_fst = new MockCommand();
    HardStopExecutorCommand* hard_stop_command = new HardStopExecutorCommand(executor);
    MockCommand* mock_command_snd = new MockCommand();
    
    bool thread_finished = false;

    // действия при завершении потока - поднятие флага
    executor->setActionOnFinish(std::function<void(void)>([&thread_finished]() {
        thread_finished = true;
    }));

    EXPECT_CALL(*mock_queue, isEmpty()).
        WillRepeatedly(Return(false));

    EXPECT_CALL(*mock_queue, pop()).
        WillOnce(Return(mock_command_fst)).
        WillOnce(Return(hard_stop_command)).
        WillRepeatedly(Return(mock_command_snd));
  
    EXPECT_CALL(*mock_command_fst, execute()).Times(1);
    EXPECT_CALL(*mock_command_snd, execute()).Times(0);

    EXPECT_CALL(*mock_command_fst, Die()).Times(1);
    EXPECT_CALL(*mock_command_snd, Die()).Times(1);

    // ---------- ACT ----------
    start_command.execute();
    while (!thread_finished) { ; }

    // ---------- ASSERT ----------
    EXPECT_EQ(thread_finished, true);

    // освобождение памяти
    delete mock_queue;
    delete mock_command_snd;
    delete executor;
};

