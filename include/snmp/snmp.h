#ifndef SNMP_H
#define SNMP_H
/*
 * Definitions for the Simple Network Management Protocol (RFC 1067).
 *
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

/* These come first */
#include "asn1.h"
#include "snmp_error.h"
#include "snmp_session.h"
#include "snmp_vars.h"
#include "snmp_pdu.h"
#include "snmp_msg.h"
#include "snmp_coexist.h"
#include "snmp_error.h"
#include "snmp_api_error.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "snmp-internal.h"
#include "parse.h"
#include "snmp_compat.h"
#include "snmp_debug.h"

#define SNMP_PORT	    161         /* standard UDP port for SNMP agents
                                           to receive requests messages */
#define SNMP_TRAP_PORT	    162         /* standard UDP port for SNMP
                                           managers to receive notificaion
                                          (trap and inform) messages */

#ifndef SNMP_MAX_LEN
#define SNMP_MAX_LEN	    1500        /* typical maximum message size */
#endif
#define SNMP_MIN_MAX_LEN    484         /* minimum maximum message size */

/* SNMP versions */
/* There currently exists the following SNMP versions.
 * (Note that only SNMPv1 is in widespread usage, and this code supports
 *  only SNMPv1, SNMPv2p, and SNMPv2c. Placeholders have been added to
 *  support SNMPv3, when it is published.
 *
 *  SNMPv1 - (full) the original version, defined by RFC 1157
 *  SNMPsec - (historic) the first attempt to add strong security
 *             to SNMPv1, defined by RFCs 1351, 1352, and 1353.
 *  SNMPv2p - (historic) party-based SNMP, which was another
 *             attempt to add strong security to SNMP, defined
 *             by RFCs 1441, 1445, 1446, 1448, and 1449.
 *  SNMPv2c - (experimental) community string-based SNMPv2,
 *             which was an attempt to combine the protocol
 *             operations of SNMPv2 with the security of
 *             SNMPv1, defined by RFCs 1901, 1905, and 1906.
 *  SNMPv2u - (experimental) user-based SNMPv2, which provided
 *             security based on user names and protocol
 *             operations of SNMPv2, defined by RFCs 1905,
 *             1909, and 1910.
 *  SNMPv2* (or SNMPv2star) - (experimental) an attempt to add the
 *             best features of SNMPv2p and SNMPv2u, defined
 *             by unpublished documents found at WEB site
 *             owned by SNMP Research (a leading SNMP vendor)
 *  SNMPv3 - the current attempt by the IETF working group to merge
 *             the SNMPv2u and SNMPv2* proposals into a more widly
 *             accepted SNMPv3.  It is defined by not yet published
 *             documents of the IETF SNMPv3 WG.
 *
 * SNMPv1, SNMPv2c, SNMPv2u, and SNMPv3 messages have a common
 * form, which is an ASN.1 sequence containing a message version 
 * field, followed by version dependent fields.
 * SNMPsec, SNMPv2p, and SNMPv2* messages have a common form,
 * which is a tagged ASN.1 context specific sequence containing
 * message dependent fields.
 *
 * In the #defines for the message versions below, the value
 * for SNMPv1, SNMPv2c, SNMPv2u, and SNMPv3 messages is the
 * value of the message version field. Since SNMPsec, SNMPv2p,
 * and SNMPv2* messages do not have a message version field,
 * the value in the defines for them is choosen to be a large
 * arbitrary number.
 *
 * Note that many of the version ID's are defined below purely for
 * documentational purposes.  At this point the only protocol planned
 * for future implementations is SNMP3, as the other v2 protocols will
 * not be supported by the IETF (ie, v2u, v2sec, v2star) or used by
 * the snmp community at large (at the time of this writing).  */

/* versions based on version field */
#define SNMP_VERSION_1	   0
#define SNMP_VERSION_2c    1
#define SNMP_VERSION_2u    2   /* not (will never be) supported by this code */
#define SNMP_VERSION_3     3   /* not yet supported by this code */

/* versions not based on a version field */
#define SNMP_VERSION_sec   128 /* not (will never be) supported by this code */
#define SNMP_VERSION_2p	   129
#define SNMP_VERSION_2star 130 /* not (will never be) supported by this code */

/* PDU types in SNMPv1, SNMPsec, SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
#define SNMP_MSG_GET	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x0)
#define SNMP_MSG_GETNEXT    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x1)
#define SNMP_MSG_RESPONSE   (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x2)
#define SNMP_MSG_SET	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x3)

/* PDU types in SNMPv1 and SNMPsec */
#define SNMP_MSG_TRAP	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x4)

/* PDU types in SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
#define SNMP_MSG_GETBULK    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x5)
#define SNMP_MSG_INFORM	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x6)
#define SNMP_MSG_TRAP2	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x7)

/* PDU types in SNMPv2u, SNMPv2*, and SNMPv3 */
#define SNMP_MSG_REPORT	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x8)

/* Exception values for SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
#define SNMP_NOSUCHOBJECT    (ASN_CONTEXT | ASN_PRIMITIVE | 0x0)
#define SNMP_NOSUCHINSTANCE  (ASN_CONTEXT | ASN_PRIMITIVE | 0x1)
#define SNMP_ENDOFMIBVIEW    (ASN_CONTEXT | ASN_PRIMITIVE | 0x2)

/* row status values */
#define SNMP_ROW_NONEXISTENT		0
#define SNMP_ROW_ACTIVE			1
#define SNMP_ROW_NOTINSERVICE		2
#define SNMP_ROW_NOTREADY		3
#define SNMP_ROW_CREATEANDGO		4
#define SNMP_ROW_CREATEANDWAIT		5
#define SNMP_ROW_DESTROY		6

/* row storage values */
#define SNMP_STORAGE_OTHER		1
#define SNMP_STORAGE_VOLATILE		2
#define SNMP_STORAGE_NONVOLATILE	3
#define SNMP_STORAGE_PERMANENT		4
#define SNMP_STORAGE_READONLY		5

/* message processing models */
#define SNMP_MP_MODEL_SNMPv1		0
#define SNMP_MP_MODEL_SNMPv2c		1
#define SNMP_MP_MODEL_SNMPv2u		2
#define SNMP_MP_MODEL_SNMPv3		3
#define SNMP_MP_MODEL_SNMPv2p		256

/* security values */
#define SNMP_SEC_MODEL_ANY		0
#define SNMP_SEC_MODEL_SNMPv1		1
#define SNMP_SEC_MODEL_SNMPv2c		2
#define SNMP_SEC_MODEL_USM		3
#define SNMP_SEC_MODEL_SNMPv2p		256

#define SNMP_SEC_LEVEL_NOAUTH		1
#define SNMP_SEC_LEVEL_AUTHNOPRIV	2
#define SNMP_SEC_LEVEL_AUTHPRIV		3

/* view status */
#define SNMP_VIEW_INCLUDED		1
#define SNMP_VIEW_EXCLUDED		2

/* basic oid values */
#define SNMP_OID_INTERNET		1, 3, 6, 1
#define SNMP_OID_ENTERPRISES		SNMP_OID_INTERNET, 4, 1
#define SNMP_OID_MIB2			SNMP_OID_INTERNET, 2, 1
#define SNMP_OID_SNMPV2			SNMP_OID_INTERNET, 6
#define SNMP_OID_SNMPMODULES		SNMP_OID_SNMPV2, 3


#ifdef CMU_COMPATIBLE
/* PDU types in SNMPv1, SNMPsec, SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
#define GET_REQ_MSG	    SNMP_MSG_GET
#define GETNEXT_REQ_MSG	    SNMP_MSG_GETNEXT
#define GET_RSP_MSG	    SNMP_MSG_RESPONSE
#define SET_REQ_MSG	    SNMP_MSG_SET

/* PDU types in SNMPv1 and SNMPsec */
#define TRP_REQ_MSG	    SNMP_MSG_TRAP

/* PDU types in SNMPv2p, SNMPv2c, SNMPv2u, SNMPv2*, and SNMPv3 */
#define BULK_REQ_MSG	    SNMP_MSG_GETBULK
#define INFORM_REQ_MSG	    SNMP_MSG_INFORM
#define TRP2_REQ_MSG	    SNMP_MSG_TRAP2

/* PDU types in SNMPv2u, SNMPv2*, and SNMPv3 */
#define REPORT_RSP_MSG	    SNMP_MSG_REPORT

/* since CMU V1.5 */

#define SNMP_PDU_GET	    SNMP_MSG_GET
#define SNMP_PDU_GETNEXT    SNMP_MSG_GETNEXT
#define SNMP_PDU_RESPONSE   SNMP_MSG_RESPONSE
#define SNMP_PDU_SET        SNMP_MSG_SET
#define SNMP_PDU_GETBULK    SNMP_MSG_GETBULK
#define SNMP_PDU_INFORM     SNMP_MSG_INFORM
#define SNMP_PDU_V2TRAP     SNMP_MSG_TRAP2
#define SNMP_PDU_REPORT     SNMP_MSG_REPORT

#define SNMP_TRAP_AUTHENTICATIONFAILURE SNMP_TRAP_AUTHFAIL

#define SMI_INTEGER     ASN_INTEGER
#define SMI_STRING      ASN_OCTET_STR
#define SMI_OBJID       ASN_OBJECT_ID
#define SMI_NULLOBJ     ASN_NULL
#define SMI_IPADDRESS   ASN_IPADDRESS
#define SMI_COUNTER32	    ASN_COUNTER
#define SMI_GAUGE32	    ASN_GAUGE
#define SMI_UNSIGNED32 SMI_GAUGE32
#define SMI_TIMETICKS   ASN_TIMETICKS
#define SMI_OPAQUE ASN_OPAQUE
#define SMI_COUNTER64   ASN_COUNTER64

char *uptime_string (u_long, char *);
int mib_TxtToOid (char *, oid **, int *);
int mib_OidToTxt (oid *, int , char *, int );

struct snmp_pdu;
char *snmp_pdu_type (struct snmp_pdu *);

struct snmp_session;
u_char * cmu_snmp_parse (struct snmp_session *session,
    struct snmp_pdu *pdu,
    u_char *data,
    int length);

#endif /* CMU_COMPATIBLE */

void xdump (u_char *, int, char *);
u_char *snmp_parse_var_op (u_char *, oid *, int *, u_char *, int *,
                               u_char **, int *);
u_char *snmp_build_var_op (u_char *, oid *, int *, u_char, int, u_char *,
                               int *);
#endif /* SNMP_H */
