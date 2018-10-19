#pragma once

class WebsocketServlet_impl : public acl::WebSocketServlet
{
public:
	WebsocketServlet_impl(acl::redis_client_cluster& cluster, size_t max_conns);
	~WebsocketServlet_impl(void);

	acl::session& get_session(void) const
	{
		return *session_;
	}

protected:
	// @override
	bool doUnknown(acl::HttpServletRequest&, acl::HttpServletResponse&);
	// @override
	bool doGet(acl::HttpServletRequest&, acl::HttpServletResponse&);
	// @override
	bool doPost(acl::HttpServletRequest&, acl::HttpServletResponse&);

	//for websocket
	/**
	 * @override
	 * websocket �ر���Ϣ�ص�
	 * @return {void}
	 */
	void onClose(void) {}

	/**
	 * @override
	 * websocket ping ��Ϣ�ص�.
	 * @param {const char *} buf ��Ϣ����
	 * @param {int} len ��Ϣ���ݳ���
	 * @return {bool} false �Ͽ����ӡ�
	 */
	bool onPing(const char *buf, unsigned long long len);

	/**
	 * @override
	 * websocket pong ��Ϣ�ص�.
	 * @param {const char *} buf ��Ϣ����
	 * @param {int} len ��Ϣ���ݳ���
	 * @return {bool} false �Ͽ����ӡ�
	 */
	bool onPong(const char *buf, unsigned long long len);

	/**
	 * @override
	 * websocket ping ��Ϣ�ص�.
	 * @param data{char *} �ص����ݻ�������
	 * @param len{unsigned long long}�ص����ݻ��������ȡ�
	 * @param text{bool } true Ϊ�ı����ݡ������� ���������ݡ�
	 * @return {bool} false �Ͽ����ӡ�
	 */
	bool onMessage(char *data, unsigned long long len, bool text);

private:
	acl::session* session_;
	int step_;
	acl::string filename_;
	acl::ofstream *file_;
	int filesize_;
	int current_filesize_;
};
