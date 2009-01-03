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

void myOperServHelp(User * u);

class OSRandomNews : public Module
{
 public:
	OSRandomNews(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion
		("$Id$");
		this->SetType(CORE);

		/** XXX: For some unknown reason, do_randomnews is actaully defined in news.c
		 * we can look at moving it here later
		 **/
		c = createCommand("RANDOMNEWS", do_randomnews, is_services_admin, NEWS_HELP_RANDOM, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_admin(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RANDOMNEWS);
	}
}

MODULE_INIT("os_randomnews", OSRandomNews)
