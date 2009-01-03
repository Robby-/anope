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

int do_modunload(User * u);

void myOperServHelp(User * u);

class OSModUnLoad : public Module
{
 public:
	OSModUnLoad(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->SetPermanent(true);

		c = createCommand("MODUNLOAD", do_modunload, is_services_root, -1, -1, -1, -1, OPER_HELP_MODUNLOAD);
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
	if (is_services_root(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODUNLOAD);
	}
}

/**
 * The /os modunload command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_modunload(User *u)
{
	char *name;
	int status;

	name = strtok(NULL, "");
	if (!name)
	{
		syntax_error(s_OperServ, u, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
		return MOD_CONT;
	}

	Module *m = findModule(name);
	if (!m)
	{
		syntax_error(s_OperServ, u, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
		return MOD_CONT;
	}

	alog("Trying to unload module [%s]", name);

	status = ModuleManager::UnloadModule(m, u);

	if (status != MOD_ERR_OK)
	{
		notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, name);
	}

	return MOD_CONT;
}

MODULE_INIT("os_modunload", OSModUnLoad)
