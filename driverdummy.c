/*

  ding network driver (c) Rishi Tharun
  ------------------------------------
    > Source for dingdummy network driver.
    > Run 'make' from the project root

  * Author: Rishi Tharun <vrishitharunj@gmail.com>
  * License: GPLv3

*/

#define DING_DUMMY

#include"dingdriver.h"

int ding_send(struct sk_buff* skb, struct net_device* ding)
{
  /*
      THIS IS INCOMPLETE.
      NEED TO HANDLE THIS.
      ROUTING DECISIONS ARE NOT FULLY DONE.
  */

  printk(KERN_INFO"ding driver xmit\n");
  return 0;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_LICENSE(DRIVER_LICENSE);

module_init(init_ding);
module_exit(deinit_ding);
