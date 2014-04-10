#include "use.h"
#ifdef USE_IPV6

#include "ipv6_pseudo_header.h"

#include <string.h>           // memcpy
#include <stddef.h>           // offsetof
#include <arpa/inet.h>        // ntohs, htonl
#include <netinet/ip6.h>      // ip6_hdr

#include <stdio.h>
#include "buffer.h"

buffer_t * ipv6_pseudo_header_create(const uint8_t * ipv6_segment)
{
    buffer_t             * psh;
    const struct ip6_hdr * iph = (const struct ip6_hdr *) ipv6_segment;
    ipv6_pseudo_header_t * data;

    if (!(psh = buffer_create())) {
        goto ERR_BUFFER_CREATE;
    }

    if (!(buffer_resize(psh, sizeof(ipv6_pseudo_header_t)))) {
        goto ERR_BUFFER_RESIZE;
    }

    data = (ipv6_pseudo_header_t *) buffer_get_data(psh);
    memcpy((uint8_t *) data + offsetof(ipv6_pseudo_header_t, ip_src), &iph->ip6_src, sizeof(ipv6_t));
    memcpy((uint8_t *) data + offsetof(ipv6_pseudo_header_t, ip_dst), &iph->ip6_dst, sizeof(ipv6_t));

    // IPv6 stores a uint16 but our pseudo header uses an uint32 ...
    // So to take care of endianness we cannot directly copy the value.
    data->size = htonl(ntohs(iph->ip6_ctlun.ip6_un1.ip6_un1_plen));
    data->zeros = 0;
    data->zero  = 0;
    data->protocol = iph->ip6_ctlun.ip6_un1.ip6_un1_nxt;

    return psh;

ERR_BUFFER_RESIZE:
    buffer_free(psh);
ERR_BUFFER_CREATE:
    return NULL;
}

#endif // USE_IPV6
