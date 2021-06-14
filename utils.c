/*

  ding network driver (c) Rishi Tharun
  ------------------------------------
    > Source for ding driver utilities.
    > Run 'make' from the project root

  * Author: Rishi Tharun <vrishitharunj@gmail.com>
  * License: GPLv3

*/

#include <linux/types.h>

#define LOOPBACK 0x0100007f

#define CLASS(inet1) (inet1 & 0xc0)

#define CLASS_A0 0x00
#define CLASS_A1 0x40
#define CLASS_B 0x80
#define CLASS_C 0xc0

#define CLASS_A_MASK 0x000000ff
#define CLASS_B_MASK 0x0000ffff
#define CLASS_C_MASK 0x00ffffff

enum
{
  False = 0,
  True = 1
};

static int power(int base, int pow)
{
  int ans=1;

  while(pow--) ans*=base;
  return ans;
}

static u32 _get_gateway(u32 inet)
{
  unsigned char * inet_ptr = (unsigned char *) &inet;
  inet_ptr[3] = inet_ptr[3] == 1 ? 2 : 1;
  return inet;
}

u32 get_gateway(u32 inet)
{
  if (inet == LOOPBACK) return LOOPBACK;
  else return _get_gateway(inet);
}

unsigned char stringsEqual(const char* str1, const char* str2)
{
  while (*str1 && *str2)
  {
    if (*str1++ != *str2++) return False;
    else {;}
  }
  if (*str1 != *str2) return False;
  else return True;
}

u32 get_broadcast(u32 inet, u32 mask)
{
  u32 broadcast = inet;
  unsigned char * broadcast_ptr = (unsigned char *) &broadcast;

  switch(mask)
  {
    case CLASS_A_MASK:
      broadcast_ptr[3] = broadcast_ptr[2] = broadcast_ptr[1] = 0xff;
      break;
    case CLASS_B_MASK:
      broadcast_ptr[3] = broadcast_ptr[2] = 0xff;
      break;
    case CLASS_C_MASK:
      broadcast_ptr[3] = 0xff;
      break;
    default:;
  }
  return broadcast;
}

u32 get_mask(u32 inet)
{
  unsigned char * inet_ptr = (unsigned char *) &inet;
  switch(CLASS(inet_ptr[0]))
  {
    case CLASS_A0:
    case CLASS_A1:
      return CLASS_A_MASK;
    case CLASS_B:
      return CLASS_B_MASK;
    case CLASS_C:
      return CLASS_C_MASK;
    default:
      return 0;
  }
}

u32 str_to_inet(const char * inet_str)
{
  unsigned char i = 0, temp_index = 0;
  unsigned char temp[] = {0,0,0,0};
  while(* (inet_str+i))
  {
    unsigned char curr = * (inet_str+i);
    if (temp_index > 3) return LOOPBACK;
    else {;}
    if ( curr == '.') temp_index++;
    else
    {
      if(curr >= 48 && curr <=57) temp[temp_index] = (curr-48) + temp[temp_index]*10;
      else return LOOPBACK;
    }
    i++;
  }
  if (i<4) return LOOPBACK;
  else {;}

  return *((u32*)(temp));
}

void str_to_hwaddr(const char * hwa_str, unsigned char hwa[])
{
  unsigned char i = 0, hwa_index = 0, expo = 1;
  while(* (hwa_str+i))
  {
    unsigned char curr = * (hwa_str+i);
    if (hwa_index > 5) goto err;
    else {;}
    if ( curr == ':') { hwa_index++; expo = 1; }
    else
    {
      if(curr >= 48 && curr <=57) hwa[hwa_index] += (curr-48) * power(16, expo) ;
      else if (curr >= 97 && curr <=102) hwa[hwa_index] += (curr-87) * power(16, expo) ;
      else if (curr >= 65 && curr <= 70) hwa[hwa_index] += (curr-55) * power(16, expo) ;
      else goto err;
      expo--;
    }
    i++;
  }
  if (i<6) goto err;
  else {;}

  return;

  err:
    hwa[0] = hwa[1] = hwa[2] = hwa[3] = hwa[4] = hwa[5] = 0;
}
