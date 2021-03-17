#ifndef _ALG_H_
#define _ALG_H_

class CloudResources
{
private:
	// 判断输入信息 
	// iDigit_Num = 0：接下来每行为服务器信息
	// iDigit_Num = 1：接下来每行为虚拟机信息
	// iDigit_Num = 2：处理用户请求总天数
	// iDigit_Num > 3: 接下来每行为每天的用户请求
	int iDigit_Num;
	int m_iServers_num;// 当前服务器数量（实时变化）
	int m_iToday_servers_num;// 当天服务器总量(第二天数量才会变化)
	int m_iVirtualMachines_num;// 当前虚拟机数量
	int m_iRequestDays;// 用户请求天数
	//long long m_iAllCost;// 总开销
	unordered_map<string, vector<int>> m_hsServers;// 所有类型的服务器信息 key=服务器型号，value=vector<string>([0]CPU核数,[1]内存大小,[2]硬件成本,[3]每日能耗成本)
	unordered_map<string, vector<int>> m_hsVirtualMachines;// 所有类型的虚拟机信息  key=虚拟机型号, value=vector<string>([0]CPU核数, [1]内存大小, [2]是否双节点部署）
	unordered_map<int, vector<string>> m_hsUserRequest;// 用户请求 key=m_iRequestDays value=vector<string>（这一天的所有请求）
	unordered_map<int, pair<pair<int, int>, pair<int, int>>> m_hsRequested_Servers;// 已购买服务器信息 key=服务器ID，value=pair{A结点pair{剩余CPU，剩余内存}，B结点pair{剩余CPU，剩余内存}}
	unordered_map<string, pair<pair<int, int>, string>> m_hsRequested_VirtualMachines;// 已申请虚拟机信息 key=虚拟机ID，value=pair{pair{保存虚拟机所在的服务器ID，是否双节点(0：双节点，1：单节点在A，2：单节点在B)},虚拟机类型}
	vector<string> m_vPurchase_order;// 每天购买的服务器顺序
	vector<pair<string, int>> m_vEachDay_purchase; // 每天购买的服务器信息 string=服务器型号，int=该型号服务器购买数量
	vector<pair<int, int>> m_vOrder;//映射关系表

	//unordered_map<string, int> m_hsAll_purchase; // 所有购买的服务器信息 key=服务器型号，value=该型号服务器购买数量
	vector<pair<int, int>> m_vVirtualMachine_Warehousing;// 每天虚拟机入库信息，pair{虚拟机啊ID，pair{保存虚拟机所在的服务器ID，是否双节点 0：双节点，1：单节点在A，2：单节点在B）}}

	//CloudResources(const CloudResources&){}//复制构造函数
	//CloudResources &operator=(const CloudResources&){}//运算符重载

public:
	CloudResources(const string& sTxt_address);//构造函数，读取txt文本信息
	CloudResources(); //标准输入构造函数
	void print();// 标准输出打印
	vector<string> Information_processing(const string& strLine) const;// 处理文本信息
	vector<int> Str_to_Int(const vector<string>& strLine) const;// 提取服务器和虚拟机的数字信息（CPU核数,内存大小,硬件成本,每日能耗成本）
	void run(const vector<string>& vRequest, const int iRequest_num);
	void add(const vector<string>& strAdd_information);// 为虚拟机开辟资源
	void del(const vector<string>& strDel_information);// 删除虚拟机
	int isExist(const vector<pair<int, int>>& vTable, int iNumber);// 寻找映射关系
	vector<pair<int, int>> deal_serial_number(const vector<string>& vPurchase, const vector<pair<string, int>>& vEachDay_purchase);// 对每天购买的服务器进行重新编号
	string find_optimal_server(const int& iCPU_demand, const int& iMemory_demand);// 购买新的服务器
};

//构造函数，读取txt文本信息
CloudResources::CloudResources(const string& sTxt_address)
{
	// 类成员初始化
	m_iServers_num = 0;
	m_iVirtualMachines_num = 0;
	m_iRequestDays = 0;
	// m_iAllCost = 0;

	ifstream infile;
	infile.open(sTxt_address.data());// 读取txt文件
	assert(infile.is_open());   // 若失败,则输出错误消息,并终止程序运行 

	// 判断输入信息 
	// iDigit_Num = 0：接下来每行为服务器信息
	// iDigit_Num = 1：接下来每行为虚拟机信息
	// iDigit_Num = 2：处理用户请求总天数
	// iDigit_Num > 3: 接下来每行为每天的用户请求

	iDigit_Num = 0;
	string sEachLine;//记录txt的其中一行
	while (getline(infile, sEachLine))
	{
		int sum = atoi(sEachLine.c_str());//读取数字
		if (iDigit_Num == 0)
		{// 接下来每行为服务器信息
			while (sum > 0 && getline(infile, sEachLine))
			{
				vector<string> vLine_information = Information_processing(sEachLine);
				vector<int> vInformation = Str_to_Int(vector<string>(vLine_information.begin() + 1, vLine_information.end()));
				m_hsServers[vLine_information[0]].assign(vInformation.begin(), vInformation.end());
				sum--;
			}
			iDigit_Num++;
		}
		else if (iDigit_Num == 1)
		{// 接下来每行为虚拟机信息
			while (sum > 0 && getline(infile, sEachLine))
			{
				vector<string> vLine_information = Information_processing(sEachLine);
				vector<int> vInformation = Str_to_Int(vector<string>(vLine_information.begin() + 1, vLine_information.end()));
				m_hsVirtualMachines[vLine_information[0]].assign(vInformation.begin(), vInformation.end());
				sum--;
			}
			iDigit_Num++;
		}
		else if (iDigit_Num == 2)
		{// 处理用户请求总天数
			m_iRequestDays = sum;
			iDigit_Num++;
		}
		else
		{// 接下来每行为每天的用户请求
			int iRequest_num = sum;
			int day = iDigit_Num - 3;// 请求的天数
			while (sum > 0 && getline(infile, sEachLine))
			{
				m_hsUserRequest[day].push_back(sEachLine);
				sum--;
			}
			m_iToday_servers_num = m_iServers_num;// 记录当天未购买服务器前的服务器总量
			run(m_hsUserRequest[day], iRequest_num);
			iDigit_Num++;
		}
	}
	infile.close();
}

CloudResources::CloudResources()
{
	m_iServers_num = 0;
	m_iVirtualMachines_num = 0;
	m_iRequestDays = 0;
	// m_iAllCost = 0;

	// 判断输入信息 
	// iDigit_Num = 0：接下来每行为服务器信息
	// iDigit_Num = 1：接下来每行为虚拟机信息
	// iDigit_Num = 2：处理用户请求总天数
	// iDigit_Num > 3: 接下来每行为每天的用户请求

	iDigit_Num = 0;
	string sEachLine;//记录txt的其中一行
	while (getline(cin, sEachLine))
	{
		int sum = atoi(sEachLine.c_str());//读取数字
		if (iDigit_Num == 0)
		{// 接下来每行为服务器信息
			while (sum > 0 && getline(cin, sEachLine))
			{
				vector<string> vLine_information = Information_processing(sEachLine);
				vector<int> vInformation = Str_to_Int(vector<string>(vLine_information.begin() + 1, vLine_information.end()));
				m_hsServers[vLine_information[0]].assign(vInformation.begin(), vInformation.end());
				sum--;
			}
			iDigit_Num++;
		}
		else if (iDigit_Num == 1)
		{// 接下来每行为虚拟机信息
			while (sum > 0 && getline(cin, sEachLine))
			{
				vector<string> vLine_information = Information_processing(sEachLine);
				vector<int> vInformation = Str_to_Int(vector<string>(vLine_information.begin() + 1, vLine_information.end()));
				m_hsVirtualMachines[vLine_information[0]].assign(vInformation.begin(), vInformation.end());
				sum--;
			}
			iDigit_Num++;
		}
		else if (iDigit_Num == 2)
		{// 处理用户请求总天数
			m_iRequestDays = sum;
			iDigit_Num++;
		}
		else
		{// 接下来每行为每天的用户请求
			int iRequest_num = sum;
			int day = iDigit_Num - 3;// 请求的天数
			while (sum > 0 && getline(cin, sEachLine))
			{
				m_hsUserRequest[day].push_back(sEachLine);
				sum--;
			}
			run(m_hsUserRequest[day], iRequest_num);
			iDigit_Num++;
		}
	}
}

// 输出打印
void CloudResources::print()
{
	if (!m_vEachDay_purchase.empty())
	{// 当天购买了服务器
		int iPurchase_num = m_vEachDay_purchase.size();
		cout << "(" << "purchase, " << iPurchase_num << ")" << endl;
		for (const auto& server : m_vEachDay_purchase)
		{
			cout << "(" << server.first << ", " << server.second << ")" << endl;
		}
	}
	else
	{
		cout << "(" << "purchase, " << 0 << ")" << endl;
	}

	cout << "(" << "migration, " << 0 << ")" << endl;

	vector<pair<int, int>> vTmpOrder = deal_serial_number(m_vPurchase_order, m_vEachDay_purchase);
	m_vOrder.insert(m_vOrder.end(), vTmpOrder.begin(), vTmpOrder.end());// 寻找购买顺序与购买记录的映射关系

	if (!m_vVirtualMachine_Warehousing.empty())
	{// 当天有虚拟机入库
		for (const auto& id : m_vVirtualMachine_Warehousing)
		{
			if (id.second == 0)
			{// 双节点
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ")" << endl;
			}
			else if (id.second == 1)
			{// 单节点部署在A
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ", " << "A" << ")" << endl;
			}
			else
			{// 单节点部署在B
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ", " << "B" << ")" << endl;
			}
		}
	}
}

//处理文本信息
vector<string> CloudResources::Information_processing(const string& strLine) const
{
	int iSize = strLine.size();
	vector<string> vResult;
	for (int i = 0; i < iSize;)
	{
		if (strLine[i] == '(' || strLine[i] == ' ' || strLine[i] == ',') i++;
		else if (strLine[i] == ')') break;
		else
		{
			int j = i;
			while (i < iSize && strLine[i] != ',' && strLine[i] != ' ' && strLine[i] != ')') i++;
			string strTmpStr = strLine.substr(j, i - j);
			vResult.push_back(strTmpStr);
		}
	}
	return vResult;
}

// 提取服务器和虚拟机的数字信息（CPU核数,内存大小,硬件成本,每日能耗成本）
vector<int> CloudResources::Str_to_Int(const vector<string>& strLine) const
{
	vector<int> result;
	for (const auto& str : strLine)
	{
		result.push_back(atoi(str.c_str()));
	}
	return result;
}


// 处理每天的请求
void CloudResources::run(const vector<string>& vRequest, const int iRequest_num)
{
	//输入： vRequest：当天所有开辟虚拟机请求 iRequest_num：当天请求数量
	m_iToday_servers_num = m_iServers_num;// 记录当天未购买服务器前的服务器总量
	m_vEachDay_purchase.clear();// 清空购买信息
	m_vPurchase_order.clear();// 清空购买顺序
	m_vVirtualMachine_Warehousing.clear();// 清空入库信息
	for (int i = 0; i < iRequest_num; i++)
	{
		vector<string> iCur_request = Information_processing(vRequest[i]);
		if (iCur_request[0] == "add")
		{
			add(iCur_request);
		}
		else if (iCur_request[0] == "del")
		{
			del(iCur_request);
		}
	}

	print();
}

// 为虚拟机开辟资源
void CloudResources::add(const vector<string>& strAdd_information)
{
	// 输入：虚拟机信息
	string strDemand_VirtualMachine_name = strAdd_information[1];// 获取所需虚拟机型号
	string iDemand_VirtualMachine_id = strAdd_information[2];// 虚拟机ID
	int iDemand_CPU_num = m_hsVirtualMachines[strDemand_VirtualMachine_name][0];// 获取所需虚拟机的CPU数目
	int iDemand_Memory_size = m_hsVirtualMachines[strDemand_VirtualMachine_name][1];// 获取所需虚拟机的内存大小
	int iIS_Dual_node = m_hsVirtualMachines[strDemand_VirtualMachine_name][2];
	for (auto& sever : m_hsRequested_Servers)
	{// 遍历已购的服务器，是否能放入虚拟机
		if (iIS_Dual_node == 0)
		{// 该虚拟机为单节点部署
			if (sever.second.first.first >= iDemand_CPU_num && sever.second.first.second >= iDemand_Memory_size)
			{// 当前服务器A节点能以放入虚拟机
				sever.second.first.first -= iDemand_CPU_num;// 当前服务器A节点CPU减少
				sever.second.first.second -= iDemand_Memory_size;// 当前服务器A节点内存减少

				//虚拟机入库
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,1 });
				return;// 无需购买新服务器，退出
			}
			if (sever.second.second.first >= iDemand_CPU_num && sever.second.second.second >= iDemand_Memory_size)
			{// 当前服务器B节点能以放入虚拟机
				sever.second.second.first -= iDemand_CPU_num;// 当前服务器B节点CPU减少
				sever.second.second.second -= iDemand_Memory_size;// 当前服务器B节点内存减少

				//虚拟机入库
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 2;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,2 });
				return;// 无需购买新服务器，退出
			}
		}
		else
		{// 该虚拟机为双节点部署
			if (sever.second.first.first >= iDemand_CPU_num / 2 && sever.second.first.second >= iDemand_Memory_size / 2 && sever.second.second.first >= iDemand_CPU_num / 2 && sever.second.second.second >= iDemand_Memory_size / 2)
			{// 当前服务器能以放入虚拟机
				sever.second.first.first -= iDemand_CPU_num / 2;// 当前服务器A节点CPU减少
				sever.second.first.second -= iDemand_Memory_size / 2;// 当前服务器A节点内存减少
				sever.second.second.first -= iDemand_CPU_num / 2;// 当前服务器A节点CPU减少
				sever.second.second.second -= iDemand_Memory_size / 2;// 当前服务器A节点内存减少

				//虚拟机入库
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,0 });
				return;// 无需购买新服务器，退出
			}
		}
	}

	// 如果当前所有服务器剩余CPU数量或则剩余内存大小不够开辟新的虚拟机，购买新的服务器
	string strDemand_server_name = find_optimal_server(iDemand_CPU_num, iDemand_Memory_size);// 获取最佳购买的服务器型号

	if (iIS_Dual_node == 0)
	{// 单节点都加入新购服务器的A节点

		// 服务器入库
		m_hsRequested_Servers[m_iServers_num].first.first = m_hsServers[strDemand_server_name][0] / 2 - iDemand_CPU_num;
		m_hsRequested_Servers[m_iServers_num].first.second = m_hsServers[strDemand_server_name][1] / 2 - iDemand_Memory_size;
		m_hsRequested_Servers[m_iServers_num].second.first = m_hsServers[strDemand_server_name][0] / 2;
		m_hsRequested_Servers[m_iServers_num].second.second = m_hsServers[strDemand_server_name][1] / 2;
		// 虚拟机入库
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,1 });
	}
	else
	{//双结点虚拟机

		// 服务器入库
		m_hsRequested_Servers[m_iServers_num].first.first = (m_hsServers[strDemand_server_name][0] - iDemand_CPU_num) / 2;
		m_hsRequested_Servers[m_iServers_num].first.second = (m_hsServers[strDemand_server_name][1] - iDemand_Memory_size) / 2;
		m_hsRequested_Servers[m_iServers_num].second.first = (m_hsServers[strDemand_server_name][0] - iDemand_CPU_num) / 2;
		m_hsRequested_Servers[m_iServers_num].second.second = (m_hsServers[strDemand_server_name][1] - iDemand_Memory_size) / 2;

		// 虚拟机入库
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,0 });
	}
	m_iServers_num++;// 服务器数量+1
	m_iVirtualMachines_num++;// 虚拟机数量+1
}


// 删除虚拟机
void CloudResources::del(const vector<string>& strDel_information)
{
	string iCur_VirtualMachine_id = strDel_information[1];// 当前虚拟机ID
	int iCur_VirtualMachine_to_Server_id = m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].first.first;// 当前虚拟机所在服务器的ID
	int iIS_Dual_node = m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].first.second;// 当前虚拟机的节点部署情况
	int iCur_VirtualMachine_CPU_num = m_hsVirtualMachines[m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].second][0];// 当前虚拟机CPU数目
	int iCur_VirtualMachine_Memory_size = m_hsVirtualMachines[m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].second][1];// 当前虚拟机内存大小
	if (iIS_Dual_node == 0)
	{//双节点部署
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iCur_VirtualMachine_CPU_num / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iCur_VirtualMachine_Memory_size / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iCur_VirtualMachine_CPU_num / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iCur_VirtualMachine_Memory_size / 2;
	}
	else if (iIS_Dual_node == 1)
	{// 单节点部署，且部署在服务器A结点上
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iCur_VirtualMachine_CPU_num;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iCur_VirtualMachine_Memory_size;
	}
	else
	{// 单节点部署，且部署在服务器B结点上
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iCur_VirtualMachine_CPU_num;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iCur_VirtualMachine_Memory_size;
	}
	m_hsRequested_VirtualMachines.erase(iCur_VirtualMachine_id);
}

// 寻表
int CloudResources::isExist(const vector<pair<int, int>>& vTable, int iNumber)
{
	for (const auto& member : vTable)
	{
		if (member.first == iNumber) return member.second;
	}
	return -1;
}

// 当天购买服务器顺序映射关系
vector<pair<int, int>> CloudResources::deal_serial_number(const vector<string>& vPurchase, const vector<pair<string, int>>& vEachDay_purchase)
{
	// 输入：vPurchase：当天购买服务器顺序  vEachDay_purchase：当天购买服务器记录
	// 输出：当天购买服务器顺序映射关系
	vector<pair<int, int>> vResult;
	int iSum = m_iToday_servers_num;
	int iSize = vPurchase.size();
	for (const auto& server : vEachDay_purchase)
	{
		int iServer_num = server.second;
		for (int i = 0; i < iSize; i++)
		{
			if (iServer_num > 0 && server.first == vPurchase[i])
			{
				vResult.push_back({ i + m_iToday_servers_num,iSum++ });
				iServer_num--;
			}
		}
	}
	return vResult;
}

// 购买最佳的服务器（当前虚拟机所需CPU和内存的5倍大小）
string CloudResources::find_optimal_server(const int& iCPU_demand, const int& iMemory_demand)
{   // 输入：iCPU_demand：所需CPU数目 iMemory_demand：所需内存大小
	// 输出：购买的服务器型号
	int iReal_CPU_demand = 3 * iCPU_demand;
	int iReal_Memory_demand = 3 * iMemory_demand;
	int iRelative_error = 2147483647;// 所需服务器CPU数量和内存大小的相对误差
	string result;
	for (const auto& server : m_hsServers)
	{//遍历所有型号的服务器
		int iServerCPU_size = server.second[0];
		int iServerMemory_siz = server.second[1];
		int iError = abs(iReal_CPU_demand - iServerCPU_size) + abs(iReal_Memory_demand - iServerMemory_siz);
		if (iError < iRelative_error)
		{
			iRelative_error = iError;
			result = server.first;
		}
	}

	m_vPurchase_order.push_back(result);// 记录当天购买顺序

	for (auto& server : m_vEachDay_purchase)
	{// 遍历当天已购服务器
		if (server.first == result)
		{// 该型号服务器当天已经购买过
			server.second++;// 该型号服务器当天购买数量+1
			return result;
		}
	}
	m_vEachDay_purchase.push_back({ result,1 });// // 该型号服务器当天未购买过,加入当天购买记录
	//m_hsEachDay_purchase[result]++;
	return result;
}


#endif // !_ALG_H_

