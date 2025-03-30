#include "StartExecutorCommand.h"
#include <thread>

StartExecutorCommand::StartExecutorCommand(CmdExecutor * executor): cmdExecutor(executor)
{
}

void StartExecutorCommand::execute()
{
    std::thread* thr = new std::thread(&CmdExecutor::run, cmdExecutor);
}
