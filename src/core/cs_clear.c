/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int do_clear(User * u);
void myChanServHelp(User * u);

class CSClear : public Module
{
 public:
	CSClear(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("CLEAR", do_clear, NULL, CHAN_HELP_CLEAR, -1, -1, -1, -1);
		this->AddCommand(CHANSERV, c, MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_CLEAR);
}

/**
 * The /cs clear command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_clear(User * u)
{
	char *chan = strtok(NULL, " ");
	char *what = strtok(NULL, " ");
	Channel *c;
	ChannelInfo *ci;

	if (!what) {
		syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
	} else if (!(c = findchan(chan))) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
	} else if (!(ci = c->ci)) {
		notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
	} else if (ci->flags & CI_VERBOTEN) {
		notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
	} else if (!u || !check_access(u, ci, CA_CLEAR)) {
		notice_lang(s_ChanServ, u, PERMISSION_DENIED);
	} else if (stricmp(what, "bans") == 0) {
		const char *av[2];
		Entry *ban, *next;

		if (c->bans && c->bans->count) {
			for (ban = c->bans->entries; ban; ban = next) {
				next = ban->next;
				av[0] = "-b";
				av[1] = ban->mask;
				ircdproto->SendMode(whosends(ci), chan, "-b %s", ban->mask);
				chan_set_modes(whosends(ci)->nick, c, 2, av, 0);
			}
		}

		notice_lang(s_ChanServ, u, CHAN_CLEARED_BANS, chan);
	} else if (ircd->except && stricmp(what, "excepts") == 0) {
		const char *av[2];
		Entry *except, *next;

		if (c->excepts && c->excepts->count) {
			for (except = c->excepts->entries; except; except = next) {
				next = except->next;
				av[0] = "-e";
				av[1] = except->mask;
				ircdproto->SendMode(whosends(ci), chan, "-e %s", except->mask);
				chan_set_modes(whosends(ci)->nick, c, 2, av, 0);
			}
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_EXCEPTS, chan);

	} else if (ircd->invitemode && stricmp(what, "invites") == 0) {
		const char *av[2];
		Entry *invite, *next;

		if (c->invites && c->invites->count) {
			for (invite = c->invites->entries; invite; invite = next) {
				next = invite->next;
				av[0] = "-I";
				av[1] = invite->mask;
				ircdproto->SendMode(whosends(ci), chan, "-I %s", invite->mask);
				chan_set_modes(whosends(ci)->nick, c, 2, av, 0);
			}
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_INVITES, chan);

	} else if (stricmp(what, "modes") == 0) {
		const char *argv[2];

		if (c->mode) {
			/* Clear modes the bulk of the modes */
			ircdproto->SendMode(whosends(ci), c->name, "%s",
						   ircd->modestoremove);
			argv[0] = ircd->modestoremove;
			chan_set_modes(whosends(ci)->nick, c, 1, argv, 0);

			/* to prevent the internals from complaining send -k, -L, -f by themselves if we need
			   to send them - TSL */
			if (c->key) {
				ircdproto->SendMode(whosends(ci), c->name, "-k %s", c->key);
				argv[0] = "-k";
				argv[1] = c->key;
				chan_set_modes(whosends(ci)->nick, c, 2, argv, 0);
			}
			if (ircd->Lmode && c->redirect) {
				ircdproto->SendMode(whosends(ci), c->name, "-L %s",
							   c->redirect);
				argv[0] = "-L";
				argv[1] = c->redirect;
				chan_set_modes(whosends(ci)->nick, c, 2, argv, 0);
			}
			if (ircd->fmode && c->flood) {
				if (flood_mode_char_remove) {
					ircdproto->SendMode(whosends(ci), c->name, "%s %s",
								   flood_mode_char_remove, c->flood);
					argv[0] = flood_mode_char_remove;
					argv[1] = c->flood;
					chan_set_modes(whosends(ci)->nick, c, 2, argv, 0);
				} else {
					if (debug) {
						alog("debug: flood_mode_char_remove was not set unable to remove flood/throttle modes");
					}
				}
			}
			check_modes(c);
		}

		notice_lang(s_ChanServ, u, CHAN_CLEARED_MODES, chan);
	} else if (stricmp(what, "ops") == 0) {
		const char *av[6];  /* The max we have to hold: chan, ts, modes(max3), nick, nick, nick */
		int ac, isop, isadmin, isown, count, i;
		char buf[BUFSIZE], tmp[BUFSIZE], tmp2[BUFSIZE];
		struct c_userlist *cu, *next;

		if (ircd->svsmode_ucmode) {
			av[0] = chan;
			ircdproto->SendSVSModeChan(av[0], "-o", NULL);
			if (ircd->owner) {
				ircdproto->SendSVSModeChan(av[0], ircd->ownerunset, NULL);
			}
			if (ircd->protect || ircd->admin) {
				ircdproto->SendSVSModeChan(av[0], ircd->adminunset, NULL);
			}
			for (cu = c->users; cu; cu = next) {
				next = cu->next;
				isop = chan_has_user_status(c, cu->user, CUS_OP);
				isadmin = chan_has_user_status(c, cu->user, CUS_PROTECT);
				isown = chan_has_user_status(c, cu->user, CUS_OWNER);
				count = (isop ? 1 : 0) + (isadmin ? 1 : 0) + (isown ? 1 : 0);

				if (!isop && !isadmin && !isown)
					continue;

				snprintf(tmp, BUFSIZE, "-%s%s%s", (isop ? "o" : ""), (isadmin ?
						ircd->adminunset+1 : ""), (isown ? ircd->ownerunset+1 : ""));

				if (ircdcap->tsmode) {
					snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
					av[1] = buf;
					av[2] = tmp;
					/* We have to give as much nicks as modes.. - Viper */
					for (i = 0; i < count; i++)
						av[i+3] = cu->user->nick;
					ac = 3 + i;
				} else {
					av[1] = tmp;
					/* We have to give as much nicks as modes.. - Viper */
					for (i = 0; i < count; i++)
						av[i+2] = cu->user->nick;
					ac = 2 + i;
				}

				do_cmode(s_ChanServ, ac, av);
			}
		} else {
			av[0] = chan;
			for (cu = c->users; cu; cu = next) {
				next = cu->next;
				isop = chan_has_user_status(c, cu->user, CUS_OP);
				isadmin = chan_has_user_status(c, cu->user, CUS_PROTECT);
				isown = chan_has_user_status(c, cu->user, CUS_OWNER);
				count = (isop ? 1 : 0) + (isadmin ? 1 : 0) + (isown ? 1 : 0);

				if (!isop && !isadmin && !isown)
					continue;

				snprintf(tmp, BUFSIZE, "-%s%s%s", (isop ? "o" : ""), (isadmin ?
						ircd->adminunset+1 : ""), (isown ? ircd->ownerunset+1 : ""));
				/* We need to send the IRCd a nick for every mode.. - Viper */
				snprintf(tmp2, BUFSIZE, "%s %s %s", (isop ? cu->user->nick : ""),
						(isadmin ? cu->user->nick : ""), (isown ? cu->user->nick : ""));

				if (ircdcap->tsmode) {
					snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
					av[1] = buf;
					av[2] = tmp;
					/* We have to give as much nicks as modes.. - Viper */
					for (i = 0; i < count; i++)
						av[i+3] = cu->user->nick;
					ac = 3 + i;

					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[2], tmp2);
				} else {
					av[1] = tmp;
					/* We have to give as much nicks as modes.. - Viper */
					for (i = 0; i < count; i++)
						av[i+2] = cu->user->nick;
					ac = 2 + i;

					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[1], tmp2);
				}

				do_cmode(s_ChanServ, ac, av);
			}
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_OPS, chan);
	} else if (ircd->halfop && stricmp(what, "hops") == 0) {
		const char *av[4];
		int ac;
		char buf[BUFSIZE];
		struct c_userlist *cu, *next;

		for (cu = c->users; cu; cu = next) {
			next = cu->next;
			if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
				continue;

			if (ircdcap->tsmode) {
				snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
				av[0] = chan;
				av[1] = buf;
				av[2] = "-h";
				av[3] = cu->user->nick;
				ac = 4;
			} else {
				av[0] = chan;
				av[1] = "-h";
				av[2] = cu->user->nick;
				ac = 3;
			}

			if (ircd->svsmode_ucmode) {
				if (ircdcap->tsmode)
					ircdproto->SendSVSModeChan(av[0], av[2], NULL);
				else
					ircdproto->SendSVSModeChan(av[0], av[1], NULL);

				do_cmode(s_ChanServ, ac, av);
				break;
			} else {
				if (ircdcap->tsmode)
					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[2],
								   av[3]);
				else
					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[1],
								   av[2]);
			}
			do_cmode(s_ChanServ, ac, av);
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_HOPS, chan);
	} else if (stricmp(what, "voices") == 0) {
		const char *av[4];
		int ac;
		char buf[BUFSIZE];
		struct c_userlist *cu, *next;

		for (cu = c->users; cu; cu = next) {
			next = cu->next;
			if (!chan_has_user_status(c, cu->user, CUS_VOICE))
				continue;

			if (ircdcap->tsmode) {
				snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long int>(time(NULL)));
				av[0] = chan;
				av[1] = buf;
				av[2] = "-v";
				av[3] = cu->user->nick;
				ac = 4;
			} else {
				av[0] = chan;
				av[1] = "-v";
				av[2] = cu->user->nick;
				ac = 3;
			}

			if (ircd->svsmode_ucmode) {
				if (ircdcap->tsmode)
					ircdproto->SendSVSModeChan(av[0], av[2], NULL);
				else
					ircdproto->SendSVSModeChan(av[0], av[1], NULL);

				do_cmode(s_ChanServ, ac, av);
				break;
			} else {
				if (ircdcap->tsmode) {
					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[2],
								   av[3]);
				} else {
					ircdproto->SendMode(whosends(ci), av[0], "%s %s", av[1],
								   av[2]);
				}
			}
			do_cmode(s_ChanServ, ac, av);
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_VOICES, chan);
	} else if (stricmp(what, "users") == 0) {
		const char *av[3];
		struct c_userlist *cu, *next;
		char buf[256];

		snprintf(buf, sizeof(buf), "CLEAR USERS command from %s", u->nick);

		for (cu = c->users; cu; cu = next) {
			next = cu->next;
			av[0] = sstrdup(chan);
			av[1] = sstrdup(cu->user->nick);
			av[2] = sstrdup(buf);
			ircdproto->SendKick(whosends(ci), av[0], av[1], av[2]);
			do_kick(s_ChanServ, 3, av);
			delete [] av[2];
			delete [] av[1];
			delete [] av[0];
		}
		notice_lang(s_ChanServ, u, CHAN_CLEARED_USERS, chan);
	} else {
		syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
	}
	return MOD_CONT;
}

MODULE_INIT("cs_clear", CSClear)
