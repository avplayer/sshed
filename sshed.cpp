/*
 * sshed-c.cpp
  *
 *  Created on: 2018/6/29
 *      Author: yirenyiai
 *      Mail  : lushuwen_gz@foxmail.com
 */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <vector>
#include <string>
#include <algorithm>

#ifdef _DEBUG
#define DEBUG_INFO printf
#else
void empty_printf(const char *__restrict __format, ...){}
#define DEBUG_INFO empty_printf
#endif


struct ssh_config_data
{
	char m_host[1024];
	char m_host_name[1024];
	char m_user[1024];
	char m_identify_file[1024];
	int m_port;

	void printf_string(bool show_all = true)
	{
		if (show_all)
		{
			printf("host:[%s]\tHostName:[%s:%d]\tUser:[%s]\tIdentifyFile:[%s]\n",
					m_host, m_host_name, m_port, m_user, m_identify_file);
		}
		else
		{
			printf("host:[%s]\tHostName:[%s:%d]\n",
					m_host, m_host_name, m_port);
		}
	}

	void set_value(const char* key, const char* value)
	{
		char buf[56] = {0};

		const unsigned int key_len = strlen(key);
		const unsigned int val_len = strlen(value);
		for (unsigned int i(0); i < key_len && i < sizeof(buf); ++i)
			buf[i] = tolower(key[i]);


		if (0 == strcmp(buf, "host") && val_len < sizeof(m_host))  strcpy(m_host, value);
		else if (0 == strcmp(buf, "hostname") && val_len < sizeof(m_host_name)) strcpy(m_host_name, value);
		else if (0 == strcmp(buf, "port")) m_port = atoi(value);
		else if (0 == strcmp(buf, "user") && val_len < sizeof(m_user)) strcpy(m_user, value);
		else if (0 == strcmp(buf, "identityfile") && val_len < sizeof(m_identify_file)) strcpy(m_identify_file, value);
	}

	bool operator < (const ssh_config_data& other) const
	{
		return strcmp(m_host, other.m_host) <= 0;
	}
};

static std::vector<ssh_config_data> g_vec_data;


// parse
void parse_config(const char* config_path)
{
	if (!g_vec_data.empty())
		g_vec_data.clear();

	FILE* fp;
	long file_len = 0, r_len = 0;
	fp = fopen(config_path, "rb+");
	if (!fp) return ;

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	rewind(fp);

	char* file_buf = new char[file_len];
	r_len = fread(file_buf, 1, file_len, fp);

	// parse file ctx to struct
	int offset = 0, index = 0;
	char cache[10][56] = {0};
	for (int cur(0); cur < file_len && (r_len == file_len); ++cur)
	{
		switch(file_buf[cur])
		{
		case '\t':
		case '\r':
		case '\n':
		case ' ':
		{
			if (cur - offset > 1)
			{
				// key word
				strncpy(cache[index++], file_buf + offset, cur - offset);
				DEBUG_INFO("parse key word : [%s]\n", cache[index - 1]);
				offset = cur + 1;

				if (index == 10)
				{
					ssh_config_data data;
					data.set_value(cache[0], cache[1]);
					data.set_value(cache[2], cache[3]);
					data.set_value(cache[4], cache[5]);
					data.set_value(cache[6], cache[7]);
					data.set_value(cache[8], cache[9]);
					g_vec_data.push_back(data);

					for (int i(0); i < 10; ++i)
						memset(cache[i], 0, sizeof(cache[i]));

					index = 0;
				}
			}
			else
			{
				offset += 1;
			}
		}
			break;
		default:
			break;
		}
	}

	// handle the last
	if (index != 0)
	{
		ssh_config_data data;
		data.set_value(cache[0], cache[1]);
		data.set_value(cache[2], cache[3]);
		data.set_value(cache[4], cache[5]);
		data.set_value(cache[6], cache[7]);
		data.set_value(cache[8], cache[9]);
		g_vec_data.push_back(data);
		index = 0;
	}

	if (file_buf)
		delete [] file_buf;

	if (fp)
		fclose(fp);
}

void api_help()
{
	printf("AME:\n");
	printf("\tsshed-c - SSH config editor and hosts manager\r\n");

	printf("USAGE:\n");
	printf("\tsshed-c command [arguments...]\r\n");

	printf("VERSION:\n");
	printf("\t0.0.1(beta)\r\n");

	printf("AUTHOR:\n");
	printf("\tyirenyiai<lushuwen_gz@formail.com>\r\n");

	printf("COMMANDS\n");
	printf("\tshow\tShows host\n");
	printf("\tlist\tLists all hosts\n");
	printf("\tauto_complete\tSet auto complete of all hosts\n");

	printf("\tadd\tAdd hosts\n");
	printf("\tadds\tAdd hosts from config file, the config file line format\n");
	printf("\t\teg:Host:<host>,Port:<port>,User:<user>,IdentityFile:<identityfile>\n");

	printf("\tremove\tRemoves hosts, removes all when ssh host unname pass\n");
	printf("\trollback\troll back last operator\n");
	printf("\tclean_cache\tclean ssh cache\n");
	printf("\thelp\tthis message\n");
	printf("\r\n");
}

void api_show(const char* config_path)
{
	FILE* fp;
	long file_len = 0, r_len = 0;
	fp = fopen(config_path, "rb+");
	if (!fp) return ;

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	rewind(fp);

	char* file_buf = new char[file_len];
	r_len = fread(file_buf, 1, file_len, fp);

	if (r_len == file_len)
	{
		printf("%s", file_buf);
	}

	if (file_buf)
		delete [] file_buf;

	if (fp)
		fclose(fp);
}

void api_list()
{
	std::vector<ssh_config_data> vec_tmp = g_vec_data;
	std::stable_sort(vec_tmp.begin(), vec_tmp.end());

	printf("ssh config have %ld items\n", vec_tmp.size());
	for (unsigned int i(0); i < vec_tmp.size(); ++i)
		vec_tmp[i].printf_string(false);
}

void api_add()
{
	ssh_config_data data;
	printf("Host:");
	scanf("%s", data.m_host);
	// ckh unique
	bool is_unique = false;
	unsigned int i = 0;
	for (; i < g_vec_data.size() && !is_unique; ++i)
	{
		if (0 == strcmp(g_vec_data[i].m_host, data.m_host))
			is_unique = true;
	}

	if (!is_unique)
	{
		// continue other info
		printf("HostName:");
		scanf("%s", data.m_host_name);

		printf("Port:");
		scanf("%d", &data.m_port);

		printf("User:");
		scanf("%s", data.m_user);

		printf("IdentityFile:");
		scanf("%s", data.m_identify_file);

		g_vec_data.push_back(data);
	}
	else
	{
		printf("%s: is exit, current info:\n", g_vec_data[i].m_host);
		g_vec_data[i].printf_string();
	}
}


/*
 *
 * line format eg:
 * Host:<host>,Port:<port>,User:<user>,IdentityFile:<identityfile>
 * */
void api_adds(const char* adds_config_file)
{
	char line_ctx[1024] = {0};
	FILE* fp = fopen(adds_config_file, "rb+");

	while (fp && !feof(fp))
	{
		memset(line_ctx, 0, sizeof(line_ctx));
		fgets(line_ctx, sizeof(line_ctx), fp);  //读取一行

		// parse it
		int index = 0, offset = 0;
		char cache[10][1024] = {0};
		bool is_terminal = false;
		for (unsigned int cur(0); cur < sizeof(line_ctx) && !is_terminal; ++cur)
		{
			switch(line_ctx[cur])
			{
			case ',':
			case ':':
			case '=':
				strncpy(cache[index++], line_ctx + offset, cur - offset);
				DEBUG_INFO("adds parse keyword : [%s]\n", cache[index - 1]);
				offset = cur + 1;
				break;

			case '\n':
			{
				// set the last keyword
				if (cur - offset > 0)
				{
					strncpy(cache[index++], line_ctx + offset, cur - offset);
					offset = cur + 1;
				}

				ssh_config_data data;
				data.set_value(cache[0], cache[1]);
				data.set_value(cache[2], cache[3]);
				data.set_value(cache[4], cache[5]);
				data.set_value(cache[6], cache[7]);
				data.set_value(cache[8], cache[9]);

				bool is_exit = false;
				for (unsigned int i(0); i < g_vec_data.size() && !is_exit; ++i)
				{
					if (0 == strcmp(data.m_host, g_vec_data[i].m_host))
					{
						is_exit = true;
						break;
					}
				}

				if (!is_exit)
				{
					printf("parse line succes:\t");
					data.printf_string();
					g_vec_data.push_back(data);
				}
				else
				{
					printf("Host [%s] is exit, ingore this items, you have to change or delete old item and readd it.\n", data.m_host);
				}
				is_terminal = true;
			}
				break;
			case '\0':
				is_terminal = true;
				break;
			default:
				break;
			}
		}
	}

	if (fp)
		fclose(fp);
}


std::vector<std::string> api_cache_load(const char* cache_file_path, const char* ssh_key)
{
	// find the key ip
	char ssh_ip_formart[2][1024] = {0};
	for (unsigned i(0); i < g_vec_data.size(); ++i)
	{
		if (ssh_key && 0 == strcmp(g_vec_data[i].m_host, ssh_key))
		{
			::snprintf(ssh_ip_formart[0], sizeof(ssh_ip_formart[0]), "%s", g_vec_data[i].m_host_name);
			::snprintf(ssh_ip_formart[1], sizeof(ssh_ip_formart[1]), "[%s]:%d", g_vec_data[i].m_host_name, g_vec_data[i].m_port);
			break;
		}
	}

	// load file
	char line_ctx[2 * 1024] = {0};
	std::vector<std::string> vec_kline_ctx;

	FILE* fp = fopen(cache_file_path, "rb+");
	while (fp && !feof(fp))
	{
		fgets(line_ctx, 1024, fp);  //读取一行
		//  ssh_key formart [127.0.0.1]
		if (!ssh_key ||
				0 == strncmp(line_ctx, ssh_ip_formart[0], strlen(ssh_ip_formart[0])) ||
				0 == strncmp(line_ctx, ssh_ip_formart[1], strlen(ssh_ip_formart[1])))
		{
			printf("remove cache : %s\n", line_ctx);
			continue;
		}
		vec_kline_ctx.push_back(line_ctx);
	}

	if (fp)
		fclose(fp);
	return vec_kline_ctx;
}

void api_cache_save(const char* cache_file_path, const std::vector<std::string>& vec_line_ctx)
{
	FILE* fp = fopen(cache_file_path, "wb+");

	for (unsigned int i(0); i < vec_line_ctx.size() && fp; ++i)
		fwrite(vec_line_ctx[i].c_str(), vec_line_ctx[i].size(), 1, fp);

	if (fp)
		fclose(fp);
}

void api_update(const char* ssh_key)
{
	for (unsigned int i(0); i < g_vec_data.size(); ++i)
	{
		if (0 == strcmp(g_vec_data[i].m_host, ssh_key))
		{
			//
			printf("old:");
			g_vec_data[i].printf_string();

			// update it
			printf("Host:%s -> Host:", g_vec_data[i].m_host);
			scanf("%s", g_vec_data[i].m_host);

			printf("HostName:%s -> HostName:", g_vec_data[i].m_host_name);
			scanf("%s", g_vec_data[i].m_host_name);

			printf("Port:%d -> Port:", g_vec_data[i].m_port);
			scanf("%d", &g_vec_data[i].m_port);

			printf("User:%s -> User:", g_vec_data[i].m_user);
			scanf("%s", g_vec_data[i].m_user);

			printf("Identify file:%s -> Identify file:", g_vec_data[i].m_identify_file);
			scanf("%s", g_vec_data[i].m_identify_file);

			printf("new:");
			g_vec_data[i].printf_string();
			break;
		}
	}

}

void api_remove(const char* ssh_key)
{
	std::vector<ssh_config_data> vec_tmp;
	for (unsigned int i(0); i < g_vec_data.size() && ssh_key; ++i)
	{
		if (0 == strcmp("clean_all_ssh_config", ssh_key) ||
				0 == strcmp(g_vec_data[i].m_host, ssh_key))
		{
			continue;
		}

		vec_tmp.push_back(g_vec_data[i]);
	}

	printf("origin %ld ssh items\n", g_vec_data.size());
	printf("remove %ld ssh items\n", g_vec_data.size() - vec_tmp.size());
	printf("havae  %ld ssh items now\n", vec_tmp.size());
	vec_tmp.swap(g_vec_data);
}

void api_save(const char* config_path)
{
	FILE* fp;
	fp = fopen(config_path, "w+");
	if (!fp) return ;

	char write_buf[1024] = {0};
	int write_len = 0;
	for (unsigned int i(0); i < g_vec_data.size(); ++i)
	{
		write_len = ::snprintf(write_buf, sizeof(write_buf), "Host %s\n", g_vec_data[i].m_host);
		fwrite(write_buf, write_len, 1, fp);

		write_len = ::snprintf(write_buf, sizeof(write_buf), "\tHostName %s\n", g_vec_data[i].m_host_name);
		fwrite(write_buf, write_len, 1, fp);

		write_len = ::snprintf(write_buf, sizeof(write_buf), "\tPort %d\n", g_vec_data[i].m_port);
		fwrite(write_buf, write_len, 1, fp);

		write_len = ::snprintf(write_buf, sizeof(write_buf), "\tUser %s\n", g_vec_data[i].m_user);
		fwrite(write_buf, write_len, 1, fp);

		write_len = ::snprintf(write_buf, sizeof(write_buf), "\tIdentityFile %s\n", g_vec_data[i].m_identify_file);
		fwrite(write_buf, write_len, 1, fp);

		write_len = ::snprintf(write_buf, sizeof(write_buf), "\r\n");
		fwrite(write_buf, write_len, 1, fp);
	}

	if (fp)
		fclose(fp);
}

void api_auto_complete()
{
	std::string auto_complete_str = "complete -W '";
	for (unsigned int i(0); i < g_vec_data.size(); ++i)
	{
		auto_complete_str += g_vec_data[i].m_host;
		auto_complete_str += " ";
	}
	auto_complete_str += "' ssh";

	printf("the ssh auto complete scripe:\n");
	printf("%s\n", auto_complete_str.c_str());
}


void api_back_up(const char* config_path)
{
	char* file_buf = NULL;
	long r_len = 0;
	FILE* fp_origin = fopen(config_path, "rb+");

	FILE* fp_dest = NULL;
	char config_backup_path[1024] = {0};
	const unsigned int config_path_len = strlen(config_path);
	if (config_path_len < sizeof(config_backup_path))
	{
		strcat(config_backup_path, config_path);
		strcat(config_backup_path, ".bk");
		fp_dest = fopen(config_backup_path, "wb+");
	}




	if (fp_origin)
	{
		fseek(fp_origin, 0, SEEK_END);
		const long file_len = ftell(fp_origin);
		rewind(fp_origin);

		file_buf = new char[file_len];
		r_len = fread(file_buf, 1, file_len, fp_origin);

		// write to back up
		if (r_len == file_len && fp_dest)
			fwrite(file_buf, file_len, 1, fp_dest);
	}


	if (file_buf)
		delete [] file_buf;

	if (fp_dest)
		fclose(fp_dest);

	if (fp_origin)
		fclose(fp_origin);
}

bool api_ssh_connect(const char* ssh_key)
{
	bool is_ssh_connect = false;
	unsigned int i = 0;
	for (; i < g_vec_data.size() && is_ssh_connect; ++i)
	{
		if (0 == strcmp(g_vec_data[i].m_host, ssh_key))
		{
			char cmd[1024] = {0};
			::snprintf(cmd, sizeof(cmd), "ssh %s", ssh_key);
			system(cmd);
			is_ssh_connect = true;
			break;
		}
	}

	return is_ssh_connect;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		api_help();
		return -1;// help info
	}

	bool is_save = false;
	char user_ssh_config_file_path[1024] = {0};
	const char* home_dir = getenv("HOME");
	strcat(user_ssh_config_file_path, home_dir);
	strcat(user_ssh_config_file_path + strlen(home_dir), "/.ssh/config");

	parse_config(user_ssh_config_file_path);
	const char* cmd = argv[1];
	DEBUG_INFO("cmd : %s\n", cmd);

	if (0 == strcmp(cmd, "list"))
	{
		api_list();
	}
	else if (0 == strcmp(cmd, "auto_complete"))
	{
		// set auto complete
		api_auto_complete();
	}
	else if (0 == strcmp(cmd, "show"))
	{
		api_show(user_ssh_config_file_path);
	}
	else if (0 == strcmp(cmd, "add"))
	{
		api_add();
		is_save = true;
	}
	else if (0 == strcmp(cmd, "adds"))
	{
		if (argc > 2)
		{
			const char* adds_config_paht = argv[2];
			api_adds(adds_config_paht);
			is_save = true;
		}
		else
		{
			printf("adds param miss.you must call is like : sshed-c adds [file]\n");;
		}
	}
	else if (0 == strcmp(cmd, "remove"))
	{
		const char* ssh_key = NULL;

		if (argc > 2)
			ssh_key = argv[2];

		api_remove(ssh_key);
		is_save = true;
	}
	else if (0 == strcmp(cmd, "update"))
	{
		const char* ssh_key = NULL;

		if (argc > 2)
		{
			ssh_key = argv[2];
			api_update(ssh_key);
			is_save = true;
		}
		else
		{
			printf("update ssh key param miss!\n");
		}
	}
	else if (0 == strcmp(cmd, "clean_cache"))
	{
		const char* ssh_key = NULL;
		if (argc > 2)
			ssh_key = argv[2];

		char cache_file_path[1024] = {0};

		const char* home_path = getenv("HOME");
		const unsigned int home_path_len = strlen(home_path);

		const char* ssh_path = "/.ssh/known_hosts";
		const unsigned int ssh_path_len = strlen(ssh_path);

		if (home_path_len + ssh_path_len)
		{
			strncat(cache_file_path, home_path, home_path_len);
			strncat(cache_file_path + home_path_len, ssh_path, ssh_path_len);
		}

		std::vector<std::string> vec_line_ctx = api_cache_load(cache_file_path, ssh_key);
		api_cache_save(cache_file_path, vec_line_ctx);

		printf("current cache %ld items\n", vec_line_ctx.size());
	}
	else if (0 == strcmp(cmd, "rollback"))
	{
		char user_ssh_config_backup_file_path[1024] = {0};

		if (strlen(user_ssh_config_file_path) < sizeof(user_ssh_config_backup_file_path))
		{
			strcat(user_ssh_config_backup_file_path, user_ssh_config_file_path);
			strcat(user_ssh_config_backup_file_path, ".bk");

			parse_config(user_ssh_config_backup_file_path);
			is_save = true;
		}
	}
	else if (0 == strcmp(cmd, "help"))
	{
		api_help();
	}
	else
	{
		// 传入参数如果是某个ssh key, 则直接连入
		const char* ssh_key = cmd;
		if (!api_ssh_connect(ssh_key))
		{
			printf("param error !!\n");
			api_help();
		}
	}

	if (is_save)
	{
		api_back_up(user_ssh_config_file_path);
		api_save(user_ssh_config_file_path);
	}
	return 0;
}

