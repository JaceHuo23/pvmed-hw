#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <openssl/conf.h>
#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#ifdef _MSC_VER
#include <intrin.h>
#include <Windows.h>
#include <Iphlpapi.h>
#else
#include <cpuid.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <ctime>
using namespace std;


#pragma  comment(lib, "wsock32.lib")
#pragma  comment(lib, "iphlpapi.lib")

#ifdef _DEBUG
#pragma  comment(lib, "VC/libcrypto64MDd.lib")
#pragma  comment(lib, "VC/libssl64MDd.lib")
#else
#pragma  comment(lib, "VC/libcrypto64MD.lib")
#pragma  comment(lib, "VC/libssl64MD.lib")
#endif // _DEBUG



std::string cmdPopen(const std::string& cmdLine) {
	char buffer[1024] = { '\0' };
	FILE* pf = NULL;
	pf = _popen(cmdLine.c_str(), "r");
	if (NULL == pf) {
		printf("open pipe failed\n");
		return std::string("");
	}
	std::string ret;
	while (fgets(buffer, sizeof(buffer), pf)) {
			ret += buffer;
	}
	_pclose(pf);

	return ret;
}



string get_cpuid()
{
	std::uint32_t reg[4];
#ifdef _MSC_VER
	__cpuid(reinterpret_cast<int*>(reg), 1);
#else
	__cpuid(1, reg[0], reg[1], reg[2], reg[3]);
#endif
	unsigned char d[sizeof(reg[0])];
	char s[sizeof(reg[0]) * 2 + 1] = {};
	std::memcpy(d, reg, sizeof(d));
	for (std::size_t i = 0; i < sizeof(d); ++i)
	{
		sprintf(s + i * 2, "%02x", d[i]);
	}
	return s;
}


// get first serial number of disk
string getdiskserialid()
{
	string s_result,s_tmp;
	s_result = cmdPopen("wmic diskdrive where index=0 get serialnumber");
	s_tmp = s_result.substr(s_result.find_first_of('\n')+1,s_result.length()-s_result.find_first_of('\n'));
	return s_tmp.substr(0,s_tmp.find_first_of('\r'));
}

// get all serial number of disk
vector <string> get_disk_serial_ids()
{
	vector <string> hds;
	string s_result,s_tmp;
	s_result = cmdPopen("wmic diskdrive get serialnumber");
	// cout<<s_result;
	s_tmp = s_result.substr(s_result.find_first_of('\n')+1,s_result.length()-s_result.find_first_of('\n'));
	while(s_tmp.length() > 0){
		string hd=(s_tmp.substr(0,s_tmp.find_first_of('\r')));
		if(hd.length() > 1)
			hds.push_back(hd);
		s_tmp = s_tmp.substr(s_tmp.find_first_of('\n')+1,s_tmp.length()-s_tmp.find_first_of('\n'));
	}
	return hds;
}

vector <string> get_mac_list()
{
	vector <string> mac_addrs;
	string s_result,s_tmp;
	s_result = cmdPopen("wmic nic where PhysicalAdapter=TRUE get MACAddress");
	// cout<<s_result;
	s_tmp = s_result.substr(s_result.find_first_of('\n')+1,s_result.length()-s_result.find_first_of('\n'));
	while(s_tmp.length() > 0){
		string mac_addr=(s_tmp.substr(0,s_tmp.find_first_of('\r'))).substr(0,17);
		transform(mac_addr.begin(), mac_addr.end(), mac_addr.begin(), ::tolower);
		if(mac_addr.find_first_of(":") != string::npos)
			mac_addrs.push_back(mac_addr);
		s_tmp = s_tmp.substr(s_tmp.find_first_of('\n')+1,s_tmp.length()-s_tmp.find_first_of('\n'));
	}
	return mac_addrs;
}

int main(int argc, char* argv[])
{
	vector <string> vs_mac = get_mac_list();
	string s_cpuid = get_cpuid();
	string s_diskserialid = getdiskserialid();
	vector <string> vs_hds= get_disk_serial_ids();
	cout<<"cpu id:"<<s_cpuid<<endl;
	cout<<"disk serial id:"<<s_diskserialid<<endl;
	//cout<<"license.conf md5:"<<get_confmd5()<<endl;
	//cout<<"active code:"<<get_active_code()<<endl;
	cout<<"mac address list:"<<endl;
	for(int i = 0; i < vs_mac.size(); i++)
		cout<<vs_mac[i]<<endl;
	cout<<"hard disk serial id list:"<<endl;
	for(int i = 0; i < vs_hds.size(); i++)
		cout<<"\""+vs_hds[i]+"\""<<endl;
	
	system("pause");
    return 0;
}