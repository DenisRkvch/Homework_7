#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

#include "../SpaceBattle/CmdExecutor/Commands/StartExecutorCommand.h"
#include "../SpaceBattle/ICommand.h"
#include "../SpaceBattle/CmdExecutor/ICmdQueue.h"
#include "../SpaceBattle/CmdExecutor/Commands/SoftStopExecutorCommand.h"

class MockCommand : public ICommand {
public:
    MOCK_METHOD((void), execute, (), (override));
    ~MockCommand() override { Die(); }	// ��� �������� ������ �����������
    MOCK_METHOD((void), Die, ());
};

class MockQueue : public ICmdQueue {
public:
    MOCK_METHOD((void), push, (ICommand*), (override));
    MOCK_METHOD((ICommand*), pop, (), (override));
    MOCK_METHOD((bool), isEmpty, (), (override));
};

//�������� ����, ������� ���������, ��� ����� ������� soft stop, ����� ����������� ������ ����� ����, ��� ��� ������ ����������� - 2 �����
TEST(TestSoftStopExecutorCommand, command_stop_thread_after_no_command_left)
{
    // ---------- ARRANGE ----------
    MockQueue* mock_queue = new MockQueue();
    CmdExecutor* executor = new CmdExecutor(mock_queue);
    StartExecutorCommand start_command(executor);
    MockCommand* mock_command_fst = new MockCommand();
    SoftStopExecutorCommand* soft_stop_command = new SoftStopExecutorCommand(executor, mock_queue);
    MockCommand* mock_command_snd = new MockCommand();
    MockCommand* mock_command_trd = new MockCommand();

    bool thread_finished = false;

    // �������� ��� ���������� ������ - �������� �����
    executor->setActionOnFinish(std::function<void(void)>([&thread_finished]() {
        thread_finished = true;
        }));

    EXPECT_CALL(*mock_queue, isEmpty()).
        WillOnce(Return(false)).    // �������� ����� mock_command_fst
        WillOnce(Return(false)).    // �������� ����� soft_stop_command
        WillOnce(Return(false)).    // �������� � runCondition
        WillOnce(Return(false)).    // �������� ����� mock_command_snd
        WillOnce(Return(false)).    // �������� � runCondition
        WillOnce(Return(false)).    // �������� ����� mock_command_trd
        WillRepeatedly(Return(true));// �������� � runCondition

    EXPECT_CALL(*mock_queue, pop()).
        WillOnce(Return(mock_command_fst)).
        WillOnce(Return(soft_stop_command)).
        WillOnce(Return(mock_command_snd)).
        WillOnce(Return(mock_command_trd));

    EXPECT_CALL(*mock_command_fst, execute()).Times(1);
    EXPECT_CALL(*mock_command_snd, execute()).Times(1);
    EXPECT_CALL(*mock_command_trd, execute()).Times(1);

    EXPECT_CALL(*mock_command_fst, Die()).Times(1);
    EXPECT_CALL(*mock_command_snd, Die()).Times(1);
    EXPECT_CALL(*mock_command_trd, Die()).Times(1);

    // ---------- ACT ----------
    start_command.execute();
    while (!thread_finished) { ; }

    // ---------- ASSERT ----------
    EXPECT_EQ(thread_finished, true);

    // ������������ ������
    delete mock_queue;
    delete executor;
};

