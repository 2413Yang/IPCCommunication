#include "../src/ArgumentsTableLinux.hpp"

int main(){
    /* CType */
    // std::tuple<int,double,std::string> tuple1(1,3.14,"hello");
    // std::tuple<int,double,std::string> tuple2(2,3.15,"world");
    // bool res = CType<decltype(tuple1),decltype(tuple2),std::tuple_size<decltype(tuple1)>::value>::match(tuple1,tuple2);
    // std::cout << res << std::endl;
    
    /*CArgumentsProcess*/
    // std::tuple<int,double,std::string> tuple1(1,3.14,"hello");
    // CArgumentsProcess<decltype(tuple1),std::tuple_size<decltype(tuple1)>::value>::printf(tuple1);
    // Json::Value jsonValue;
    // CArgumentsProcess<decltype(tuple1),std::tuple_size<decltype(tuple1)>::value>::serialization(tuple1,jsonValue);
    // std::cout << jsonValue << std::endl;
    // std::cout << Json::FastWriter().write(jsonValue) << std::endl;
    // std::tuple<int,double,std::string> tuple2;
    // CArgumentsProcess<decltype(tuple2),std::tuple_size<decltype(tuple2)>::value>::deserialization(jsonValue,tuple2);
    // std::cout << std::get<1>(tuple2) << std::endl;

    /*CArgumentsTable*/
    CArgumentsTable<int,float,std::string> argumentTabe("test_one");
    if (argumentTabe(1,2.0,"abc")){
        std::cout << "参数填充成功" << std::endl;
        std::cout << argumentTabe.data() << std::endl;
        argumentTabe.printf();
        std::cout << argumentTabe.name() << std::endl;
    }else{
        std::cout << "参数填充失败" << std::endl;
    }
    // argumentTabe.printf();
    return 0;
}