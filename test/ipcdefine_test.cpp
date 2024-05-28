#include "../src/IPCInterfaceDefine.hpp"
#include "../src/StrategyContext.hpp"

namespace HmiIPC{ 
    USER_DEFINED(void, UpdateDispVehicleSpeed, int, bool);
};

int main(){
    CStrategyContext o_stgCtx;
    // HmiIPC::publisher::UpdateDispVehicleSpeed(o_stgCtx.oMsg);

    // auto& it = HmiIPC::IpcUpdateDispVehicleSpeed::GetObj();
    
    return 0;
}
