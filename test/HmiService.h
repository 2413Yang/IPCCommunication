#ifndef HMISERVICE_H_
#define HMISERVICE_H_

class CHmi{
    CHmi();
    ~CHmi();
    void UpdateDispVehicleSpeed(const int &fVehicleSpeed, bool validity);
};

#endif // HMISERVICE_H_