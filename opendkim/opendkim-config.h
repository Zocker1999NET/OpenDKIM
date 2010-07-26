/*
**  Copyright (c) 2006-2009 Sendmail, Inc. and its suppliers.
**	All rights reserved.
**
**  Copyright (c) 2009, 2010, The OpenDKIM Project.  All rights reserved.
**
**  $Id: opendkim-config.h,v 1.21.4.1 2010/07/26 14:29:02 cm-msk Exp $
*/

#ifndef _DKIM_CONFIG_H_
#define _DKIM_CONFIG_H_

#ifndef lint
static char dkim_config_h_id[] = "@(#)$Id: opendkim-config.h,v 1.21.4.1 2010/07/26 14:29:02 cm-msk Exp $";
#endif /* !lint */

#include "build-config.h"

#ifndef FALSE
# define FALSE	0
#endif /* ! FALSE */
#ifndef TRUE
# define TRUE	1
#endif /* ! TRUE */

struct configdef dkimf_config[] =
{
	{ "ADSPAction",			CONFIG_TYPE_STRING,	FALSE },
	{ "ADSPNoSuchDomain",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "AllowSHA1Only",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "AlwaysAddARHeader",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "AlwaysSignHeaders",		CONFIG_TYPE_STRING,	FALSE },
	{ "AuthservID",			CONFIG_TYPE_STRING,	FALSE },
	{ "AuthservIDWithJobID",	CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "AutoRestart",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "AutoRestartCount",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "AutoRestartRate",		CONFIG_TYPE_STRING,	FALSE },
	{ "Background",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "BaseDirectory",		CONFIG_TYPE_STRING,	FALSE },
	{ "BodyLengths",		CONFIG_TYPE_BOOLEAN,	FALSE },
#ifdef _FFR_BODYLENGTH_DB
	{ "BodyLengthDBFile",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_BODYLENGTH_DB */
#ifdef USE_UNBOUND
	{ "BogusKey",			CONFIG_TYPE_STRING,	FALSE },
	{ "BogusPolicy",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_UNBOUND*/
	{ "Canonicalization",		CONFIG_TYPE_STRING,	FALSE },
	{ "ClockDrift",			CONFIG_TYPE_INTEGER,	FALSE },
#ifdef _FFR_DEFAULT_SENDER
	{ "DefaultSender",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_DEFAULT_SENDER */
	{ "Diagnostics",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "DiagnosticDirectory",	CONFIG_TYPE_STRING,	FALSE },
	{ "DNSTimeout",			CONFIG_TYPE_INTEGER,	FALSE },
	{ "Domain",			CONFIG_TYPE_STRING,	FALSE },
	{ "DomainKeysCompat",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "DontSignMailTo",		CONFIG_TYPE_STRING,	FALSE },
	{ "EnableCoredumps",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "ExemptDomains",		CONFIG_TYPE_STRING,	FALSE },
	{ "ExternalIgnoreList",		CONFIG_TYPE_STRING,	FALSE },
#ifdef USE_LUA
	{ "FinalPolicyScript",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_LUA */
	{ "FixCRLF",			CONFIG_TYPE_BOOLEAN,	FALSE },
#ifdef _FFR_IDENTITY_HEADER
	{ "IdentityHeader",		CONFIG_TYPE_STRING,     FALSE },
	{ "IdentityHeaderRemove",	CONFIG_TYPE_BOOLEAN,    FALSE },
#endif /* _FFR_IDENTITY_HEADER */
	{ "Include",			CONFIG_TYPE_INCLUDE,	FALSE },
#ifdef USE_UNBOUND
	{ "InsecureKey",		CONFIG_TYPE_STRING,	FALSE },
	{ "InsecurePolicy",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_UNBOUND */
	{ "InternalHosts",		CONFIG_TYPE_STRING,	FALSE },
	{ "KeepTemporaryFiles",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "KeyFile",			CONFIG_TYPE_STRING,	FALSE },
	{ "KeyTable",			CONFIG_TYPE_STRING,	FALSE },
#ifdef USE_LDAP
	{ "LDAPAuthMechanism",		CONFIG_TYPE_STRING,	FALSE },
# ifdef USE_SASL
	{ "LDAPAuthName",		CONFIG_TYPE_STRING,	FALSE },
	{ "LDAPAuthRealm",		CONFIG_TYPE_STRING,	FALSE },
	{ "LDAPAuthUser",		CONFIG_TYPE_STRING,	FALSE },
# endif /* USE_SASL */
	{ "LDAPBindPassword",		CONFIG_TYPE_STRING,	FALSE },
	{ "LDAPBindUser",		CONFIG_TYPE_STRING,	FALSE },
	{ "LDAPUseTLS",			CONFIG_TYPE_BOOLEAN,	FALSE },
#endif /* USE_LDAP */
	{ "LocalADSP",			CONFIG_TYPE_STRING,	FALSE },
	{ "LogWhy",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "MaximumHeaders",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "MaximumSignedBytes",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "MacroList",			CONFIG_TYPE_STRING,	FALSE },
	{ "MilterDebug",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "Minimum",			CONFIG_TYPE_STRING,	FALSE },
	{ "MultipleSignatures",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "Mode",			CONFIG_TYPE_STRING,	FALSE },
	{ "MTA",			CONFIG_TYPE_STRING,	FALSE },
	{ "MustBeSigned",		CONFIG_TYPE_STRING,	FALSE },
	{ "OmitHeaders",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-BadSignature",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-Default",			CONFIG_TYPE_STRING,	FALSE },
	{ "On-DNSError",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-InternalError",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-KeyNotFound",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-NoSignature",		CONFIG_TYPE_STRING,	FALSE },
	{ "On-Security",		CONFIG_TYPE_STRING,	FALSE },
	{ "PeerList",			CONFIG_TYPE_STRING,	FALSE },
	{ "PidFile",			CONFIG_TYPE_STRING,	FALSE },
#ifdef POPAUTH
	{ "POPDBFile",			CONFIG_TYPE_STRING,	FALSE },
#endif /* POPAUTH */
	{ "Quarantine",			CONFIG_TYPE_BOOLEAN,	FALSE },
#ifdef QUERY_CACHE
	{ "QueryCache",			CONFIG_TYPE_BOOLEAN,	FALSE },
#endif /* QUERY_CACHE */
#ifdef _FFR_REDIRECT
	{ "RedirectFailuresTo",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_REDIRECT */
	{ "RemoveARAll",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "RemoveARFrom",		CONFIG_TYPE_STRING,	FALSE },
	{ "RemoveOldSignatures",	CONFIG_TYPE_BOOLEAN,	FALSE },
#ifdef _FFR_REPLACE_RULES
	{ "ReplaceRules",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_REPLACE_RULES */
#ifdef _FFR_REPORT_INTERVALS
	{ "ReportIntervalDB",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_REPORT_INTERVALS */
	{ "ReportAddress",		CONFIG_TYPE_STRING,	FALSE },
#ifdef _FFR_DKIM_REPUTATION
	{ "ReputationFail",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "ReputationPass",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "ReputationReject",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "ReputationRoot",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_DKIM_REPUTATION */
	{ "RequiredHeaders",		CONFIG_TYPE_BOOLEAN,	FALSE },
#ifdef _FFR_RESIGN
	{ "RequireSafeKeys",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "ResignAll",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "ResignMailTo",		CONFIG_TYPE_STRING,	FALSE },
#ifdef USE_LUA
	{ "ScreenPolicyScript",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_LUA */
#endif /* _FFR_RESIGN */
	{ "Selector",			CONFIG_TYPE_STRING,	FALSE },
#ifdef USE_LUA
	{ "SetupPolicyScript",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_LUA */
#ifdef _FFR_SELECTOR_HEADER
	{ "SelectorHeader",		CONFIG_TYPE_STRING,	FALSE },
	{ "SelectorHeaderRemove",	CONFIG_TYPE_BOOLEAN,	FALSE },
#endif /* _FFR_SELECTOR_HEADER */
	{ "SendADSPReports",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "SenderHeaders",		CONFIG_TYPE_STRING,	FALSE },
#ifdef _FFR_SENDER_MACRO
	{ "SenderMacro",		CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_SENDER_MACRO */
	{ "SendReports",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "SignatureAlgorithm",		CONFIG_TYPE_STRING,	FALSE },
	{ "SignatureTTL",		CONFIG_TYPE_INTEGER,	FALSE },
	{ "SignHeaders",		CONFIG_TYPE_STRING,	FALSE },
	{ "SigningTable",		CONFIG_TYPE_STRING,	FALSE },
	{ "Socket",			CONFIG_TYPE_STRING,	FALSE },
#ifdef _FFR_STATS
	{ "Statistics",			CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_STATS */
	{ "StrictTestMode",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "SubDomains",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "Syslog",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "SyslogFacility",		CONFIG_TYPE_STRING,	FALSE },
	{ "SyslogSuccess",		CONFIG_TYPE_BOOLEAN,	FALSE },
	{ "TemporaryDirectory",		CONFIG_TYPE_STRING,	FALSE },
	{ "TestPublicKeys",		CONFIG_TYPE_STRING,	FALSE },
#ifdef USE_UNBOUND
	{ "TrustAnchorFile",		CONFIG_TYPE_STRING,	FALSE },
#endif /* USE_UNBOUND */
	{ "TrustSignaturesFrom",	CONFIG_TYPE_STRING,	FALSE },
	{ "UMask",			CONFIG_TYPE_INTEGER,	FALSE },
	{ "UserID",			CONFIG_TYPE_STRING,	FALSE },
#ifdef _FFR_VBR
	{ "VBR-Certifiers",		CONFIG_TYPE_STRING,	FALSE },
	{ "VBR-TrustedCertifiers",	CONFIG_TYPE_STRING,	FALSE },
	{ "VBR-Type",			CONFIG_TYPE_STRING,	FALSE },
#endif /* _FFR_VBR */
	{ "X-Header",			CONFIG_TYPE_BOOLEAN,	FALSE },
	{ NULL,				-1,			FALSE }
};

#endif /* _DKIM_CONFIG_H_ */
