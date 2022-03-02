#include <sys/types.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <ctype.h>
#include <netinet/ether.h>
#include <unistd.h>

#include "util.h"


int main(void)
{
    char mac[256] = {};
    get_user_key(mac);
    printf("\n\n %s \n", mac);

    int state = get_network_status("eth0");
    printf("\n\n get_network_status : %d \n\n", state);
    
}

int get_network_status(char *network)
{
    struct ifaddrs *addrs, *tmp;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
        {
            sa = (struct sockaddr_in *)tmp->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            int compare_name = strcmp(tmp->ifa_name, network);
            if (compare_name == 0)
            {
                printf("Iface : %s\tAddress : %s\n", tmp->ifa_name, addr);
                return 1;
            }
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    return 0;
}

void get_user_key(char *mac)
{
    int sock;
    struct ifreq ifr;
    char mac_adr[18] = {
        0,
    };

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        dp(4, "socket");
        return 0;
    }

    strcpy(ifr.ifr_name, "eth0");

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
    {
        dp(4, "ioctl() - get mac");
        close(sock);
        return 0;
    }

    // convert format ex) 00:00:00:00:00:00
    convrt_mac(ether_ntoa((struct ether_addr *)(ifr.ifr_hwaddr.sa_data)), mac_adr, sizeof(mac_adr) - 1);

    strcpy(mac, mac_adr);
    close(sock);
}

void convrt_mac(const char *data, char *cvrt_str, int sz)

{
    char buf[128] = {
        0,
    };
    char t_buf[8];
    char *stp = strtok((char *)data, ":");

    int temp = 0;

    do
    {
        memset(t_buf, 0, sizeof(t_buf));
        sscanf(stp, "%x", &temp);
        snprintf(t_buf, sizeof(t_buf) - 1, "%02X", temp);
        strncat(buf, t_buf, sizeof(buf) - 1);
        strncat(buf, ":", sizeof(buf) - 1);
    } while ((stp = strtok(NULL, ":")) != NULL);

    buf[strlen(buf) - 1] = '\0';
    strncpy(cvrt_str, buf, sz);
}

void get_cpu_usage() {
     char loadDataBuf[ONE_LINE] = {0};
  char cpuId[4] = {0};
  
  int jiffies[2][JIFFIES_NUM] = {0}, totalJiffies;
  int diffJiffies[JIFFIES_NUM];
  int idx;
 
  FILE* statFile;
    while(1) {
    statFile = fopen("/proc/stat", "r");
    fscanf(statFile, "%s %d %d %d %d",
      cpuId, &jiffies[PRESENT][USER], &jiffies[PRESENT][USER_NICE],
   &jiffies[PRESENT][SYSTEM], &jiffies[PRESENT][IDLE]);
  
    for(idx = 0, totalJiffies = 0; idx < JIFFIES_NUM; ++idx){
      diffJiffies[idx] = jiffies[PRESENT][idx] - jiffies[PAST][idx];
      totalJiffies = totalJiffies + diffJiffies[idx];
    }
    
   printf("Cpu usage : %f%%\n",
      100.0*(1.0-(diffJiffies[IDLE] / (double)totalJiffies)));
        
    memcpy(jiffies[PAST], jiffies[PRESENT], sizeof(int)*JIFFIES_NUM);
 
    fclose(statFile);
    sleep(1);
    }
    // return cpu_usage;
}


float get_gpu_temp()
{
    float gputemp = 0;
    char *gpu;
    gpu = (char *)malloc(sizeof(char) * 6);

    FILE *fgputemp = fopen("/sys/devices/virtual/thermal/thermal_zone2/temp", "r");
    if (!fgputemp)
    {
        printf("Something went wrong\n");
        return 0;
    }

    gputemp = atoi(fgets(gpu, 6, fgputemp)) / 1000;

    fclose(fgputemp);
    return gputemp;
}

float get_cpu_temp()
{

    float cputemp = 0;

    char *cpu;
    cpu = (char *)malloc(sizeof(char) * 6);

    FILE *fcputemp = fopen("/sys/devices/virtual/thermal/thermal_zone1/temp", "r");
    if (!fcputemp)
    {
        printf("Something went wrong\n");
        exit(EXIT_FAILURE);
    }
    cputemp = atoi(fgets(cpu, 6, fcputemp)) / 1000;
    fclose(fcputemp);
    return cputemp;
}



int get_send_byte()
{
    FILE *fp = fopen("/proc/net/dev", "r");
    char buf[200], ifname[20];
    unsigned long int r_bytes, t_bytes, r_packets, t_packets;

    int sum = 0;
    int i = 0;
    // skip first two lines
    for (int i = 0; i < 2; i++)
    {
        fgets(buf, 200, fp);
    }

    while (fgets(buf, 200, fp))
    {

        sscanf(buf, "%[^:]: %lu %lu %*lu %*lu %*lu %*lu %*lu %*lu %lu %lu",
               ifname, &r_bytes, &r_packets, &t_bytes, &t_packets);

        if (strcmp(ifname, "    lo") == 0)
            continue;

        // printf("%s: rbytes: %lu rpackets: %lu tbytes: %lu tpackets: %lu\n",
        //        ifname, r_bytes, r_packets, t_bytes, t_packets);

        sum += t_bytes;
        i += 1;
    }
    fclose(fp);
    return sum;
}