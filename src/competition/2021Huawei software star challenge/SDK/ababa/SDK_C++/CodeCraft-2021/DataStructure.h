//
// Created by kvrmnks on 2021/3/10.
//

#ifndef PROJECT_DATASTRUCTURE_H
#define PROJECT_DATASTRUCTURE_H


#include <map>
#include <string>
#include <list>
using namespace std;

int N;										//可以采购服务器的数量,[1,100]
struct ServerInformation					//服务器信息
{
    char typeName[21];						//服务器型号
    int coreNum;							//CPU核数，不超过1024
    int memorySize;							//内存大小,单位G，不超过1024G
    int hardwareCost;						//硬件成本，不超过5e5
    int dayCost;							//每日耗能成本，不超过5000
}serverInformation[100];					//可以采购的服务器类型信息
map<string, int> mpSevere;                  //服务器型号名到编号（下标）的映射

int M;										//售买虚拟机的数量,[1,1000]
struct VirtualMachineInformation
{
    char typeName[21];						//虚拟机型号
    int coreNum;							//CPU核数
    int memorySize;							//内存大小
    int isDoubleNode;						//是否双节点部属，0为单节点部署，1为双节点部属

    int coreNumNode;						//单节点CPU核数
    int memorySizeNode;						//单节点CPU内存占用
}virtualMachineInformation[1000];			//虚拟机型号信息
map<string, int> mpVirtualMachine;          //虚拟机型号名到编号（下标）的映射


int T;										//T天用户请求序列数据,[1,1000]
struct Require
{
    int type;								//请求类型。0为add操作，1位del操作
    char virtualMachineName[21];            //虚拟机名称
    int virtualMachineNum;					//请求的虚拟机类型编号
    int id;									//虚拟机ID
}require[200000];							//用户请求
int requireNum[1000];						//第i+1天用户请求数目
int requireRank[1000];						//第i+1天用户请求的起始地址

struct VirtualMachine						//请求使用的虚拟机
{
    int id;									//虚拟机ID
    int type;								//虚拟机型号

    int nodeNum;                            //如果虚拟机在服务器中存储，nodeNum表示被部署到了哪个节点。双节点部署模式无效。
    int serverNum;                          //如果虚拟机在服务器中存储，serverNum表示该虚拟机被部署到服务器的下标
}virtualMachine[100000 + 1];
int virtualMachineNum = 0;                  //虚拟机最大下标

map<int, int> vmIdToRank;                    //虚拟机id到下标的映射

struct Server								//已经购买的、使用中的服务器
{
    int type;								//服务器型号编号
    int rank;                               //服务器编号

    int remainCoreNodeA;					//服务器A结点剩余内核
    int remainCoreNodeB;					//服务器B结点剩余内核
    int remainMemoryNodeA;					//服务器A结点剩余内存
    int remainMemoryNodeB;					//服务器B结点剩余内存

    bool open;								//服务器是否工作中
    int cost;								//该服务器的累计成本
    int dayCost;                            //该服务器每天耗电成本

    list<int> vmList;               	    //该服务器中当前存在的虚拟机下标链表

    static int severNum;					//已经购买的服务器数量

    //检查是否能把下标为rank的虚拟机添加到该服务器
    //如果虚拟机是单核模式，core表示加载到哪一个内核。0表示内核A，1表示内核B
    bool canAddVirtualMachine(int rank, int core = 0)
    {
        VirtualMachine& vm = virtualMachine[rank];
        VirtualMachineInformation& vmInfor= virtualMachineInformation[vm.type];
        if(vmInfor.isDoubleNode)
        {
            if(remainCoreNodeA < vmInfor.coreNumNode || remainCoreNodeB < vmInfor.coreNumNode)
                return false;
            if(remainMemoryNodeA < vmInfor.memorySizeNode || remainMemoryNodeB < vmInfor.memorySizeNode)
                return false;
        }
        else
        {
            if(core == 0)
            {
                if(remainCoreNodeA < vmInfor.coreNumNode)
                    return false;
                if(remainMemoryNodeA < vmInfor.memorySizeNode)
                    return false;
            }
            else if(core == 1)
            {
                if(remainCoreNodeB < vmInfor.coreNumNode)
                    return false;
                if(remainMemoryNodeB < vmInfor.memorySizeNode)
                    return false;
            }
            else return false;
        }

        return true;
    }

    //添加虚拟机到服务器,接受其在virtualMachine的下标
    //如果虚拟机是单核模式，core表示加载到哪一个内核。0表示内核A，1表示内核B
    //返回是否添加成功
    bool addVirtualMachine(int rank, int core = 0)
    {
        if(!canAddVirtualMachine(rank, core)) return false;
        VirtualMachine& vm = virtualMachine[rank];
        VirtualMachineInformation& vmInfor= virtualMachineInformation[vm.type];
        if(vmInfor.isDoubleNode)
        {
            remainCoreNodeA -= vmInfor.coreNumNode, remainCoreNodeB -= vmInfor.coreNumNode;
            remainMemoryNodeA -= vmInfor.memorySizeNode, remainMemoryNodeB -= vmInfor.memorySizeNode;

            open = true;
        }
        else
        {
            if(core == 0)
            {
                remainCoreNodeA -= vmInfor.coreNumNode;
                remainMemoryNodeA -= vmInfor.memorySizeNode;

                open = true;
                vm.nodeNum = 0;
            }
            else if(core == 1)
            {
                remainCoreNodeB -= vmInfor.coreNumNode;
                remainMemoryNodeB -= vmInfor.memorySizeNode;

                open = true;
                vm.nodeNum = 1;
            }
            else return false;
        }

        vmList.push_back(rank);
        vm.serverNum = this->rank;
        return true;
    }

    //删除虚拟机
    //若不存在ID为id的虚拟机，返回false
    bool delVirtualMachine(int id)
    {
        auto it = vmList.begin();
        while(it != vmList.end() && virtualMachine[(*it)].id != id)
            ++ it;
        if(it == vmList.end()) return false;
        VirtualMachine& vm = virtualMachine[(*it)];
        if(vm.id == id)
        {
            VirtualMachineInformation& vmInfor = virtualMachineInformation[vm.type];
            if(vmInfor.isDoubleNode)
            {
                remainCoreNodeA += vmInfor.coreNumNode, remainCoreNodeB += vmInfor.coreNumNode;
                remainMemoryNodeA += vmInfor.memorySizeNode, remainMemoryNodeB += vmInfor.memorySizeNode;
            }
            else
            {
                if(vm.nodeNum == 0)
                {
                    remainCoreNodeA += vmInfor.coreNumNode;
                    remainMemoryNodeA += vmInfor.memorySizeNode;
                }
                else if(vm.nodeNum == 1)
                {
                    remainCoreNodeB += vmInfor.coreNumNode;
                    remainMemoryNodeB += vmInfor.memorySizeNode;
                }
            }

            if(vmList.empty())
                open = false;
            return true;
        }

        return true;
    }
}server[100000 + 10];						//sever[i] 编号为i的服务器
int serverNum = 0;                           //已有服务器数量

int serverIDVM[100000 + 10];

//新增服务器
void addServer(int type)
{
    ServerInformation& seInfor = serverInformation[type];
    Server& se = server[serverNum];

    se.type = type;
    se.rank = serverNum;
    se.remainMemoryNodeA = se.remainMemoryNodeB = (seInfor.memorySize >> 1);
    se.remainCoreNodeA = se.remainCoreNodeB = (seInfor.coreNum >> 1);
    se.dayCost = seInfor.dayCost;
    se.cost += seInfor.hardwareCost;

    ++ serverNum;
}

//新增虚拟机
void addVirtualMachine(int type, int id)
{
    VirtualMachine& vm = virtualMachine[virtualMachineNum];
    VirtualMachineInformation& vmInfor = virtualMachineInformation[vm.type];

    vm.id = id;
    vm.type = type;

    ++ virtualMachineNum;
}

#endif //PROJECT_DATASTRUCTURE_H
