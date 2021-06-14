/*

  ding network driver (c) Rishi Tharun
  ------------------------------------
    > Source for ding driver support functions.
    > Run 'make' from the project root

  * Author: Rishi Tharun <vrishitharunj@gmail.com>
  * License: GPLv3

*/

#include<linux/netdevice.h>
#include<linux/inetdevice.h>

#define HW_ADDR_LEN 6

unsigned char stringsEqual(const char* str1, const char* str2);

enum
{
  False = 0,
  True = 1
};

enum
{
  LOCAL_ADDRESS = 0,
  SUBNET_MASK = 1,
  BROADCAST_ADDRESS = 2
};

struct net_device * get_net_dev(unsigned char * dev_name)
{
  /*
      caller must
        * hold rtnl_lock() to update
        * hold rcu_read_lock() / read_lock(&dev_base_lock) to read
  */

  struct net_device * dev = NULL;
  dev = first_net_device(&init_net);
  while (dev != NULL)
  {
    if (stringsEqual(dev->name, dev_name)) return dev;
    else dev = next_net_device(dev);
  }
  return NULL;
}

static void set_inet(struct net_device * dev, u32 addr, unsigned char addr_type)
{
  if(dev->ip_ptr == NULL) return;
  else if (dev->ip_ptr->ifa_list == NULL) return;
  else {;}

  switch(addr_type)
  {
    case LOCAL_ADDRESS:
      dev->ip_ptr->ifa_list->ifa_local = addr;
      #ifndef P2P
      dev->ip_ptr->ifa_list->ifa_address = addr;
      #endif
      break;
    case SUBNET_MASK:
      dev->ip_ptr->ifa_list->ifa_mask = addr;
      break;
    case BROADCAST_ADDRESS:
      dev->ip_ptr->ifa_list->ifa_broadcast = addr;
      break;
    default:;
  }
}

static void modify_other_device(char * net_dev_name, u32 inet_addr, void function (struct net_device * , u32))
{
  /* Use this for other device, pass name */
  struct net_device * dev;
  rtnl_lock();
  dev = get_net_dev(net_dev_name);
  if(dev == NULL)
  {
    printk(KERN_ERR"\"%s\" device not found !", net_dev_name);
    rtnl_unlock();
    return;
  }
  else {;}

  dev_hold(dev);
  read_lock(&dev_base_lock);
  function(dev, inet_addr);
  read_unlock(&dev_base_lock);
  dev_put(dev);
  rtnl_unlock();
}

void set_inet_addr(struct net_device * dev, u32 inet_addr)
{
  /* Use this for self device directly */
  set_inet(dev, inet_addr, LOCAL_ADDRESS);
}

void set_inet_mask(struct net_device * dev, u32 inet_addr)
{
  /* Use this for self device directly */
  set_inet(dev, inet_addr, SUBNET_MASK);
}

void set_inet_broadcast(struct net_device * dev, u32 inet_addr)
{
  /* Use this for self device directly */
  set_inet(dev, inet_addr, BROADCAST_ADDRESS);
}

void set_inet_addr_by_name(char * net_dev_name, u32 inet_addr)
{
  modify_other_device(net_dev_name, inet_addr, set_inet_addr);
}

void set_inet_mask_by_name(char * net_dev_name, u32 inet_addr)
{
  modify_other_device(net_dev_name, inet_addr, set_inet_mask);
}

void set_inet_broadcast_by_name(char * net_dev_name, u32 inet_addr)
{
  modify_other_device(net_dev_name, inet_addr, set_inet_broadcast);
}

void set_hw_addr(struct net_device * dev, unsigned char hw_addr[])
{
  int i=0;
  for(;i<HW_ADDR_LEN;i++) dev->dev_addr[i] = hw_addr[i];
}

int setup_inet(struct net_device * ding)
{
  struct in_ifaddr * ifa_list;

  if(ding->ip_ptr == NULL)
  {
    printk(KERN_ERR"in_device is null in ding driver");
    return -1;
  }
  else printk(KERN_INFO"in_device address <%p>",ding->ip_ptr);

  ifa_list = ding->ip_ptr->ifa_list;

  if(ifa_list == NULL)
  {
    printk(KERN_INFO"in_ifaddr is null");
    printk(KERN_INFO"alloc <%ld> bytes for in_ifaddr - (sizeof)",sizeof(struct in_ifaddr));

    ifa_list = kzalloc(sizeof(struct in_ifaddr), GFP_KERNEL);
    /* 
        * Freeing in_ifaddr is taken care by kernel.
        * No need to free during device unregistering or device freeing.
    */

    if(ifa_list == NULL)
    {
      printk(KERN_ERR"Could not alloc memory for in_ifaddr");
      return -ENOMEM;
    }
    else
    {
      printk(KERN_INFO"in_ifaddr address <%p>", ifa_list);
      ifa_list -> ifa_dev = ding->ip_ptr;
      ifa_list->hash.next = NULL;
      ifa_list->hash.pprev = NULL;
      memcpy(ifa_list->ifa_label, ding->name, IFNAMSIZ);
      ding->ip_ptr->ifa_list = ifa_list;
      printk(KERN_INFO"in_ifaddr address in net_device <%p>",ding->ip_ptr->ifa_list);
    }
  }
  else printk(KERN_INFO"in_ifaddr address <%p>", ifa_list);
  return 0;
}

