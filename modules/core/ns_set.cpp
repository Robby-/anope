/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "nickserv.h"

class CommandNSSet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandNSSet() : Command("SET", 1, 3)
	{
		this->SetDesc(_("Set options, including kill protection"));
	}

	~CommandNSSet()
	{
		this->subcommands.clear();
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (readonly)
		{
			source.Reply(_(NICK_SET_DISABLED));
			return MOD_CONT;
		}

		if (u->Account()->HasFlag(NI_SUSPENDED))
		{
			source.Reply(_(NICK_X_SUSPENDED), u->Account()->display.c_str());
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[0]);

		if (c)
		{
			Anope::string cmdparams = u->Account()->display;
			for (std::vector<Anope::string>::const_iterator it = params.begin() + 1, it_end = params.end(); it != it_end; ++it)
				cmdparams += " " + *it;
			/* Don't log the whole message for set password */
			if (c->name != "PASSWORD")
				Log(LOG_COMMAND, u, this) << params[0] << " " << cmdparams;
			else
				Log(LOG_COMMAND, u, this) << params[0];
			mod_run_cmd(nickserv->Bot(), u, NULL, c, params[0], cmdparams);
		}
		else
			source.Reply(_(NICK_SET_UNKNOWN_OPTION), Config->UseStrictPrivMsgString.c_str(), params[0].c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(_("Syntax: \002SET \037option\037 \037parameters\037\002\n"
					" \n"
					"Sets various nickname options.  \037option\037 can be one of:"));
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(source);
			source.Reply(_("In order to use this command, you must first identify\n"
					"with your password (\002%s%s HELP IDENTIFY\002 for more\n"
					"information).\n"
					" \n"
					"Type \002%s%s HELP SET \037option\037\002 for more information\n"
					"on a specific option."), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), (Config->UseStrictPrivMsg ? "/msg " : "/"), Config->s_NickServ.c_str());
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
				return c->OnHelp(source, subcommand);
		}

		return false;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", _(NICK_SET_SYNTAX));
	}

	bool AddSubcommand(Module *creator, Command *c)
	{
		c->module = creator;
		c->service = this->service;
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(const Anope::string &command)
	{
		return this->subcommands.erase(command);
	}

	Command *FindCommand(const Anope::string &subcommand)
	{
		subcommand_map::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
			return it->second;

		return NULL;
	}
};

class CommandNSSetDisplay : public Command
{
 public:
	CommandNSSetDisplay() : Command("DISPLAY", 2)
	{
		this->SetDesc(_("Set the display of your group in Services"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(params[1]);

		if (!na || na->nc != u->Account())
		{
			source.Reply(_(NICK_SASET_DISPLAY_INVALID), u->Account()->display.c_str());
			return MOD_CONT;
		}

		change_core_display(u->Account(), params[1]);
		source.Reply(_(NICK_SET_DISPLAY_CHANGED), u->Account()->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET DISPLAY \037new-display\037\002\n"
				" \n"
				"Changes the display used to refer to your nickname group in \n"
				"Services. The new display MUST be a nick of your group."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", _(NICK_SET_SYNTAX));
	}
};

class CommandNSSetPassword : public Command
{
 public:
	CommandNSSetPassword() : Command("PASSWORD", 2)
	{
		this->SetDesc(_("Set your nickname password"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &param = params[1];
		unsigned len = param.length();

		if (u->Account()->display.equals_ci(param) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(_(MORE_OBSCURE_PASSWORD));
			return MOD_CONT;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(_(PASSWORD_TOO_LONG));
			return MOD_CONT;
		}

		enc_encrypt(param, u->Account()->pass);
		Anope::string tmp_pass;
		if (enc_decrypt(u->Account()->pass, tmp_pass) == 1)
			source.Reply(_(NICK_SASET_PASSWORD_CHANGED_TO), u->Account()->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(_(NICK_SASET_PASSWORD_CHANGED), u->Account()->display.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET PASSWORD \037new-password\037\002\n"
				" \n"
				"Changes the password used to identify you as the nick's\n"
				"owner."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", _(NICK_SET_SYNTAX));
	}
};

class NSSet : public Module
{
	CommandNSSet commandnsset;
	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSetPassword commandnssetpassword;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		this->AddCommand(nickserv->Bot(), &commandnsset);

		commandnsset.AddSubcommand(this, &commandnssetdisplay);
		commandnsset.AddSubcommand(this, &commandnssetpassword);
	}
};

MODULE_INIT(NSSet)
