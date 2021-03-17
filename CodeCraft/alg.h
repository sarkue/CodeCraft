#ifndef _ALG_H_
#define _ALG_H_

class CloudResources
{
private:
	// �ж�������Ϣ 
	// iDigit_Num = 0��������ÿ��Ϊ��������Ϣ
	// iDigit_Num = 1��������ÿ��Ϊ�������Ϣ
	// iDigit_Num = 2�������û�����������
	// iDigit_Num > 3: ������ÿ��Ϊÿ����û�����
	int iDigit_Num;
	int m_iServers_num;// ��ǰ������������ʵʱ�仯��
	int m_iToday_servers_num;// �������������(�ڶ��������Ż�仯)
	int m_iVirtualMachines_num;// ��ǰ���������
	int m_iRequestDays;// �û���������
	//long long m_iAllCost;// �ܿ���
	unordered_map<string, vector<int>> m_hsServers;// �������͵ķ�������Ϣ key=�������ͺţ�value=vector<string>([0]CPU����,[1]�ڴ��С,[2]Ӳ���ɱ�,[3]ÿ���ܺĳɱ�)
	unordered_map<string, vector<int>> m_hsVirtualMachines;// �������͵��������Ϣ  key=������ͺ�, value=vector<string>([0]CPU����, [1]�ڴ��С, [2]�Ƿ�˫�ڵ㲿��
	unordered_map<int, vector<string>> m_hsUserRequest;// �û����� key=m_iRequestDays value=vector<string>����һ�����������
	unordered_map<int, pair<pair<int, int>, pair<int, int>>> m_hsRequested_Servers;// �ѹ����������Ϣ key=������ID��value=pair{A���pair{ʣ��CPU��ʣ���ڴ�}��B���pair{ʣ��CPU��ʣ���ڴ�}}
	unordered_map<string, pair<pair<int, int>, string>> m_hsRequested_VirtualMachines;// �������������Ϣ key=�����ID��value=pair{pair{������������ڵķ�����ID���Ƿ�˫�ڵ�(0��˫�ڵ㣬1�����ڵ���A��2�����ڵ���B)},���������}
	vector<string> m_vPurchase_order;// ÿ�칺��ķ�����˳��
	vector<pair<string, int>> m_vEachDay_purchase; // ÿ�칺��ķ�������Ϣ string=�������ͺţ�int=���ͺŷ�������������
	vector<pair<int, int>> m_vOrder;//ӳ���ϵ��

	//unordered_map<string, int> m_hsAll_purchase; // ���й���ķ�������Ϣ key=�������ͺţ�value=���ͺŷ�������������
	vector<pair<int, int>> m_vVirtualMachine_Warehousing;// ÿ������������Ϣ��pair{�������ID��pair{������������ڵķ�����ID���Ƿ�˫�ڵ� 0��˫�ڵ㣬1�����ڵ���A��2�����ڵ���B��}}

	//CloudResources(const CloudResources&){}//���ƹ��캯��
	//CloudResources &operator=(const CloudResources&){}//���������

public:
	CloudResources(const string& sTxt_address);//���캯������ȡtxt�ı���Ϣ
	CloudResources(); //��׼���빹�캯��
	void print();// ��׼�����ӡ
	vector<string> Information_processing(const string& strLine) const;// �����ı���Ϣ
	vector<int> Str_to_Int(const vector<string>& strLine) const;// ��ȡ���������������������Ϣ��CPU����,�ڴ��С,Ӳ���ɱ�,ÿ���ܺĳɱ���
	void run(const vector<string>& vRequest, const int iRequest_num);
	void add(const vector<string>& strAdd_information);// Ϊ�����������Դ
	void del(const vector<string>& strDel_information);// ɾ�������
	int isExist(const vector<pair<int, int>>& vTable, int iNumber);// Ѱ��ӳ���ϵ
	vector<pair<int, int>> deal_serial_number(const vector<string>& vPurchase, const vector<pair<string, int>>& vEachDay_purchase);// ��ÿ�칺��ķ������������±��
	string find_optimal_server(const int& iCPU_demand, const int& iMemory_demand);// �����µķ�����
};

//���캯������ȡtxt�ı���Ϣ
CloudResources::CloudResources(const string& sTxt_address)
{
	// ���Ա��ʼ��
	m_iServers_num = 0;
	m_iVirtualMachines_num = 0;
	m_iRequestDays = 0;
	// m_iAllCost = 0;

	ifstream infile;
	infile.open(sTxt_address.data());// ��ȡtxt�ļ�
	assert(infile.is_open());   // ��ʧ��,�����������Ϣ,����ֹ�������� 

	// �ж�������Ϣ 
	// iDigit_Num = 0��������ÿ��Ϊ��������Ϣ
	// iDigit_Num = 1��������ÿ��Ϊ�������Ϣ
	// iDigit_Num = 2�������û�����������
	// iDigit_Num > 3: ������ÿ��Ϊÿ����û�����

	iDigit_Num = 0;
	string sEachLine;//��¼txt������һ��
	while (getline(infile, sEachLine))
	{
		int sum = atoi(sEachLine.c_str());//��ȡ����
		if (iDigit_Num == 0)
		{// ������ÿ��Ϊ��������Ϣ
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
		{// ������ÿ��Ϊ�������Ϣ
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
		{// �����û�����������
			m_iRequestDays = sum;
			iDigit_Num++;
		}
		else
		{// ������ÿ��Ϊÿ����û�����
			int iRequest_num = sum;
			int day = iDigit_Num - 3;// ���������
			while (sum > 0 && getline(infile, sEachLine))
			{
				m_hsUserRequest[day].push_back(sEachLine);
				sum--;
			}
			m_iToday_servers_num = m_iServers_num;// ��¼����δ���������ǰ�ķ���������
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

	// �ж�������Ϣ 
	// iDigit_Num = 0��������ÿ��Ϊ��������Ϣ
	// iDigit_Num = 1��������ÿ��Ϊ�������Ϣ
	// iDigit_Num = 2�������û�����������
	// iDigit_Num > 3: ������ÿ��Ϊÿ����û�����

	iDigit_Num = 0;
	string sEachLine;//��¼txt������һ��
	while (getline(cin, sEachLine))
	{
		int sum = atoi(sEachLine.c_str());//��ȡ����
		if (iDigit_Num == 0)
		{// ������ÿ��Ϊ��������Ϣ
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
		{// ������ÿ��Ϊ�������Ϣ
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
		{// �����û�����������
			m_iRequestDays = sum;
			iDigit_Num++;
		}
		else
		{// ������ÿ��Ϊÿ����û�����
			int iRequest_num = sum;
			int day = iDigit_Num - 3;// ���������
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

// �����ӡ
void CloudResources::print()
{
	if (!m_vEachDay_purchase.empty())
	{// ���칺���˷�����
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
	m_vOrder.insert(m_vOrder.end(), vTmpOrder.begin(), vTmpOrder.end());// Ѱ�ҹ���˳���빺���¼��ӳ���ϵ

	if (!m_vVirtualMachine_Warehousing.empty())
	{// ��������������
		for (const auto& id : m_vVirtualMachine_Warehousing)
		{
			if (id.second == 0)
			{// ˫�ڵ�
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ")" << endl;
			}
			else if (id.second == 1)
			{// ���ڵ㲿����A
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ", " << "A" << ")" << endl;
			}
			else
			{// ���ڵ㲿����B
				int iReal_server_id = isExist(m_vOrder, id.first);
				cout << "(" << iReal_server_id << ", " << "B" << ")" << endl;
			}
		}
	}
}

//�����ı���Ϣ
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

// ��ȡ���������������������Ϣ��CPU����,�ڴ��С,Ӳ���ɱ�,ÿ���ܺĳɱ���
vector<int> CloudResources::Str_to_Int(const vector<string>& strLine) const
{
	vector<int> result;
	for (const auto& str : strLine)
	{
		result.push_back(atoi(str.c_str()));
	}
	return result;
}


// ����ÿ�������
void CloudResources::run(const vector<string>& vRequest, const int iRequest_num)
{
	//���룺 vRequest���������п������������ iRequest_num��������������
	m_iToday_servers_num = m_iServers_num;// ��¼����δ���������ǰ�ķ���������
	m_vEachDay_purchase.clear();// ��չ�����Ϣ
	m_vPurchase_order.clear();// ��չ���˳��
	m_vVirtualMachine_Warehousing.clear();// ��������Ϣ
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

// Ϊ�����������Դ
void CloudResources::add(const vector<string>& strAdd_information)
{
	// ���룺�������Ϣ
	string strDemand_VirtualMachine_name = strAdd_information[1];// ��ȡ����������ͺ�
	string iDemand_VirtualMachine_id = strAdd_information[2];// �����ID
	int iDemand_CPU_num = m_hsVirtualMachines[strDemand_VirtualMachine_name][0];// ��ȡ�����������CPU��Ŀ
	int iDemand_Memory_size = m_hsVirtualMachines[strDemand_VirtualMachine_name][1];// ��ȡ������������ڴ��С
	int iIS_Dual_node = m_hsVirtualMachines[strDemand_VirtualMachine_name][2];
	for (auto& sever : m_hsRequested_Servers)
	{// �����ѹ��ķ��������Ƿ��ܷ��������
		if (iIS_Dual_node == 0)
		{// �������Ϊ���ڵ㲿��
			if (sever.second.first.first >= iDemand_CPU_num && sever.second.first.second >= iDemand_Memory_size)
			{// ��ǰ������A�ڵ����Է��������
				sever.second.first.first -= iDemand_CPU_num;// ��ǰ������A�ڵ�CPU����
				sever.second.first.second -= iDemand_Memory_size;// ��ǰ������A�ڵ��ڴ����

				//��������
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,1 });
				return;// ���蹺���·��������˳�
			}
			if (sever.second.second.first >= iDemand_CPU_num && sever.second.second.second >= iDemand_Memory_size)
			{// ��ǰ������B�ڵ����Է��������
				sever.second.second.first -= iDemand_CPU_num;// ��ǰ������B�ڵ�CPU����
				sever.second.second.second -= iDemand_Memory_size;// ��ǰ������B�ڵ��ڴ����

				//��������
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 2;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,2 });
				return;// ���蹺���·��������˳�
			}
		}
		else
		{// �������Ϊ˫�ڵ㲿��
			if (sever.second.first.first >= iDemand_CPU_num / 2 && sever.second.first.second >= iDemand_Memory_size / 2 && sever.second.second.first >= iDemand_CPU_num / 2 && sever.second.second.second >= iDemand_Memory_size / 2)
			{// ��ǰ���������Է��������
				sever.second.first.first -= iDemand_CPU_num / 2;// ��ǰ������A�ڵ�CPU����
				sever.second.first.second -= iDemand_Memory_size / 2;// ��ǰ������A�ڵ��ڴ����
				sever.second.second.first -= iDemand_CPU_num / 2;// ��ǰ������A�ڵ�CPU����
				sever.second.second.second -= iDemand_Memory_size / 2;// ��ǰ������A�ڵ��ڴ����

				//��������
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = sever.first;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;
				m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
				m_iVirtualMachines_num++;
				m_vVirtualMachine_Warehousing.push_back({ sever.first,0 });
				return;// ���蹺���·��������˳�
			}
		}
	}

	// �����ǰ���з�����ʣ��CPU��������ʣ���ڴ��С���������µ�������������µķ�����
	string strDemand_server_name = find_optimal_server(iDemand_CPU_num, iDemand_Memory_size);// ��ȡ��ѹ���ķ������ͺ�

	if (iIS_Dual_node == 0)
	{// ���ڵ㶼�����¹���������A�ڵ�

		// ���������
		m_hsRequested_Servers[m_iServers_num].first.first = m_hsServers[strDemand_server_name][0] / 2 - iDemand_CPU_num;
		m_hsRequested_Servers[m_iServers_num].first.second = m_hsServers[strDemand_server_name][1] / 2 - iDemand_Memory_size;
		m_hsRequested_Servers[m_iServers_num].second.first = m_hsServers[strDemand_server_name][0] / 2;
		m_hsRequested_Servers[m_iServers_num].second.second = m_hsServers[strDemand_server_name][1] / 2;
		// ��������
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 1;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,1 });
	}
	else
	{//˫��������

		// ���������
		m_hsRequested_Servers[m_iServers_num].first.first = (m_hsServers[strDemand_server_name][0] - iDemand_CPU_num) / 2;
		m_hsRequested_Servers[m_iServers_num].first.second = (m_hsServers[strDemand_server_name][1] - iDemand_Memory_size) / 2;
		m_hsRequested_Servers[m_iServers_num].second.first = (m_hsServers[strDemand_server_name][0] - iDemand_CPU_num) / 2;
		m_hsRequested_Servers[m_iServers_num].second.second = (m_hsServers[strDemand_server_name][1] - iDemand_Memory_size) / 2;

		// ��������
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.first = m_iServers_num;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].first.second = 0;
		m_hsRequested_VirtualMachines[iDemand_VirtualMachine_id].second = strDemand_VirtualMachine_name;
		m_vVirtualMachine_Warehousing.push_back({ m_iServers_num,0 });
	}
	m_iServers_num++;// ����������+1
	m_iVirtualMachines_num++;// ���������+1
}


// ɾ�������
void CloudResources::del(const vector<string>& strDel_information)
{
	string iCur_VirtualMachine_id = strDel_information[1];// ��ǰ�����ID
	int iCur_VirtualMachine_to_Server_id = m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].first.first;// ��ǰ��������ڷ�������ID
	int iIS_Dual_node = m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].first.second;// ��ǰ������Ľڵ㲿�����
	int iCur_VirtualMachine_CPU_num = m_hsVirtualMachines[m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].second][0];// ��ǰ�����CPU��Ŀ
	int iCur_VirtualMachine_Memory_size = m_hsVirtualMachines[m_hsRequested_VirtualMachines[iCur_VirtualMachine_id].second][1];// ��ǰ������ڴ��С
	if (iIS_Dual_node == 0)
	{//˫�ڵ㲿��
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iCur_VirtualMachine_CPU_num / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iCur_VirtualMachine_Memory_size / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iCur_VirtualMachine_CPU_num / 2;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iCur_VirtualMachine_Memory_size / 2;
	}
	else if (iIS_Dual_node == 1)
	{// ���ڵ㲿���Ҳ����ڷ�����A�����
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.first += iCur_VirtualMachine_CPU_num;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].first.second += iCur_VirtualMachine_Memory_size;
	}
	else
	{// ���ڵ㲿���Ҳ����ڷ�����B�����
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.first += iCur_VirtualMachine_CPU_num;
		m_hsRequested_Servers[iCur_VirtualMachine_to_Server_id].second.second += iCur_VirtualMachine_Memory_size;
	}
	m_hsRequested_VirtualMachines.erase(iCur_VirtualMachine_id);
}

// Ѱ��
int CloudResources::isExist(const vector<pair<int, int>>& vTable, int iNumber)
{
	for (const auto& member : vTable)
	{
		if (member.first == iNumber) return member.second;
	}
	return -1;
}

// ���칺�������˳��ӳ���ϵ
vector<pair<int, int>> CloudResources::deal_serial_number(const vector<string>& vPurchase, const vector<pair<string, int>>& vEachDay_purchase)
{
	// ���룺vPurchase�����칺�������˳��  vEachDay_purchase�����칺���������¼
	// ��������칺�������˳��ӳ���ϵ
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

// ������ѵķ���������ǰ���������CPU���ڴ��5����С��
string CloudResources::find_optimal_server(const int& iCPU_demand, const int& iMemory_demand)
{   // ���룺iCPU_demand������CPU��Ŀ iMemory_demand�������ڴ��С
	// ���������ķ������ͺ�
	int iReal_CPU_demand = 3 * iCPU_demand;
	int iReal_Memory_demand = 3 * iMemory_demand;
	int iRelative_error = 2147483647;// ���������CPU�������ڴ��С��������
	string result;
	for (const auto& server : m_hsServers)
	{//���������ͺŵķ�����
		int iServerCPU_size = server.second[0];
		int iServerMemory_siz = server.second[1];
		int iError = abs(iReal_CPU_demand - iServerCPU_size) + abs(iReal_Memory_demand - iServerMemory_siz);
		if (iError < iRelative_error)
		{
			iRelative_error = iError;
			result = server.first;
		}
	}

	m_vPurchase_order.push_back(result);// ��¼���칺��˳��

	for (auto& server : m_vEachDay_purchase)
	{// ���������ѹ�������
		if (server.first == result)
		{// ���ͺŷ����������Ѿ������
			server.second++;// ���ͺŷ��������칺������+1
			return result;
		}
	}
	m_vEachDay_purchase.push_back({ result,1 });// // ���ͺŷ���������δ�����,���뵱�칺���¼
	//m_hsEachDay_purchase[result]++;
	return result;
}


#endif // !_ALG_H_

