#!/usr/bin/perl
#
# Copyright (c) 2010, 2011, The OpenDKIM Project.  All rights reserved.
#
# $Id: opendkim-genstats,v 1.26 2010/10/27 06:18:45 cm-msk Exp $
#
# Script to generate rate and spam ratio predictions for each domain
# based on accumulated data.  EXPERIMENTAL.

###
### Setup
###

use strict;
use warnings;

use DBI;
use File::Basename;
use Getopt::Long;
use IO::Handle;
use POSIX;

require DBD::mysql;

# general
my $progname      = basename($0);
my $version       = "\@VERSION@";
my $verbose       = 0;
my $helponly      = 0;
my $showversion   = 0;

my $out;
my $idx;

my $dbi_s;
my $dbi_h;
my $dbi_a;

my $thresh;

# DB parameters
my $def_dbhost    = "localhost";
my $def_dbname    = "opendkim";
my $def_dbuser    = "opendkim";
my $def_dbpasswd  = "opendkim";
my $def_dbport    = "3306";
my $dbhost;
my $dbname;
my $dbuser;
my $dbpasswd;
my $dbport;

my $dbscheme     = "mysql";

# output location (default)
my $def_output   = "/var/www/docs/opendkim/rates.html";
my $reportout;
my $tmpout;

# "spammy" ratio default
my $spamratio    = 0.75;

# minimum message count for consideration
my $minmsgs      = 10;
my $minspamcount = 2;

# prediction interval calculation
my $def_pi       = 75;
my $pisize;
my $stdscore;
my %stdscores    = (	50,	0.67,
			68,	1.00,
			75,	1.15,
			90,	1.64,
			95,	1.96,
			99,	2.58 );

# Minimum messages.id value; used to mark the start of useful data
my $minmsgid     = 5855122;

###
### NO user-serviceable parts beyond this point
###

sub usage
{
	print STDERR "$progname: usage: $progname [options]\n";
	print STDERR "\t--dbhost=host      database host [$def_dbhost]\n";
	print STDERR "\t--dbname=name      database name [$def_dbname]\n";
	print STDERR "\t--dbpasswd=passwd  database password [$def_dbpasswd]\n";
	print STDERR "\t--dbport=port      database port [$def_dbport]\n";
	print STDERR "\t--dbuser=user      database user [$def_dbuser]\n";
	print STDERR "\t--help             print help and exit\n";
	print STDERR "\t--output=file      output file [$def_output]\n";
	print STDERR "\t--prediction=pct   prediction interval [$def_pi]\n";
	print STDERR "\t--verbose          verbose output\n";
	print STDERR "\t--version          print version and exit\n";
}

# parse command line arguments
my $opt_retval = &Getopt::Long::GetOptions ('dbhost=s' => \$dbhost,
                                            'dbname=s' => \$dbname,
                                            'dbpasswd=s' => \$dbpasswd,
                                            'dbport=s' => \$dbport,
                                            'dbuser=s' => \$dbuser,
                                            'help!' => \$helponly,
                                            'outputs' => \$reportout,
                                            'prediction=i' => \$pisize,
                                            'verbose!' => \$verbose,
                                            'version!' => \$showversion,
                                           );

if (!$opt_retval || $helponly)
{
	usage();

	if ($helponly)
	{
		exit(0);
	}
	else
	{
		exit(1);
	}
}

if ($showversion)
{
	print STDOUT "$progname v$version\n";
	exit(0);
}

# apply defaults
if (!defined($dbhost))
{
	if (defined($ENV{'OPENDKIM_DBHOST'}))
	{
		$dbhost = $ENV{'OPENDKIM_DBHOST'};
	}
	else
	{
		$dbhost = $def_dbhost;
	}
}

if (!defined($dbname))
{
	if (defined($ENV{'OPENDKIM_DB'}))
	{
		$dbname = $ENV{'OPENDKIM_DB'};
	}
	else
	{
		$dbname = $def_dbname;
	}
}

if (!defined($dbpasswd))
{
	if (defined($ENV{'OPENDKIM_PASSWORD'}))
	{
		$dbpasswd = $ENV{'OPENDKIM_PASSWORD'};
	}
	else
	{
		$dbpasswd = $def_dbpasswd;
	}
}

if (!defined($dbport))
{
	if (defined($ENV{'OPENDKIM_PORT'}))
	{
		$dbport = $ENV{'OPENDKIM_PORT'};
	}
	else
	{
		$dbport = $def_dbport;
	}
}

if (!defined($dbuser))
{
	if (defined($ENV{'OPENDKIM_USER'}))
	{
		$dbuser = $ENV{'OPENDKIM_USER'};
	}
	else
	{
		$dbuser = $def_dbuser;
	}
}

if (!defined($reportout))
{
	if (defined($ENV{'OPENDKIM_OUTPUT'}))
	{
		$reportout = $ENV{'OPENDKIM_OUTPUT'};
	}
	else
	{
		$reportout = $def_output;
	}
}

if (!defined($pisize))
{
	$pisize = $def_pi;
}

$stdscore = $stdscores{$pisize};
if (!defined($stdscore))
{
	print STDERR "$progname: unknown prediction interval size $pisize\n";
	exit(1);
}
elsif ($verbose)
{
	print STDERR "$progname: using standard score $stdscore\n";
}

my $dbi_dsn = "DBI:" . $dbscheme . ":database=" . $dbname .
              ";host=" . $dbhost . ";port=" . $dbport;

$dbi_h = DBI->connect($dbi_dsn, $dbuser, $dbpasswd, { PrintError => 0 });
if (!defined($dbi_h))
{
	print STDERR "$progname: unable to connect to database: $DBI::errstr\n";
	exit(1);
}

if ($verbose)
{
	print STDERR "$progname: connected to database\n";
}

#
# start the report
#
$tmpout = $reportout . "." . $$;
open($out, ">", $tmpout) or die "$progname: can't open $tmpout: $!";
if ($verbose)
{
	print STDERR "$progname: report started in $tmpout\n";
}

print $out "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\">\n";
print $out "<html>\n";
print $out "  <head>\n";
print $out "    <title>\n";
print $out "      OpenDKIM Rate Recommendations\n";
print $out "    </title>\n";
print $out "  </head>\n";
print $out "\n";
print $out "  <body>\n";

print $out "<h1>OpenDKIM Rate Recommendations</h1>\n";
print $out "Generated " . strftime("%b %e %Y %H:%M:%S", localtime) . "</h1>\n";
print $out "<hr>\n";

#
# compute the low-time threshold
#
if ($verbose)
{
	print STDERR "$progname: computing low-time threshold\n";
}

print $out "Analysis of high spam domain duration (>= " . $spamratio * 100 . "% spam, >= " . $minspamcount . " msg(s))\n";
print $out "<table border=1 columns=11>\n";
print $out " <tr>\n";
print $out "  <td>Domains</td>\n";
print $out "  <td>Min. Duration</td>\n";
print $out "  <td>Max. Duration</td>\n";
print $out "  <td>Mean Duration</td>\n";
print $out "  <td>Duration Std. Dev.</td>\n";
print $out " </tr>\n";
$dbi_s = $dbi_h->prepare("SELECT COUNT(*) AS domains, MIN(l) AS 'min duration', MAX(l) AS 'max duration', AVG(l) AS 'mean duration', STDDEV_POP(l) AS 'duration stddev' FROM (SELECT COUNT(*) AS c, SUM(messages.spam)/COUNT(*) AS r, DATEDIFF(MAX(messages.msgtime), MIN(messages.msgtime)) AS l FROM signatures JOIN messages ON signatures.message = messages.id WHERE messages.id >= $minmsgid AND NOT spam = -1 AND pass = 1 GROUP BY signatures.domain) t1 WHERE r >= $spamratio AND c >= $minspamcount");
if (!$dbi_s->execute)
{
	print STDERR "$progname: report #1 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
else
{
	while ($dbi_a = $dbi_s->fetchrow_arrayref())
	{
		print $out " <tr>\n";
		for ($idx = 0; $idx < 5; $idx++)
		{
			if (defined($dbi_a->[$idx]))
			{
				print $out " <td> " . $dbi_a->[$idx] . " </td>\n";
			}
			else
			{
				print $out " <td> NULL </td>\n";
			}
		}
		print $out " </tr>\n";

		$thresh = $dbi_a->[3] + $dbi_a->[4] * $stdscore;
	}
}
print $out "</table>\n";
print $out "<br>\n";
$dbi_s->finish;

if ($verbose)
{
	print STDERR "$progname: low time threshold is $thresh day(s)\n";
}

#
# Compute/update message counts and spam ratios for all signing domains
#
if ($verbose)
{
	print STDERR "$progname: computing counts and ratios for signers\n";
}

$dbi_s = $dbi_h->prepare("INSERT IGNORE INTO aggregate (domain, updated, date, messages, spam) SELECT signatures.domain, CURRENT_TIMESTAMP(), DATE(messages.msgtime), COUNT(*), SUM(spam) FROM messages JOIN signatures ON signatures.message = messages.id WHERE NOT spam = -1 AND pass = 1 AND messages.id >= $minmsgid AND NOT DATE(messages.msgtime) = DATE(CURRENT_TIMESTAMP()) GROUP BY domain, DATE(messages.msgtime)");
if (!$dbi_s->execute)
{
	print STDERR "$progname: pass #1 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
$dbi_s->finish;

#
# Compute/update message counts and spam ratios for unsigned mail
#
if ($verbose)
{
	print STDERR "$progname: computing counts and ratios for unsigned mail\n";
}

$dbi_s = $dbi_h->prepare("INSERT IGNORE INTO aggregate (domain, updated, date, messages, spam) SELECT 0, CURRENT_TIMESTAMP(), DATE(messages.msgtime), COUNT(*), SUM(spam) FROM messages WHERE NOT spam = -1 AND messages.id >= $minmsgid AND NOT DATE (messages.msgtime) = DATE(CURRENT_TIMESTAMP()) AND (sigcount = 0 OR messages.id NOT IN (SELECT message FROM signatures WHERE pass = 1)) GROUP BY DATE(messages.msgtime)");
if (!$dbi_s->execute)
{
	print STDERR "$progname: pass #2 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
$dbi_s->finish;

if ($verbose)
{
	print STDERR "$progname: computing counts and ratios for low-time domains\n";
}

$dbi_s = $dbi_h->prepare("INSERT IGNORE INTO aggregate (domain, updated, date, messages, spam) SELECT -1, CURRENT_TIMESTAMP(), DATE(messages.msgtime), COUNT(*), SUM(spam) FROM messages JOIN signatures ON signatures.message = messages.id JOIN domains on signatures.domain = domains.id WHERE NOT spam = -1 AND messages.id >= $minmsgid AND NOT DATE (messages.msgtime) = DATE(CURRENT_TIMESTAMP()) AND (sigcount = 0 OR messages.id NOT IN (SELECT message FROM signatures WHERE pass = 1)) AND DATEDIFF(DATE(NOW()), DATE(domains.firstseen)) < $thresh GROUP BY DATE(messages.msgtime)");
if (!$dbi_s->execute)
{
	print STDERR "$progname: pass #2 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
$dbi_s->finish;

#
# Compute ratios wherever they're missing
# 
if ($verbose)
{
	print STDERR "$progname: computing missing ratios\n";
}

$dbi_s = $dbi_h->prepare("UPDATE aggregate SET ratio = spam / messages WHERE ratio IS NULL");
if (!$dbi_s->execute)
{
	print STDERR "$progname: pass #3 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
$dbi_s->finish;

#
# Compute predictions
#
if ($verbose)
{
	print STDERR "$progname: computing specific domain predictions\n";
}

$dbi_s = $dbi_h->prepare("INSERT INTO predictions (domain, rate_samples, rate_max, rate_avg, rate_stddev, rate_high, ratio_max, ratio_avg, ratio_stddev, ratio_high, daily_limit_low, daily_limit_high) SELECT domain, COUNT(aggregate.messages), MAX(aggregate.messages), AVG(aggregate.messages), STDDEV_POP(aggregate.messages), AVG(aggregate.messages) + STDDEV_POP(aggregate.messages) * $stdscore, MAX(aggregate.ratio), AVG(aggregate.ratio), STDDEV_POP(aggregate.ratio), AVG(aggregate.ratio) + STDDEV_POP(aggregate.ratio) * $stdscore, (AVG(aggregate.messages) + STDDEV_POP(aggregate.messages) * $stdscore) * (1 - (AVG(aggregate.ratio) + STDDEV_POP(aggregate.ratio) * $stdscore)), (AVG(aggregate.messages) + STDDEV_POP(aggregate.messages) * $stdscore) * (1 - (AVG(aggregate.ratio) - STDDEV_POP(aggregate.ratio) * $stdscore)) FROM aggregate GROUP BY domain ON DUPLICATE KEY UPDATE rate_samples = VALUES(rate_samples), rate_max = VALUES(rate_max), rate_avg = VALUES(rate_avg), rate_stddev = VALUES(rate_stddev), rate_high = VALUES(rate_high), ratio_max = VALUES(ratio_max), ratio_avg = VALUES(ratio_avg), ratio_stddev = VALUES(ratio_stddev), ratio_high = VALUES(ratio_high), daily_limit_low = VALUES(daily_limit_low), daily_limit_high = VALUES(daily_limit_high)");
if (!$dbi_s->execute)
{
	print STDERR "$progname: pass #4 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
$dbi_s->finish;

#
# reports
# 
if ($verbose)
{
	print STDERR "$progname: reporting on interesting domains\n";
}

print $out "$pisize% prediction interval, standard score $stdscore\n";
print $out "<table border=1 columns=12>\n";
print $out " <tr>\n";
print $out "  <td>Domain ID</td>\n";
print $out "  <td>Domain Name</td>\n";
print $out "  <td>Mean Rate</td>\n";
print $out "  <td>Max Rate</td>\n";
print $out "  <td>Rate Std. Dev.</td>\n";
print $out "  <td>Rate High Prediction</td>\n";
print $out "  <td>Mean Spam Ratio</td>\n";
print $out "  <td>Max Spam Ratio</td>\n";
print $out "  <td>Spam Ratio Std. Dev.</td>\n";
print $out "  <td>Spam Ratio High Prediction</td>\n";
print $out "  <td>Volume Limit (low)</td>\n";
print $out "  <td>Volume Limit (high)</td>\n";
print $out " </tr>\n";
$dbi_s = $dbi_h->prepare("SELECT predictions.domain, domains.name, rate_avg, rate_max, rate_stddev, rate_high, ratio_avg, ratio_max, ratio_stddev, ratio_high, daily_limit_low, daily_limit_high FROM predictions LEFT OUTER JOIN domains ON predictions.domain = domains.id WHERE domain IN (13, 22, 127, 205, 309, 418, 450, 795, 22199, 144284, 313943, 537522, 0, -1)");
if (!$dbi_s->execute)
{
	print STDERR "$progname: report #1 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
else
{
	while ($dbi_a = $dbi_s->fetchrow_arrayref())
	{
		print $out " <tr>\n";
		for ($idx = 0; $idx < 12; $idx++)
		{
			if (defined($dbi_a->[$idx]))
			{
				print $out " <td> " . $dbi_a->[$idx] . " </td>\n";
			}
			else
			{
				print $out " <td> NULL </td>\n";
			}
		}
		print $out " </tr>\n";
	}
}
print $out "</table>\n";
print $out "<br>\n";
$dbi_s->finish;

#
# low-time behaviour analysis
# 
if ($verbose)
{
	print STDERR "$progname: reporting on low-time domain behaviour\n";
}

print $out "Analysis of low-time domain behaviour (<= " . $thresh . " days, >= " . $minmsgs . " msg(s))\n";
print $out "<table border=1 columns=11>\n";
print $out " <tr>\n";
print $out "  <td>Domains</td>\n";
print $out "  <td>Max Rate</td>\n";
print $out "  <td>Mean Rate</td>\n";
print $out "  <td>Rate Std. Dev.</td>\n";
print $out "  <td>Rate High Prediction</td>\n";
print $out "  <td>Max Spam Ratio</td>\n";
print $out "  <td>Mean Spam Ratio</td>\n";
print $out "  <td>Spam Ratio Std. Dev.</td>\n";
print $out "  <td>Spam Ratio Low Prediction</td>\n";
print $out "  <td>Spam Ratio High Prediction</td>\n";
print $out "  <td>Volume Limit</td>\n";
print $out " </tr>\n";
$dbi_s = $dbi_h->prepare("SELECT COUNT(*) AS domains, MAX(counts.c) AS rate_max, AVG(counts.c) AS rate_avg, STDDEV_POP(counts.c) AS rate_stddev, AVG(counts.c) + STDDEV_POP(counts.c) * $stdscore AS rate_high, MAX(counts.r) AS ratio_max, AVG(counts.r) AS ratio_avg, STDDEV_POP(counts.r) AS ratio_stddev, AVG(counts.r) - STDDEV_POP(counts.r) * $stdscore AS ratio_low, AVG(counts.r) + STDDEV_POP(counts.r) * $stdscore AS ratio_high, AVG(counts.c) + STDDEV_POP(counts.c) * $stdscore * (1 - (AVG(counts.r) + STDDEV_POP(counts.r) * $stdscore)) AS daily_limit FROM (SELECT SUM(messages) AS c, SUM(spam)/SUM(messages) AS r FROM aggregate JOIN domains ON aggregate.domain = domains.id WHERE domains.firstseen >= DATE_SUB(CURRENT_TIMESTAMP(), INTERVAL $thresh DAY) GROUP BY domain HAVING SUM(messages) >= $minmsgs) counts");
if (!$dbi_s->execute)
{
	print STDERR "$progname: report #2 failed: " . $dbi_h->errstr;
	$dbi_s->finish;
	$dbi_h->disconnect;
	exit(1);
}
else
{
	while ($dbi_a = $dbi_s->fetchrow_arrayref())
	{
		print $out " <tr>\n";
		for ($idx = 0; $idx < 11; $idx++)
		{
			if (defined($dbi_a->[$idx]))
			{
				print $out " <td> " . $dbi_a->[$idx] . " </td>\n";
			}
			else
			{
				print $out " <td> NULL </td>\n";
			}
		}
		print $out " </tr>\n";
	}
}
print $out "</table>\n";
$dbi_s->finish;

# footer
print $out "  </body>\n";
print $out "</html>\n";

# all done!
if ($verbose)
{
	print STDERR "$progname: terminating\n";
}

$dbi_h->disconnect;
close $out;
rename($tmpout, $reportout) or die "$progname: rename to $reportout failed; $!";
exit(0);
