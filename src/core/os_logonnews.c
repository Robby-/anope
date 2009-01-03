/* OperServ core functions
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

Command *c;

void myOperServHelp(User * u);
int load_config();
int reload_config(int argc, char **argv);

class OSLogonNews : public Module
{
 public:
	OSLogonNews(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		EvtHook *hook;
		char buf[BUFSIZE];

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		/**
		* For some unknown reason, do_logonnews is actaully defined in news.c
		* we can look at moving it here later
		**/
		c = createCommand("LOGONNEWS", do_logonnews, is_services_admin,
		NEWS_HELP_LOGON, -1, -1, -1, -1);
		snprintf(buf, BUFSIZE, "%d", NewsCount),
		c->help_param1 = sstrdup(buf);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);

		hook = createEventHook(EVENT_RELOAD, reload_config);
		if (this->AddEventHook(hook) != MOD_ERR_OK)
		{
			throw ModuleException("os_logonnews: Can't hook to EVENT_RELOAD event");
		}
	}

	~OSLogonNews()
	{
		delete [] c->help_param1;
	}
};



/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_admin(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_LOGONNEWS);
	}
}


/**
 * Upon /os reload refresh the count
 **/
int reload_config(int argc, char **argv) {
	char buf[BUFSIZE];

	if (argc >= 1) {
		if (!stricmp(argv[0], EVENT_START)) {
			delete [] c->help_param1;
			snprintf(buf, BUFSIZE, "%d", NewsCount),
			c->help_param1 = sstrdup(buf);
		}
	}

	return MOD_CONT;
}

/* EOF */

MODULE_INIT("os_logonnews", OSLogonNews)
