

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
	int m_iKnown_request_days;
	

	//long long m_iAllCost;// 总开销
	unordered_map<string, vector<int>> m_hsServers;// 所有类型的服务器信息 key=服务器型号，value=vector<string>([0]CPU核数,[1]内存大小,[2]硬件成本,[3]每日能耗成本)
	unordered_map<string, vector<int>> m_hsVirtualMachines;// 所有类型的虚拟机信息  key=虚拟机型号, value=vector<string>([0]CPU核数, [1]内存大小, [2]是否双节点部署）
	unordered_map<int, vector<string>> m_hsUserRequest;// 用户请求 key=m_iRequestDays value=vector<string>（这一天的所有请求）
	unordered_map<int, pair<pair<int, int>, pair<int, int>>> m_hsRequested_Servers;// 已购买服务器信息 key=服务器ID，value=pair{A结点pair{剩余CPU，剩余内存}，B结点pair{剩余CPU，剩余内存}}
	unordered_map<string, pair<pair<int, int>, string>> m_hsRequested_VirtualMachines;// 已申请虚拟机信息 key=虚拟机ID，value=pair{pair{保存虚拟机所在的服务器ID，是否双节点(0：双节点，1：单节点在A，2：单节点在B)},虚拟机类型}
	unordered_map<int, pair<int, string>> m_hsServers_IdleState;   //已购买服务器信息2 ，key=服务器ID， value=pair{服务器空闲状态(0：空闲；1：工作中)， 服务器型号}
	unordered_map<int, vector<string>> m_hsServers_UseInfo;   //已购买的服务器的使用信息，int=服务器ID号，vector中为占用该服务器的所有虚拟机的id号
	unordered_map<int, float> m_hsServers_UseRate;//服务器使用率, pair(int=服务器ID， float=服务器使用率)
	unordered_map<string, int> m_vVM_information;// 优先队列，记录所有部署服务器 string=虚拟机id int=虚拟机CPU+内存

	vector<string> m_vPurchase_order;// 每天购买的服务器顺序
	vector<pair<string, int>> m_vEachDay_purchase; // 每天购买的服务器信息 string=服务器型号，int=该型号服务器购买数量
	vector<pair<int, int>> m_vOrder;//映射关系表
	vector<pair<int, int>> m_vVirtualMachine_Warehousing;// 每天虚拟机入库信息，pair{虚拟机啊ID，pair{保存虚拟机所在的服务器ID，是否双节点 0：双节点，1：单节点在A，2：单节点在B）}}
	vector<pair<string, pair<int, int>>> m_vEachDay_Migration;//每天迁移的虚拟机信息，string=虚拟机id，vector([0]:目的服务器ID；[1]:所在节点，1：单节点在A，2：单节点在B)
	//CloudResources(const CloudResources&){}//复制构造函数
	//CloudResources &operator=(const CloudResources&){}//运算符重载

public:
	CloudResources(const string& sTxt_address);//构造函数，读取txt文本信息
	CloudResources(); //标准输入构造函数
	void print();// 标准输出打印
	vector<string> Information_processing(const string& strLine) const;// 处理文本信息
	vector<int> Str_to_Int(const vector<string>& strLine) const;// 提取服务器和虚拟机的数字信息（CPU核数,内存大小,硬件成本,每日能耗成本）
	void run(const vector<string>& vRequest);
	void add(const vector<string>& strAdd_information);// 为虚拟机开辟资源
	void find_optimal_insert_server(vector<int>& vInformation, const int iDemand_CPU_num, const int iDemand_Memory_size, int iIS_dual_node, int iIs_empty);// 寻找最佳部署虚拟机的服务器
	void deploy(const vector<int>& vInformation, const int iDemand_CPU_num, const int iDemand_Memory_size);// 进行虚拟机部署
	void del(const vector<string>& strDel_information);// 删除虚拟机
	int isExist(const vector<pair<int, int>>& vTable, int iNumber);// 寻找映射关系
	vector<pair<int, int>> deal_serial_number(const vector<string>& vPurchase, const vector<pair<string, int>>& vEachDay_purchase);// 对每天购买的服务器进行重新编号
	string purchase_optimal_server(const int& iCPU_demand, const int& iMemory_demand);// 购买新的服务器

	vector<pair<int, float>>::iterator find_serverID_ptr(int serverID);//在m_hsServers_UseRate中寻找服务器id对应的位置
	void migration_init();//迁移初始化工作，确定要迁移的服务器，并将要迁移的服务器按照负载率排序，等待迁移
	int migration_deploy(const string& VM_ID, const string& VM_type);//迁移部署，将待迁移的服务器上面的虚拟机迁移到其它服务器上面

};


//构造函数，读取txt文本信息
CloudResources::CloudResources(const string& sTxt_address)
{
	// 类成员初始化
	m_iServers_num = 0;
	m_iVirtualMachines_num = 0;
	m_iRequestDays = 0;
	m_iKnown_request_days = 0;

	m_vEachDay_Migration.reserve(500);
	m_vEachDay_purchase.reserve(5000);
	m_vOrder.reserve(10000);
	m_vPurchase_order.reserve(5000);
	m_vVirtualMachine_Warehousing.reserve(20000);

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
		if (iDigit_Num == 0)
		{// 接下来每行为服务器信息
			int sum = atoi(sEachLine.c_str());//读取数字
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
			int sum = atoi(sEachLine.c_str());//读取数字
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
			vector<string> vRequset = Information_processing(sEachLine);
			m_iRequestDays = atoi(vRequset[0].c_str());
			m_iKnown_request_days = atoi(vRequset[1].c_str());
			int i = m_iKnown_request_days;
			while (i > 0)
			{
				getline(infile, sEachLine);
				int iToday_request_num = atoi(sEachLine.c_str());
				while (iToday_request_num > 0 && getline(infile, sEachLine))
				{
					m_hsUserRequest[m_iKnown_request_days - i].push_back(sEachLine);
					iToday_request_num--;
				}
				i--;
			}
			iDigit_Num++;
		}
		else
		{
			int day = iDigit_Num - 3 + m_iKnown_request_days;
			int iToday_request_num = atoi(sEachLine.c_str());
			run(m_hsUserRequest[iDigit_Num - 3]);
			while (iToday_request_num > 0 && getline(infile, sEachLine))
			{
				m_hsUserRequest[day].push_back(sEachLine);
				iToday_request_num--;
			}
			iDigit_Num++;
			if (day == m_iRequestDays - 1)
			{
				break;
			}
		}

	}

	int iRemaining_requests_num = m_iKnown_request_days;
	while (iRemaining_requests_num > 0)
	{
		cout << iDigit_Num - 3 << endl;
		//run(m_hsUserRequest[iDigit_Num - 3]);
		iDigit_Num++;
		iRemaining_requests_num--;
	}
	/*for (auto& server : m_hsRequested_Servers)
	{
		if (server.second.first.first < 0 || server.second.first.second < 0 || server.second.second.first < 0 || server.second.second.second < 0)
		{
			cout << endl << server.first << endl;
		}
	}*/
	infile.close();
}

CloudResources::CloudResources()
{
	m_iServers_num = 0;
	m_iVirtualMachines_num = 0;
	m_iRequestDays = 0;

	m_vEachDay_Migration.reserve(500);
	m_vEachDay_purchase.reserve(5000);
	m_vOrder.reserve(10000);
	m_vPurchase_order.reserve(5000);
	m_vVirtualMachine_Warehousing.reserve(20000);
	m_vVM_information.reserve(50000);

	// 判断输入信息 
	// iDigit_Num = 0：接下来每行为服务器信息
	// iDigit_Num = 1：接下来每行为虚拟机信息
	// iDigit_Num = 2：处理用户请求总天数
	// iDigit_Num > 3: 接下来每行为每天的用户请求
	iDigit_Num = 0;
	string sEachLine;//记录txt的其中一行
	while (getline(cin, sEachLine))
	{
		if (iDigit_Num == 0)
		{// 接下来每行为服务器信息
			int sum = atoi(sEachLine.c_str());//读取数字
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
			int sum = atoi(sEachLine.c_str());//读取数字
			while (sum > 0 && getline(cin, sEachLine))
			{
				vector<string> vLine_information = Information_processing(sEachLine);
				vector<int> vInformation = Str_to_Int(vector<string>(vLine_information.begin() + 1, vLine_information.end()));
				m_hsVirtualMachines[vLine_information[0]].assign(vInformation.begin(), vInformation.end());
				sum--;
			}
			iDigit_Num++;
		}
		else
		{// 处理用户请求总天数
			vector<string> vRequset = Information_processing(sEachLine);
			m_iRequestDays = atoi(vRequset[0].c_str());
			m_iKnown_request_days = atoi(vRequset[1].c_str());
			int i = m_iKnown_request_days;
			while (i > 0)
			{
				getline(cin, sEachLine);
				int iToday_request_num = atoi(sEachLine.c_str());
				while (iToday_request_num > 0 && getline(cin, sEachLine))
				{
					m_hsUserRequest[m_iKnown_request_days - i].push_back(sEachLine);
					iToday_request_num--;
				}
				i--;
			}
			iDigit_Num++;
			break;
		}
	}

	iDigit_Num = 0;
	while (iDigit_Num + m_iKnown_request_days < m_iRequestDays)
	{
		int day = iDigit_Num + m_iKnown_request_days;
		run(m_hsUserRequest[iDigit_Num]);
		fflush(stdout);
		getline(cin, sEachLine);
		int iToday_request_num = atoi(sEachLine.c_str());
		while (iToday_request_num > 0 && getline(cin, sEachLine))
		{
			m_hsUserRequest[day].push_back(sEachLine);
			iToday_request_num--;
		}
		iDigit_Num++;
	}

	int iRemaining_requests_num = m_iKnown_request_days;
	while (iRemaining_requests_num > 0)
	{
		run(m_hsUserRequest[iDigit_Num]);
		iDigit_Num++;
		iRemaining_requests_num--;
	}
}

// 输出打印
void CloudResources::print()
{

	vector<pair<int, int>> vTmpOrder = deal_serial_number(m_vPurchase_order, m_vEachDay_purchase);
	m_vOrder.insert(m_vOrder.end(), vTmpOrder.begin(), vTmpOrder.end());// 寻找购买顺序与购买记录的映射关系

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

	if (!m_vEachDay_Migration.empty())
	{//当天迁移了虚拟机
		int imigration_num = m_vEachDay_Migration.size();
		cout << "(" << "migration, " << imigration_num << ")" << endl;

		for (const auto& migration : m_vEachDay_Migration)
		{
			if (migration.second.second == 0)//双节点
			{
				int iReal_server_id = isExist(m_vOrder, migration.second.first);
				cout << "(" << migration.first << ", " << iReal_server_id << ")" << endl;
			}
			else if (migration.second.second == 1)//单节点A
			{
				int iReal_server_id = isExist(m_vOrder, migration.second.first);
				cout << "(" << migration.first << ", " << iReal_server_id << ", " << "A" << ")" << endl;
			}
			else//单节点B
			{
				int iReal_server_id = isExist(m_vOrder, migration.second.first);
				cout << "(" << migration.first << ", " << iReal_server_id << ", " << "B" << ")" << endl;
			}
		}
	}
	else
	{
		cout << "(" << "migration, " << 0 << ")" << endl;
	}

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
			vResult.emplace_back(strTmpStr);
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
		result.emplace_back(atoi(str.c_str()));
	}
	return result;
}


// 处理每天的请求
void CloudResources::run(const vector<string>& vRequest)
{

	migration_init();
	m_iToday_servers_num = m_iServers_num;
	m_vEachDay_purchase.clear();
	m_vPurchase_order.clear();
	m_vVirtualMachine_Warehousing.clear();

	int iRequest_num = vRequest.size();
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
	m_vEachDay_Migration.clear();
}


// 为虚拟机开辟资源
void CloudResources::add(const vector<string>& strAdd_information)
{
	string strDemand_VirtualMachine_name = strAdd_information[1];// 获取所需虚拟机型号
	string iDemand_VirtualMachine_id = strAdd_information[2];// 虚拟机ID
	int iDemand_CPU_num = m_hsVirtualMachines[strDemand_VirtualMachine_name][0];// 获取所需虚拟机的CPU数目
	int iDemand_Memory_size = m_hsVirtualMachines[strDemand_VirtualMachine_name][1];// 获取所需虚拟机的内存大小
	m_vVM_information[iDemand_VirtualMachine_id] = iDemand_CPU_num + iDemand_Memory_size;
	int iIS_Dual_node = m_hsVirtualMachines[strDemand_VirtualMachine_name][2];// 虚拟机是否是单节点部署
	vector<int> vCur_server_information = { -1, -1, 2147483647 };// 存放虚拟机部署位置 0：服务器ID 1：应部署节点位置 2：当前服务器CPU或内存的差 

	find_optimal_insert_server(vCur_server_information, iDemand_CPU_num, iDemand_Memory_size, iIS_Dual_node, 1);// 在非空服务器上寻找最佳虚拟机部署位置
	if (vCur_server_information[0] != -1)
	{
		deploy(vCur_server_information, iDemand_CPU_num, iDemand_Memory_size);
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = vCur_server_information[0];
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = vCur_server_information[1];
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ vCur_server_information[0],vCur_server_information[1] });
		m_hsServers_UseInfo[vCur_server_information[0]].push_back(iDemand_VirtualMachine_id);//将虚拟机id记录到对应的服务器上
		m_iVirtualMachines_num++;
		return;
	}

	find_optimal_insert_server(vCur_server_information, iDemand_CPU_num, iDemand_Memory_size, iIS_Dual_node, 0);// 在空闲服务器上寻找最佳虚拟机部署位置
	if (vCur_server_information[0] != -1)
	{
		deploy(vCur_server_information, iDemand_CPU_num, iDemand_Memory_size);
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = vCur_server_information[0];
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = vCur_server_information[1];
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ vCur_server_information[0],vCur_server_information[1] });
		m_hsServers_UseInfo[vCur_server_information[0]].emplace_back(iDemand_VirtualMachine_id);//将虚拟机id记录到对应的服务器上
		m_hsServers_IdleState[vCur_server_information[0]].first = 1;// 将该服务器置为工作状态
		m_iVirtualMachines_num++;
		return;
	}

	// 如果当前所有服务器剩余CPU数量或则剩余内存大小不够开辟新的虚拟机，购买新的服务器
	string strDemand_server_name = purchase_optimal_server(iDemand_CPU_num, iDemand_Memory_size);// 获取最佳购买的服务器型号
	m_hsServers_IdleState[m_iServers_num] = { 1,strDemand_server_name };//将新申请的服务器置为工作状态
	m_hsServers_UseRate[m_iServers_num] = iDemand_CPU_num / float(m_hsServers[strDemand_server_name][0]) + iDemand_Memory_size / float(m_hsServers[strDemand_server_name][1]);//计算服务器负载率
	m_hsServers_UseInfo[m_iServers_num].emplace_back(iDemand_VirtualMachine_id);//将虚拟机id记录到对应的服务器上


	int iA_node_CPU_num = m_hsServers[strDemand_server_name][0] / 2;// A节点CPU数目
	int iA_node_Memory_size = m_hsServers[strDemand_server_name][1] / 2;// A节点内存大小
	int iB_node_CPU_num = m_hsServers[strDemand_server_name][0] / 2;// B节点CPU数目
	int iB_node_Memory_size = m_hsServers[strDemand_server_name][1] / 2;// B节点内存大小

	if (iIS_Dual_node == 0)
	{// 单节点都加入新购服务器的A节点

		// 服务器入库
		m_hsRequested_Servers[m_iServers_num].first.first = iA_node_CPU_num - iDemand_CPU_num;
		m_hsRequested_Servers[m_iServers_num].first.second = iA_node_Memory_size - iDemand_Memory_size;
		m_hsRequested_Servers[m_iServers_num].second.first = iB_node_CPU_num;
		m_hsRequested_Servers[m_iServers_num].second.second = iB_node_Memory_size;
		// 虚拟机入库
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,1 });
	}
	else
	{//双结点虚拟机

		// 服务器入库
		m_hsRequested_Servers[m_iServers_num].first.first = iA_node_CPU_num - iDemand_CPU_num / 2;
		m_hsRequested_Servers[m_iServers_num].first.second = iA_node_Memory_size - iDemand_Memory_size / 2;
		m_hsRequested_Servers[m_iServers_num].second.first = iB_node_CPU_num - iDemand_CPU_num / 2;
		m_hsRequested_Servers[m_iServers_num].second.second = iB_node_Memory_size - iDemand_Memory_size / 2;

		// 虚拟机入库
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,0 });
	}
	m_iServers_num++;// 服务器数量+1
	m_iVirtualMachines_num++;// 虚拟机数量+1
}

// 部署虚拟机到最合适的服务器上
void CloudResources::find_optimal_insert_server(vector<int>& vInformation, const int iDemand_CPU_num, const int iDemand_Memory_size, const int iIS_Dual_node, const int iIs_empty)
{
	// 输入：
	// vInformation：存放虚拟机部署位置(该vector长度为3 0：服务器ID 1：应部署节点位置 2：当前服务器CPU或内存的差)
	// iDemand_CPU_num：虚拟机的CPU数目 
	// iDemand_Memory_size：虚拟机内存大小
	// iIS_Dual_node：虚拟机是否部署在单节点（iIS_Dual_node=0：单节点部署 iIS_Dual_node=1：双节点部署 ）
	// iIs_empty：部署虚拟机在空服务器还是非空服务器（iIs_empty=0 部署虚拟机在空服务器，iIs_empty=1 部署虚拟机在非空服务器）
	// 输出：
	// 修改后的vInformation
	for (const auto& server : m_hsRequested_Servers)
	{// 虚拟机为单节点部署
		if (m_hsServers_IdleState[server.first].first == iIs_empty)
		{// 判断部署虚拟机在空服务器还是非空服务器
			if (iIS_Dual_node == 0)
			{// 虚拟机为单节点部署
				int iCur_node_CPU_gap = server.second.first.first - server.second.second.first;// 计算服务器A、B结点CPU数目之差
				int iCur_node_Memory_gap = server.second.first.second - server.second.second.second;// 计算服务器A、B结点内存大小之差
				if (iCur_node_CPU_gap > 0 && iCur_node_Memory_gap > 0
					&& server.second.first.first >= iDemand_CPU_num && server.second.first.second >= iDemand_Memory_size
					&& abs(iCur_node_CPU_gap - iDemand_CPU_num) + abs(iCur_node_Memory_gap - iDemand_Memory_size) < vInformation[2])
				{// A节点CPU数目、内存都大于B节点
					vInformation[0] = server.first;
					vInformation[1] = 1;
					vInformation[2] = abs(iCur_node_CPU_gap - iDemand_CPU_num) + abs(iCur_node_Memory_gap - iDemand_Memory_size);
				}
				else if (iCur_node_CPU_gap < 0 && iCur_node_Memory_gap < 0
					&& server.second.second.first >= iDemand_CPU_num && server.second.second.second >= iDemand_Memory_size
					&& abs(-iCur_node_CPU_gap - iDemand_CPU_num) + abs(-iCur_node_Memory_gap - iDemand_Memory_size) < vInformation[2])
				{// A节点CPU数目、内存都小于B节点
					vInformation[0] = server.first;
					vInformation[1] = 2;
					vInformation[2] = abs(-iCur_node_CPU_gap - iDemand_CPU_num) + abs(-iCur_node_Memory_gap - iDemand_Memory_size);
				}
				else
				{// 其他情况
					if (server.second.first.first >= iDemand_CPU_num && server.second.first.second >= iDemand_Memory_size)
					{
						int iA_node_remaining_CPU_num = server.second.first.first - iDemand_CPU_num;
						int iA_node_remaining_Memory_num = server.second.first.second - iDemand_Memory_size;
						if (iA_node_remaining_CPU_num + iA_node_remaining_Memory_num < vInformation[2])
						{
							vInformation[0] = server.first;
							vInformation[1] = 1;
							vInformation[2] = iA_node_remaining_CPU_num + iA_node_remaining_Memory_num;

						}
					}
					if (server.second.second.first >= iDemand_CPU_num && server.second.second.second >= iDemand_Memory_size)
					{
						int iB_node_remaining_CPU_num = server.second.second.first - iDemand_CPU_num;
						int iB_node_remaining_Memory_num = server.second.second.second - iDemand_Memory_size;
						if (iB_node_remaining_CPU_num + iB_node_remaining_Memory_num < vInformation[2])
						{
							vInformation[0] = server.first;
							vInformation[1] = 2;
							vInformation[2] = iB_node_remaining_CPU_num + iB_node_remaining_Memory_num;

						}
					}
				}
			}
			else
			{// 虚拟机为双节点部署
				int iANode_CPU_resource_gap = server.second.first.first - iDemand_CPU_num / 2;
				int iANode_Memory_resource_gap = server.second.first.second - iDemand_Memory_size / 2;
				int iBNode_CPU_resource_gap = server.second.second.first - iDemand_CPU_num / 2;
				int iBNode_Memory_resource_gap = server.second.second.second - iDemand_Memory_size / 2;
				if (iANode_CPU_resource_gap >= 0 && iANode_Memory_resource_gap >= 0
					&& iBNode_CPU_resource_gap >= 0 && iBNode_Memory_resource_gap >= 0
					&& iANode_CPU_resource_gap + iBNode_CPU_resource_gap + iANode_Memory_resource_gap + iBNode_Memory_resource_gap < vInformation[2])
				{
					vInformation[0] = server.first;
					vInformation[1] = 0;
					vInformation[2] = iANode_CPU_resource_gap + iBNode_CPU_resource_gap + iANode_Memory_resource_gap + iBNode_Memory_resource_gap;
				}
			}
		}
	}

}

// 在已购的服务器上部署虚拟机
void CloudResources::deploy(const vector<int>& vInformation, const int iDemand_CPU_num, const int iDemand_Memory_size)
{
	// 输入：
	// vInformation：虚拟机部署位置(该vector长度为3 0：服务器ID 1：应部署节点位置 2：当前服务器CPU或内存的差)
	// iDemand_CPU_num：虚拟机的CPU数目 
	// iDemand_Memory_size：虚拟机内存大小
	switch (vInformation[1])
	{
	case 0:
		m_hsRequested_Servers[vInformation[0]].first.first -= iDemand_CPU_num / 2;
		m_hsRequested_Servers[vInformation[0]].first.second -= iDemand_Memory_size / 2;
		m_hsRequested_Servers[vInformation[0]].second.first -= iDemand_CPU_num / 2;
		m_hsRequested_Servers[vInformation[0]].second.second -= iDemand_Memory_size / 2;
		break;
	case 1:
		m_hsRequested_Servers[vInformation[0]].first.first -= iDemand_CPU_num;
		m_hsRequested_Servers[vInformation[0]].first.second -= iDemand_Memory_size;
		break;
	case 2:
		m_hsRequested_Servers[vInformation[0]].second.first -= iDemand_CPU_num;
		m_hsRequested_Servers[vInformation[0]].second.second -= iDemand_Memory_size;
		break;
	}
	m_hsServers_UseRate[vInformation[0]] = (1 - (m_hsRequested_Servers[vInformation[0]].first.first + m_hsRequested_Servers[vInformation[0]].second.first) / float(m_hsServers[m_hsServers_IdleState[vInformation[0]].second][0]) +
		1 - (m_hsRequested_Servers[vInformation[0]].first.second + m_hsRequested_Servers[vInformation[0]].second.second) / float(m_hsServers[m_hsServers_IdleState[vInformation[0]].second][1]));//计算服务器负载率
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

	if (m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first + m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first == (m_hsServers[m_hsServers_IdleState[iCur_VirtualMachine_to_Server_id].second])[0])
	{//若删除虚拟机之后使得某台服务器空闲。置服务器状态为0
		m_hsServers_IdleState[iCur_VirtualMachine_to_Server_id].first = 0;
	}

	m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].erase(remove(m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].begin(),
		m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].end(), iCur_VirtualMachine_id), m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].end());//删除对应服务器上面的虚拟机记录

	for (auto it = m_vVM_information.begin(); it != m_vVM_information.end();)
	{
		if (it->first == iCur_VirtualMachine_id)
		{
			m_vVM_information.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}

	m_hsRequested_VirtualMachines.erase(iCur_VirtualMachine_id);
	m_iVirtualMachines_num--;
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

// 购买最佳的服务器
string CloudResources::purchase_optimal_server(const int& iCPU_demand, const int& iMemory_demand)
{   // 输入：iCPU_demand：所需CPU数目 iMemory_demand：所需内存大小
	// 输出：购买的服务器型号
	float iReal_CPU_demand = 2.6 * iCPU_demand;
	float iReal_Memory_demand = 2.7 * iMemory_demand;
	float fRelative_error = 2147483647.0;// 所需服务器CPU数量和内存大小的相对误差
	string strResult;
	for (const auto& server : m_hsServers)
	{// 遍历所有型号的服务器
		int iServerCPU_size = server.second[0];
		int iServerMemory_siz = server.second[1];
		float fError = abs(iReal_CPU_demand - iServerCPU_size) + abs(iReal_Memory_demand - iServerMemory_siz);
		if (fError < fRelative_error && iServerCPU_size >= 2 * iCPU_demand && iServerMemory_siz >= 2 * iMemory_demand)
		{
			fRelative_error = fError;
			strResult = server.first;
		}
	}

	m_vPurchase_order.emplace_back(strResult);// 记录当天购买顺序

	for (auto& server : m_vEachDay_purchase)
	{// 遍历当天已购服务器
		if (server.first == strResult)
		{// 该型号服务器当天已经购买过
			server.second++;// 该型号服务器当天购买数量+1
			return strResult;
		}
	}
	m_vEachDay_purchase.push_back({ strResult,1 });// 该型号服务器当天未购买过,加入当天购买记录
	//m_hsEachDay_purchase[result]++;
	return strResult;
}

//迁移初始化工作，确定要迁移的服务器，并将要迁移的服务器按照负载率排序，等待迁移
void CloudResources::migration_init()
{
	const int  migration_upper_limit = m_hsRequested_VirtualMachines.size() * 3 / 1000;//待迁移虚拟机上限数

	vector<pair<int, float>> m_vmigration_serversSet;//待迁移服务器集合

	for (const auto& server : m_hsServers_UseRate)
	{
		if (server.second != 0.0)//server.second不能为0
		{
			m_vmigration_serversSet.emplace_back(server);
		}
	}

	sort(m_vmigration_serversSet.begin(), m_vmigration_serversSet.end(), [&](const pair<int, float>& lhs, const pair<int, float>& rhs)
		{
			return lhs.second < rhs.second;
		});//将m_vmigration_serversSet中的元素按照负载率从小到大排序。

	int migration_state;//迁移状态，1:成功； -1:失败
	int migrated_VM_num = 0;//已迁移虚拟机数量

	for (int i = 0; migrated_VM_num < migration_upper_limit && i < m_vmigration_serversSet.size(); i++)
	{//依次处理虚拟机
		int iCur_server_id = m_vmigration_serversSet[i].first;
		sort(m_hsServers_UseInfo[iCur_server_id].begin(), m_hsServers_UseInfo[iCur_server_id].end(), [&](const auto& x, const auto& y)
			{
				return m_vVM_information[x] < m_vVM_information[y];
			});
		m_hsServers_IdleState[iCur_server_id].first = 2;// 将当前服务器置为迁移状态
		for (int j = 0; j < m_hsServers_UseInfo[iCur_server_id].size();)
		{
			if (migrated_VM_num >= migration_upper_limit) break;
			string imigration_VM_ID = m_hsServers_UseInfo[iCur_server_id][j];
			migration_state = migration_deploy(imigration_VM_ID, m_hsRequested_VirtualMachines[imigration_VM_ID].second);
			if (migration_state == 1)//该虚拟机迁移成功
			{
				migrated_VM_num++;
			}
			else
			{
				j++;
			}
		}

		if (!m_hsServers_UseInfo[iCur_server_id].empty())
		{// 如果旧服务器非空，置为工作状态
			m_hsServers_IdleState[iCur_server_id].first = 1;
		}
		else
		{// 如果旧服务器空闲，置为空闲状态
			m_hsServers_IdleState[iCur_server_id].first = 0;
		}
	}
}


//迁移部署，将待迁移的服务器上面的虚拟机迁移到其它服务器上面
int CloudResources::migration_deploy(const string& VM_ID, const string& VM_type)
{
	// 输入：虚拟机信息
	string strDemand_VirtualMachine_name = VM_type;// 获取所需虚拟机型号
	string iDemand_VirtualMachine_id = VM_ID;// 虚拟机ID
	int iCur_VirtualMachine_to_Server_id = m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first;// 当前虚拟机所在服务器的ID
	int iCur_VirtualMachine_to_Server_node = m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second;// 当前虚拟机所在服务器的节点
	int iDemand_CPU_num = m_hsVirtualMachines[strDemand_VirtualMachine_name][0];// 获取所需虚拟机的CPU数目
	int iDemand_Memory_size = m_hsVirtualMachines[strDemand_VirtualMachine_name][1];// 获取所需虚拟机的内存大小
	int iIS_Dual_node = m_hsVirtualMachines[strDemand_VirtualMachine_name][2];// 虚拟机是否是单节点部署
	vector<int> vCur_server_information = { -1, -1, 2147483647 };// 存放虚拟机部署位置 0：服务器ID 1：应部署节点位置 2：当前服务器CPU或内存的差 

	find_optimal_insert_server(vCur_server_information, iDemand_CPU_num, iDemand_Memory_size, iIS_Dual_node, 1);// 在非空服务器上寻找最佳虚拟机部署位置

	if (vCur_server_information[0] != -1)
	{
		switch (vCur_server_information[1])
		{
		case 0:
			m_hsRequested_Servers[vCur_server_information[0]].first.first -= iDemand_CPU_num / 2;
			m_hsRequested_Servers[vCur_server_information[0]].first.second -= iDemand_Memory_size / 2;
			m_hsRequested_Servers[vCur_server_information[0]].second.first -= iDemand_CPU_num / 2;
			m_hsRequested_Servers[vCur_server_information[0]].second.second -= iDemand_Memory_size / 2;
			m_vEachDay_Migration.push_back({ iDemand_VirtualMachine_id ,{vCur_server_information[0],0} });//记录迁移信息
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = vCur_server_information[0];//更改虚拟机部署信息
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;//双节点
			break;
		case 1:
			m_hsRequested_Servers[vCur_server_information[0]].first.first -= iDemand_CPU_num;
			m_hsRequested_Servers[vCur_server_information[0]].first.second -= iDemand_Memory_size;
			m_vEachDay_Migration.push_back({ iDemand_VirtualMachine_id ,{vCur_server_information[0],1} });//记录迁移信息，服务器id,单节点在A
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = vCur_server_information[0];//更改虚拟机部署信息
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;//单节点在A
			break;
		case 2:
			m_hsRequested_Servers[vCur_server_information[0]].second.first -= iDemand_CPU_num;
			m_hsRequested_Servers[vCur_server_information[0]].second.second -= iDemand_Memory_size;
			m_vEachDay_Migration.push_back({ iDemand_VirtualMachine_id ,{vCur_server_information[0],2} });//记录迁移信息，服务器id,单节点在B
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = vCur_server_information[0];//更改虚拟机部署信息
			m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 2;//单节点在B
			break;
		}
		m_hsServers_UseInfo[vCur_server_information[0]].emplace_back(iDemand_VirtualMachine_id);//将虚拟机id记录到对应的新服务器上

		// 重新计算新服务器的负载率

		m_hsServers_UseRate[vCur_server_information[0]] = (1 - (m_hsRequested_Servers[vCur_server_information[0]].first.first + m_hsRequested_Servers[vCur_server_information[0]].second.first) / float(m_hsServers[m_hsServers_IdleState[vCur_server_information[0]].second][0])) +
			(1 - (m_hsRequested_Servers[vCur_server_information[0]].first.second + m_hsRequested_Servers[vCur_server_information[0]].second.second) / float(m_hsServers[m_hsServers_IdleState[vCur_server_information[0]].second][1]));//计算服务器负载率


		// 在旧服务器上删除该虚拟机
		if (iCur_VirtualMachine_to_Server_node == 0)
		{
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iDemand_CPU_num / 2;
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iDemand_Memory_size / 2;
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iDemand_CPU_num / 2;
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iDemand_Memory_size / 2;
		}
		else if (iCur_VirtualMachine_to_Server_node == 1)
		{

			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iDemand_CPU_num;
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iDemand_Memory_size;
		}
		else
		{
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iDemand_CPU_num;
			m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iDemand_Memory_size;
		}

		// 重新计算旧服务器的负载率
		m_hsServers_UseRate[iCur_VirtualMachine_to_Server_id] = (1 - (m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first + m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first) / float(m_hsServers[m_hsServers_IdleState[iCur_VirtualMachine_to_Server_id].second][0])) +
			(1 - (m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second + m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second) / float(m_hsServers[m_hsServers_IdleState[iCur_VirtualMachine_to_Server_id].second][1]));//计算服务器负载率


		m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].erase(remove(m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].begin(),
			m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].end(), iDemand_VirtualMachine_id), m_hsServers_UseInfo[iCur_VirtualMachine_to_Server_id].end());//删除对应服务器上面的虚拟机记录

		return 1;//迁移成功
	}
	return -1;//迁移失败
}

