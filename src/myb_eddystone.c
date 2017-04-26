#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "myb_eddystone.h"

#define FRAME_SIZE 20
#define URL_SIZE   17
#define N_ELEMENTS(x) (sizeof(x) / sizeof((x)[0]))

union frame {
  unsigned char bytes[FRAME_SIZE];
  struct url {
    unsigned char frame_type;
    unsigned char tx_power;
    unsigned char url_scheme;
    unsigned char url[URL_SIZE]; 
  } __attribute__((packed)) url;
};

static union frame frame;
static int frame_len;

static const char *schemes[] = {
  "http://www.",
  "https://www.",
  "http://",
  "https://"
};

static const char *expansions[] = {
  ".com/",
  ".org/",
  ".edu/",
  ".net/",
  ".info/",
  ".biz/",
  ".gov/",
  ".com",
  ".org",
  ".edu",
  ".net",
  ".info",
  ".biz",
  ".gov"
};

static const char *set_scheme(const char *url)
{
  unsigned int i;

  for (i = 0; i < N_ELEMENTS(schemes); i++) {
    if (!strncmp(url, schemes[i], strlen(schemes[i]))) {
      frame.url.url_scheme = i;
      printf("matched. i=%d, schemes='%s'\n", i, schemes[i]);
      return url + strlen(schemes[i]);
    }
  }

  return url;
}

static const char *get_expansion(const char *url, unsigned char *value)
{
  char *pos;
  unsigned char i;

  pos = strrchr(url, '.');
  if (!pos)
    return url + strlen(url);

  for (i = 0; i < N_ELEMENTS(expansions); i++) {
    if (!strncmp(pos, expansions[i], strlen(expansions[i]))) {
      *value = i;
      printf("matched. i=%d, expansion='%s'\n", i, expansions[i]);
      return pos;
    }
  }

  return url + strlen(url);
}

static void dump()
{
  int i;

  if (frame_len == 0)
    return;

  printf("  HEX: ");
  for (i = 0; i < frame_len; i++) {
    printf("0x%02X ", frame.bytes[i]);
  }
  printf("\n");

  printf("ASCII: ");
  for (i = 0; i < frame_len; i++) {
    if (isprint(frame.bytes[i]))
      printf("   %C ", frame.bytes[i]);
    else
      printf("---- ");
  }
  printf("\n");
}

void myb_eddystone_set_url(const char *url)
{
  const char *hostname_pos;
  const char *expansion_pos;
  unsigned char value = 0;
  int i;
  int hostname_len;

  if (!url)
    return;

  printf("url='%s'\n", url);

  frame.url.frame_type = 0x10;
  frame.url.tx_power = 0;
  frame_len = 2;

  hostname_pos = set_scheme(url);
  frame_len++;

  expansion_pos = get_expansion(url, &value);
  hostname_len = expansion_pos - hostname_pos;
  frame_len += hostname_len;

  for (i = 0; i < hostname_len; i++)
    frame.url.url[i] = hostname_pos[i];

  frame.url.url[i] = value;
 
  if (value > 0)
    frame_len++;

  dump();
}

const unsigned char *myb_eddystone_peek_payload(unsigned int *length)
{
  if (length)
    *length = frame_len;
    
  return frame.bytes;
}
