#ifndef MSGSERVICE_H_
#define MSGSERVICE_H_

#include "MsgInterface.h"
#include "../src/StrategyContext.hpp"

class CMsgService{
public:
    CMsgService();
    ~CMsgService();
    void Init();
private:
    CStrategyContext o_stgCtx;
};

#endif //MSGSERVICE_H_