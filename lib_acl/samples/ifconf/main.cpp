#include "lib_acl.h"

static void test1(void)
{
        ACL_IFCONF *ifconf;	/* ������ѯ������� */
        ACL_IFADDR *ifaddr;	/* ÿ��������Ϣ���� */
        ACL_ITER iter;		/* �������� */

	/* ��ѯ��������������Ϣ */
	ifconf = acl_get_ifaddrs();

	if (ifconf == NULL) {
		printf("acl_get_ifaddrs error: %s\r\n", acl_last_serror());
		return;
	}

	printf("acl_get_ifaddrs:\r\n");

	/* ����������������Ϣ */
	acl_foreach(iter, ifconf) {
		ifaddr = (ACL_IFADDR*) iter.data;
		printf(">>>name=%s, addr=%s, type=%s\r\n",
			ifaddr->name, ifaddr->addr,
			ifaddr->saddr.sa.sa_family == AF_INET ?
			 "AF_INET" : (ifaddr->saddr.sa.sa_family == AF_INET6 ?
				 "AF_INET6" : "unknown"));
	}

	/* �ͷŲ�ѯ��� */
	acl_free_ifaddrs(ifconf);
}

static void test2(const char *pattern)
{
        ACL_ITER    iter;		/* �������� */
	ACL_IFCONF *ifconf;

	ifconf = acl_ifconf_search(pattern);
	if (ifconf == NULL) {
		printf("acl_ifconf_search error\r\n");
		return;
	}

	printf("pattern=%s\r\n", pattern);
	acl_foreach(iter, ifconf) {
		const ACL_IFADDR *ifaddr = (const ACL_IFADDR *) iter.data;
		const char *type;
		if (ifaddr->saddr.sa.sa_family == AF_INET)
			type = "AF_INET";
		else if (ifaddr->saddr.sa.sa_family == AF_INET6)
			type = "AF_INET6";
		else
			type = "unknown";
		printf(">>>name=%s, addr=%s, type=%s\r\n",
			ifaddr->name, ifaddr->addr, type);
	}

	acl_free_ifaddrs(ifconf);
}

int main(void)
{
	const char *patterns[] = {
		"8190",
		"#8190",
		":8190",
		"*#8190",
		"*.*.*.*:8190",
		"0.0.0.0:8190",
		"127.*.*.*:8290, 192.168.*.*:8291, 172.16.*.*.:8292, 172.17.*.*.:8293, /unix_server@unix",
		"127.*.*.*:8290, 0.0.0.0:8191, *.*.*.*:8292",
		":8191, #8192, *:8193, *#8194",
		NULL,
	};
	int i;

	test1();

	for (i = 0; patterns[i]; i++) {
		if (i >= 0) {
			printf("\r\n-----------------------------------\r\n");
			test2(patterns[i]);
		}
	}

	return 0;
}
