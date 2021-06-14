/*

  ding network driver (c) Rishi Tharun
  ------------------------------------
    > Header for ding and ding dummy network drivers.
    > Run 'make' from the project root

  * Author: Rishi Tharun <vrishitharunj@gmail.com>
  * License: GPLv3

*/

#include<linux/module.h>
#include<linux/netdevice.h>
#include<linux/inetdevice.h>
#include<linux/if_arp.h>

#include"dingcfgs.h" /* Will be generated, based on the config values */

#define IFNAMSIZ 16
#define LOOPBACK (htonl(0x7f000001))

/* MODULE INFO */
#define DRIVER_NAME "ding"
#define DRIVER_DUMMY_NAME "dingdummy"
#define DRIVER_AUTHOR "Rishi Tharun"
#define DRIVER_LICENSE "GPL"
#define DRIVER_DESCRIPTION "ding-driver, a virtual network driver for real world network simulation"

#ifndef DLL
#define ETHER
#endif
#ifndef MTU
#define MTU 1500
#endif
#ifdef ETHER
#define HEADER_LEN 14
#endif
#ifdef IEEE_802_11
#define HEADER_LEN 24
#endif

/* COMMON CONSTS */
#define HW_ADDR_LEN 6
#define TXRX_BUFF_SIZE 100

static struct net_device* ding;

u32 get_mask(u32 inet);
u32 get_gateway(u32 inet);
u32 get_broadcast(u32 inet, u32 mask);
u32 str_to_inet(const char * inet_str);
int setup_inet(struct net_device * ding);
int ding_send(struct sk_buff* skb, struct net_device* ding);
void str_to_hwaddr(const char * hwa_str, unsigned char hwa[]);
void set_hw_addr(struct net_device * dev, unsigned char hw_addr[]);
void set_inet_addr(struct net_device * dev, u32 inet_addr);
void set_inet_mask(struct net_device * dev, u32 inet_addr);
void set_inet_broadcast(struct net_device * dev, u32 inet_addr);
void set_inet_addr_by_name(char * net_dev_name, u32 inet_addr);
void set_inet_mask_by_name(char * net_dev_name, u32 inet_addr);
void set_inet_broadcast_by_name(char * net_dev_name, u32 inet_addr);
struct net_device * get_net_dev(unsigned char * dev_name);

struct dll_head
{
  #ifdef IEEE_802_11
    unsigned short frame_control;
    unsigned short duration;
  #endif

    unsigned char dest_mac[HW_ADDR_LEN];
    unsigned char src_mac[HW_ADDR_LEN];

  #ifdef ETHER
    unsigned short type;
  #endif

  #ifdef IEEE_802_11
    unsigned char final_dest_mac[HW_ADDR_LEN];
    unsigned short sequence;
  #endif
};

static int ding_open(struct net_device* ding)
{
  printk(KERN_INFO"ding driver open\n");
  netif_start_queue(ding);
  return 0;
}

static int ding_stop(struct net_device* ding)
{
  printk(KERN_INFO "ding driver close\n");
  netif_stop_queue(ding);
  return 0;
}

static int ding_change_mac(struct net_device *ding, void *new_mac)
{
  int i=0;
  struct sockaddr * sa = new_mac;

  if (netif_running(ding)) return -EBUSY;
  else {;}

  for(;i<HW_ADDR_LEN;i++) ding->dev_addr[i] = sa->sa_data[i];
  return 0;
}

static int ding_mac(struct sk_buff *skb, struct net_device *ding, unsigned short type, const void *daddr, const void *saddr, unsigned int len)
{
  int i;
  struct dll_head * dllhdr;
  dllhdr = skb_push(skb, HEADER_LEN);
  for(i=0;i<HW_ADDR_LEN;i++)  { dllhdr->src_mac[i] = ding->dev_addr[i]; dllhdr->dest_mac[i] = *((unsigned char*) daddr+i); }
  #ifdef ETHER
    dllhdr->type = htons(type);
  #endif
  return HEADER_LEN;
}

static struct net_device_ops ding_ops =  {
                                          .ndo_open = &ding_open,
                                          .ndo_stop = &ding_stop,
                                          .ndo_start_xmit = &ding_send,
                                          .ndo_set_mac_address = &ding_change_mac
                                        };

static struct header_ops ding_dll_header =  {
                                              .create = &ding_mac,
                                           };

static void setup_ding(struct net_device * ding)
{
  printk(KERN_INFO"Setting up ding driver\n");
  ding->netdev_ops = &ding_ops;
  ding->header_ops = &ding_dll_header;

  ding->hard_header_len = HEADER_LEN;
  ding->min_header_len = 0;
  ding->addr_len = HW_ADDR_LEN;
  ding->mtu = MTU;
  ding->max_mtu = MTU;
  ding->min_mtu = MTU;
  ding->tx_queue_len = TXRX_BUFF_SIZE;
  ding->dev_addr[HW_ADDR_LEN] = 0;
  ding->flags = IFF_UP | IFF_RUNNING | IFF_PROMISC | IFF_DYNAMIC | IFF_BROADCAST;

  #ifdef ETHER
    ding->type = ARPHRD_ETHER;
  #endif

  #ifdef IEEE_802_11
    ding->type = ARPHRD_IEEE80211;
  #endif

  return;
}

static int init_ding(void)
{
  int err;
  u32 inet;
  struct in_ifaddr * ref_ifa;
  unsigned char hw_addr[HW_ADDR_LEN] = {0};

  printk(KERN_INFO"Initialization of ding driver\n");

#ifdef DING
  ding = alloc_netdev(0, DRIVER_NAME, NET_NAME_UNKNOWN, setup_ding);
#endif

#ifdef DING_DUMMY
  ding = alloc_netdev(0, DRIVER_DUMMY_NAME, NET_NAME_UNKNOWN, setup_ding);
#endif

  if(!ding)
  {
    printk(KERN_ERR"Memory allocation for ding driver Failed !\n");
    return -ENOMEM;
  }
  else
  {
    printk(KERN_INFO"Memory allocated for ding driver at <%p>\n", ding);
  }

  err = register_netdev(ding);
  if(err)
  {
    printk(KERN_ERR"ding driver registration failed with errno <%d> !\n", -err);
    free_netdev(ding);
    printk(KERN_INFO"Memory allocated to ding driver freed\n");
    return err;
  }
  else printk(KERN_INFO"ding driver successfully registered\n");
  
  err = setup_inet(ding);
  if(err < 0)
  {
    printk(KERN_ERR"Error setting up inet for ding driver\n");
    unregister_netdev(ding);
    printk(KERN_INFO"ding driver unregistered\n");
    free_netdev(ding);
    printk(KERN_INFO"Memory allocated to ding driver freed\n");
    return err;
  }
  else {;}
  ref_ifa = ding->ip_ptr->ifa_list;

#ifdef INET

  #ifdef DING
    inet = str_to_inet(INET);
  #endif

  #ifdef DING_DUMMY
    inet = get_gateway(str_to_inet(INET));
  #endif

#else
  inet = LOOPBACK;
#endif

  set_inet_addr(ding, inet);

#ifdef MASK
  set_inet_mask(ding, str_to_inet(MASK));

#else
  set_inet_mask(ding, get_mask(inet));
#endif

#ifdef HW_ADDR
  str_to_hwaddr(HW_ADDR, hw_addr);
#endif
  set_hw_addr(ding, hw_addr);

  set_inet_broadcast(ding, get_broadcast(ref_ifa->ifa_local, ref_ifa->ifa_mask));

  return 0;
}

static void deinit_ding(void)
{
  printk(KERN_INFO"De-initializing ding driver\n");
  unregister_netdev(ding);
  printk(KERN_INFO"ding driver unregistered\n");
  free_netdev(ding);
  printk(KERN_INFO"Memory allocated to ding driver freed\n");
}

