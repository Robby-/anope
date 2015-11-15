# Anope -- a set of IRC services for IRC networks

This program is free but copyrighted software; see the file docs/COPYING for
details.

Information about Anope may be found at https://www.anope.org/


**Table of Contents**

- [Credits](#credits)
- [Presentation](#presentation)
- [Installation](#installation)
- [Command-line options](#command-line-options)
- [Messages translation](#messages-translation)
- [Contact](#contact)


## Credits

Please see docs/CREDITS.md for more information.

## Presentation

Anope is a set of Services for IRC networks that allows users to manage
their nicks and channels in a secure and efficient way, and administrators
to manage their network with powerful tools.


Currently available services are:

* NickServ, a powerful nickname manager that users can use to protect
  themselves against nick stealing. Each user has its own nickname
  group, that allows the user to register as many nicks as needed
  while still being able to take profit of his privileges and to
  modify the nick configuration. NickServ also has an optional
  password retrieval/reset feature.

* ChanServ, a powerful channel manager that helps users to administer
  their channels in a totally customizable way. ChanServ has an
  internal list of privileged users and banned users that controls
  accesses on a per-channel basis. It eliminates all takeover
  problems, because of its powerful op/unban/invite and even mass
  deop and mass kick functions.

* MemoServ, an helpful companion that allows sending short messages
  to offline users, that they can then read when they come online
  later.

* BotServ, an original service that allows users to get a permanent,
  friendly bot on their channels in an easy way. Each bot can be
  configured to monitor the channels against floods, repetitions,
  caps writing, and swearing, and to take appropriate actions. It
  also can handle user-friendly commands (like !op, !deop, !voice,
  !devoice, !kick, and many others), say a short greet message when
  an user joins a channel, and even "take over" ChanServ actions such
  as auto-opping users, saying the entry notice, and so on. This
  service can be disabled if you want to save some bandwidth.

* OperServ, the IRCops' and IRC admins' black box, that allows them
  to manage the list of network bans (also known as AKILL (DALnet) or
  GLINE (Undernet)), to configure messages displayed to users when
  they log on, to set modes and to kick users from any channel, to
  send notices quickly to the entire network, and much more!

* HostServ, a neat service that allows users to show custom vHosts
  (virtual hosts) instead of their real IP address; this only works
  on daemons supporting IP cloaking, such as UnrealIRCd.


Anope currently works with:

* Bahamut 1.4.27 or later (including 1.8)
* Charybdis 3.4 or later
* Hybrid 8.1 or later
* InspIRCd 1.2 or 2.0
* ngIRCd 19.2 or later
* Plexus 3 or later
* Ratbox 2.0.6 or later
* UnrealIRCd 3.2 or later

Anope could also work with some of the daemons derived by the ones listed
above, but there's no support for them if they work or don't work.


## Installation

See the docs/INSTALL file for instructions on installing Anope.


## Command-line options

Normally, Anope can be run simply by invoking the "services" executable.
Any of the following command-line options can be specified to change
the behavior of Anope:

    --debug         Enable debugging mode; more info sent to log (give
                        option more times for more info)
    --readonly      Enable read-only mode; no changes to databases
                        allowed
    --nofork        Do not fork after startup; log messages will be
                        written to terminal
    --noexpire      Expiration routines won't be run at all
    --version       Display the version of Anope
    --nothird       Do not load the non-core modules specified
    --protocoldebug Debug each incoming message after protocol parsing
    --support       Used for support, same as --debug --nofork --nothird

Upon starting, Anope will parse its command-line parameters, then
(assuming the --nofork option is not given) detach itself and run in the
background. If Anope encounters a problem reading the database files or
cannot connect to its uplink server, it will terminate immediately;
otherwise, it will run until the connection is terminated (or a QUIT,
SHUTDOWN, or RESTART command is sent; see OperServ's help).

In the case of an error, an appropriate error message will be written to
the log file.

If Anope is run with the "--readonly" command-line option, it can serve as
a "backup" to the full version of services.  A "full" version of services
(run without --readonly) will automatically reintroduce its pseudo-clients
(NickServ, ChanServ, etc.), while a "backup" services will not, thus
allowing full services to be brought up at any time without disrupting
the network (and without having to take backup services down beforehand).

The "--debug" option is useful if you find or suspect a problem in Anope.
Giving it once on the command line will cause all traffic to and from
services as well as some other debugging information to be recorded in
the log file; if you send a bug report, PLEASE include an excerpt from
the log file WITH DEBUGGING ACTIVE; we cannot emphasize enough how
important this is to tracking down problems. (You can also enable
debugging while Anope is running using OperServ's SET DEBUG command.)
If you repeat --debug, the debugging level will be increased, which
provides more detailed information but may also slow Anope down
considerably and make the log file grow dramatically faster. In general,
a debug level of 1 is sufficient for the coding team to be able to trace
a problem, because all network traffic is included and we can usually
reproduce the problem.


## Messages translations

Please see docs/LANGUAGE for this information.


## Contact

For announcements and discussions about Anope, please visit our
Portal and Forums at https://www.anope.org/ -- make sure you register
yourself to get full benefits.

If you read the documentation carefully, and didn't find the answer to
your question, feel free to post on the website forums or join our irc
channel (irc.anope.org #anope). Once you join our Support channel be as 
precise as possible when asking a question, because we have no extraordinary
powers and can't guess things if they aren't provided. 

The more precise you are the sooner you'll be likely to get an answer.

If you think you found a bug, add it to the [bug tracking system](https://bugs.anope.org)
and - again - be as precise as possible. Also say whether the bug happens
always or under what circumstances, and anything that could be useful to
track your bug down. If you wrote a patch, send it over. :)
