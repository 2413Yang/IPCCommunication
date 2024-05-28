
#ifndef STRATEGYCONTEXT__H__
#define STRATEGYCONTEXT__H__
#include "IPCCore.h"

class CMsgService;
class CStrategyContext
{
public:
    friend CMsgService;
    CStrategyContext(void): oMsg("MsgService"){}         // Self as a publisher.

    void init(void)
    {
        oMsg.start(); //, oIvi.start(), oWatch.start(), oAnimat.start(), oUpgrade.start();
    }

private:
    CIPCConnector oMsg;
};

#endif /*STRATEGYCONTEXT__H__*/