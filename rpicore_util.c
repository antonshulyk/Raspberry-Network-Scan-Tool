
#include "syshead3.h"

#include "rpicore_global.h"

#include "rpicore_util.h"

#define KEYWORD_STR		"rtt min/avg/max/mdev = "

/*
 * string operations
 */

int rpi_strcpy(char *dst, size_t max_dst_len, const char *src)
{
	int len;
	
	if (strlen(src) == 0)
		return 0;
	
	len = strlen(src) < max_dst_len ? strlen(src) : max_dst_len;
	
	strncpy(dst, src, len);
	dst[len] = '\0';
	
	return len;
}

int rpi_strncpy(char *dst, size_t max_dst_len, const char *src, size_t copy_len)
{
	int len;
	
	if (strlen(src) == 0)
		return 0;
	
	if (copy_len > strlen(src))
		copy_len = strlen(src);
	
	len = copy_len < max_dst_len ? copy_len : max_dst_len;
	
	strncpy(dst, src, len);
	dst[len] = '\0';
	
	return len;
}

/*
 * copy file
 */

void
copy_file(const char *old_path, const char *new_path)
{
	FILE *old_fp, *new_fp;
	unsigned char *buf;
	
	struct stat st;
	
	/* get file size */
	if (stat(old_path, &st) < 0)
		return;
	
	if (!S_ISREG(st.st_mode) || st.st_size < 1)
		return;
	
	buf = (unsigned char *) malloc(st.st_size);
	if (!buf)
		return;
	
	/* open files for read/write */
	old_fp = fopen(old_path, "r");
	new_fp = fopen(new_path, "w");
	if (!old_fp || !new_fp)
	{
		free(buf);
		return;
	}
	
	/* get buffer from old fp */
	fread(buf, 1, st.st_size, old_fp);
	fwrite(buf, 1, st.st_size, new_fp);
	
	fclose(old_fp);
	fclose(new_fp);
	
	free(buf);
}

/*
 * check if file is exist

 @param is_dir		if directory is specified, then 1, otherwise 0
 @return				if success, then 1, otherwise 0
*/

int check_exist_file(const char *file_path, int is_dir)
{
	struct stat st;

	// get status of file
	if (stat(file_path, &st) != 0)
		return 0;

	// get type
	if (is_dir && S_ISDIR(st.st_mode))
		return 1;
	else if (!is_dir && S_ISREG(st.st_mode))
		return 1;

	return 0;
}


/* check network status */
int
ping_network(const char *url)
{
	int sock;

	struct sockaddr_in google_addr;

	struct hostent *googleHost;
	
	int ret = 0;
	
	/* get IP address of google web site */
	googleHost = gethostbyname(url);
	if (!googleHost)
		return 0;
	
	if (googleHost->h_addrtype != AF_INET)
		return 0;
	
	/* create socket for checking Internet connection */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		exit(-1);
	
	/* set address object for google site */
	memset(&google_addr, 0, sizeof(google_addr));
	
	google_addr.sin_family = AF_INET;
	google_addr.sin_addr.s_addr = *(unsigned long *) googleHost->h_addr_list[0];
	google_addr.sin_port = htons(80);
	
	/* connect to google web server */
	if (connect(sock, (struct sockaddr *) &google_addr, sizeof(struct sockaddr_in)) == 0)
		ret = 1;
	
	rpicore_close_socket(sock);
	
	return ret;
}

int run_sys_call(const char *cmd)
{
    int res;
    res = system(cmd);
    if ( WEXITSTATUS(res) != 0 ) {
//                syslog(LOG_CRIT," System call failed.\n");
//                syslog(LOG_CRIT," %s\n",buffer);
    }
    return res;
}

int get_cmd_output(const char *cmd, char *out, size_t out_size)
{
    FILE *fp;
    char buf[1024];
    size_t buf_size = 0;

    // open command for reading
    fp = popen(cmd, "r");
    if (!fp)
	return -1;

    // read output line
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
	// set output buffer
	snprintf(&out[strlen(out)], out_size - buf_size, "%s", buf);

	// get buffer size of output line
	buf_size += strlen(buf);

	// check buffer size is greate than output buffer size
	if (buf_size >= out_size)
	    break;
    }

    // close pipe
    pclose(fp);

    return 0;
}

/* get uuid from uuid file */
int get_uuid_from_file(const char *filepath, char *out, size_t out_size)
{
    FILE *fp;
    char buf[1024];
    size_t buf_size = 0;

    // open file for reading
    fp = fopen(filepath, "r");
    if (!fp)
	return -1;

    // read output line
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
	// set output buffer
	snprintf(&out[strlen(out)], out_size - buf_size, "%s", buf);

	// get buffer size of output line
	buf_size += strlen(buf);

	// check buffer size is greate than output buffer size
	if (buf_size >= out_size)
	    break;
    }

    // close pipe
    pclose(fp);

    return 0;
}

/* get ping result */

float get_ping_result (const char *user_input)
{
	const char *p;
	float min, avg, max, mdev;

	p = strstr(user_input, KEYWORD_STR);
	if (p)
	{

	    p = p + strlen(KEYWORD_STR);

	    sscanf(p, "%f/%f/%f/%f ms", &min, &avg, &max, &mdev);

	    fprintf(stderr, "%f, %f, %f, %f", min, avg, max, mdev);
	}

	return avg;
    
}


const char*
netmasks[MAX_IPADDR_LEN + 1] =
{
        "128.0.0.0",
        "192.0.0.0",
        "224.0.0.0",
        "240.0.0.0",
        "248.0.0.0",
        "252.0.0.0",
        "254.0.0.0",
        "255.0.0.0",
        "255.128.0.0",
        "255.192.0.0",
        "255.224.0.0",
        "255.240.0.0",
        "255.248.0.0",
        "255.252.0.0",
        "255.254.0.0",
        "255.255.0.0",
        "255.255.128.0",
        "255.255.192.0",
        "255.255.224.0",
        "255.255.240.0",
        "255.255.248.0",
        "255.255.252.0",
        "255.255.254.0",
        "255.255.255.0",
        "255.255.255.128",
        "255.255.255.192",
        "255.255.255.224",
        "255.255.255.240",
        "255.255.255.248",
        "255.255.255.252",
        "255.255.255.254",
        "255.255.255.255",
        NULL,
};
int get_cdir_from_netmask(char *netmask)
{
    int i = 0;

    while (netmasks[i] != NULL)
    {
	if (strcmp(netmask, netmasks[i]) == 0)
	{
	    return i + 1;
	}

	i ++;
    }
}

int get_addr_info(const char *ifname, char *ip_addr, size_t ip_addr_s, int *cidr)
{
    int fd;
    struct ifreq ifr;
    char  netmask[64];

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFADDR, &ifr);
    snprintf(ip_addr, ip_addr_s, "%s", inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr));

    ioctl(fd, SIOCGIFNETMASK, &ifr);
    snprintf(netmask, sizeof(netmask), "%s", inet_ntoa(((struct sockaddr_in *) &ifr.ifr_netmask)->sin_addr));
    
    // get cidr

    *cidr = get_cdir_from_netmask(netmask);
    close(fd);

    return 0;
}

