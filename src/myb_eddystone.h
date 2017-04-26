#ifndef __MYB_EDDYSTONE_H__
#define __MYB_EDDYSTONE_H__

void myb_eddystone_set_url(const char *url);
const unsigned char *myb_eddystone_peek_payload(unsigned int *length);

#endif
