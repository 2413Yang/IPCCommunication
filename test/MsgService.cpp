#include "MsgService.h"

CMsgService::CMsgService(){

}

CMsgService::~CMsgService(){
    
}

void CMsgService::Init(){
    HmiIPC::publisher::UpdateDispVehicleSpeed(o_stgCtx.oMsg);
}