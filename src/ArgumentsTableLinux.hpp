#ifndef ARGUMENTSTABLE_LINUX__H__
#define ARGUMENTSTABLE_LINUX__H__

#include <tuple>
#include <iostream>
#include <string>
#include <json/json.h>
#include "BaseTypeJson.hpp"
//检测参数是否是字符串类型
#define isStrType(vir) ((typeid(vir) == typeid(const char *)) || typeid(vir) == typeid(std::string))

// 类型检查
template <typename ArgT1, typename ArgT2, size_t N>
class CType
{
public:
    static bool match(ArgT1 &arg1, ArgT2 &arg2)
    {
        bool ret = true;
        if (CType<ArgT1, ArgT2, N - 1>::match(arg1, arg2))
        {
            auto param1 = std::get<N - 1>(arg1);
            auto param2 = std::get<N - 1>(arg2);
            if ((typeid(param1) != typeid(param2)) && (!isStrType(param1) || !isStrType(param2)))
            {
                // LOGERR(" ERROE: The parameter tables do not match");
                std::cout << "ERROE: The parameter tables do not match" << std::endl;
                ret = false;
            }
        }
        return ret;
    }
};

template <typename ArgT1, typename ArgT2>
class CType<ArgT1, ArgT2, 1>{
public:
    static bool match(ArgT1 &arg1, ArgT2 &arg2){
        bool ret = true;
        auto param1 = std::get<0>(arg1);
        auto param2 = std::get<0>(arg2);
        if ((typeid(param1) != typeid(param2)) && (!isStrType(param1) || !isStrType(param2)))
        {
            std::cout << "ERROE: The parameter tables do not match" << std::endl;
            // LOGERR(" ERROE: The parameter tables do not match");
            ret = false;
        }
        return ret;
    }
};

//参数处理模板，处理参数的序列化和反序列化
template <typename ArgT, size_t N>
class CArgumentsProcess
{
public:
    //打印参数的类型和值。通过递归地调用自身来处理所有参数
    static void printf(ArgT arg)
    {
        CArgumentsProcess<ArgT, N - 1>::printf(arg);
        auto v = std::get<N - 1>(arg);
        std::cout << "type: " << typeid(v).name() << " value: " << v << std::endl;
    }
    //将参数序列化为 JSON 格式。它递归地调用自身来处理所有参数，并将每个参数转换为 JSON 值并添加到 Json::Value 对象中
    template <typename SerialT>
    static void serialization(const ArgT &arg, SerialT &param)
    {
        CArgumentsProcess<ArgT, N - 1>::serialization(arg, param);
        auto tmp = std::get<N - 1>(arg);
        Jsoncpp::serialize(std::string("param") + std::to_string(N - 1), tmp, param);
        // CArgumentsProcess<ArgT, N - 1>::serialization(arg, param);
        // auto tmp = std::get<N - 1>(arg);
        // Json::Value jsonEncoder;
        // jsonEncoder["param" + std::to_string(N - 1)] = tmp;
        // std::cout << "N: " << N << " jsonEncoder: " << jsonEncoder << std::endl;
        // param = jsonEncoder;
        // param.append(jsonEncoder);

    }
    //将 JSON 格式的参数反序列化为参数表格。递归地调用自身来处理所有参数，并从 Json::Value 对象中读取每个参数的值并赋给对应的参数
    static void deserialization(const Json::Value &param, ArgT &arg)
    {
        CArgumentsProcess<ArgT, N - 1>::deserialization(param, arg);
        auto &tmp = std::get<N - 1>(arg);
        if (param.isMember("param" + std::to_string(N - 1))) {
            const auto &jsonParam = param["param" + std::to_string(N - 1)];
            if (!jsonParam.isNull())
                tmp = jsonParam.as<typename std::decay<decltype(tmp)>::type>();
        }   
    }
private:
    static Json::Value jsonEncoder;
};

// 特化模板CArgumentsProcess终止条件
template <typename ArgT>
class CArgumentsProcess<ArgT,0>{
public:
    static void printf(ArgT arg){}
    template <typename SerialT>
    static void serialization(const ArgT &arg, SerialT &param){}
    static void deserialization(const Json::Value &param, ArgT &arg){}
};

//参数表格类模板，用于管理参数并提供序列化和打印功能
template <typename... T>
class CArgumentsTable
{
public:
    //参数表格类的构造函数，用于初始化参数表格的名称
    CArgumentsTable(std::string name)
        : mName(name) {}

    //重载函数调用()，用于将参数存储到参数表格中，并检查参数类型是否匹配
    bool operator()(T... args)
    {
        bool ret = false;
        auto tmp = (mTable = std::forward_as_tuple(args...));
        if (CType<decltype(tmp), decltype(mTable), sizeof...(T)>::match(tmp, mTable))
            ret = true;
        else
            std::cout << "parameter list type matches error." << std::endl;
            // LOGERR("parameter list type matches error.");
        return ret;
    }


    bool separate(Json::Value &info)
    {
        CArgumentsProcess<decltype(mTable), std::tuple_size<std::tuple<T...>>::value>::deserialization(info, mTable);
        return true;
    }
    
    //将参数表格序列化为 JSON 格式的字符串
    std::string &data(void)
    {
        Json::Value jsonEncoder;
        CArgumentsProcess<decltype(mTable), sizeof...(T)>::serialization(mTable, jsonEncoder);
        argvsData = mName + ":json:" + Json::FastWriter().write(jsonEncoder);
        return argvsData;
    }

    //用于打印参数表格中的参数类型和值
    void printf(void)
    {
        CArgumentsProcess<decltype(mTable), sizeof...(T)>::printf(mTable);
    }

    //用于获取参数表格的名称
    std::string &name()
    {
        return mName;
    }

private:
    std::string mName;          //参数表格的名称
    std::string argvsData;      //序列化后的数据
    std::tuple<T...> mTable;    //参数的元组
};

#endif /*ARGUMENTSTABLE_LINUX__H__*/
