struct cmd {
        std::map<std::string, cmd> sub_cmds; // cmds below this one
        generic_callback_t f; // func for this cmd or NULL
        std::string desc;

        bool find(cmd&, std::vector<std::string>&, std::vector<std::string>&, bool = false) const;
        bool add(const std::string&, generic_callback_t, const std::string&);
        bool add(std::vector<std::string>&, generic_callback_t, const std::string&);
        bool addlist(cmd_init_list);

        cmd() : f(0) {}
};


bool cmd::add(std::vector<std::string>& args, generic_callback_t f, const std::string& desc)
{
        printf("adding: [%s]\n", args[0].c_str());
        if (args.size() == 1) {
                printf("add %s: f = %p\n", args[0].c_str(), f);
                sub_cmds[args[0]].f = f;
                sub_cmds[args[0]].desc = desc;
                return true;
        }

        std::string s = args[0];
        args.erase(args.begin());
        sub_cmds[s].add(args, f, desc);
        return true;
}

bool cmd::add(const std::string& ncmd, generic_callback_t f, const std::string& desc)
{
        std::vector<std::string> args;
        str::tokenise(ncmd, args);
        return add(args, f, desc);
}

bool cmd::find(cmd& ret, std::vector<std::string>& argret,
               std::vector<std::string> & cmds, bool incomp) const
{
        std::map<std::string, cmd>::const_iterator it = sub_cmds.find(cmds[0]);

        printf("searching: [%s] this=%p\n", cmds[0].c_str(), this);

        if (it == sub_cmds.end()) {
                do {
                        printf("check !R\n");
                        // might have !R
                        if ((it = sub_cmds.find("!R")) != sub_cmds.end()) {
                                std::stringstream s;
                                std::copy(cmds.begin(), cmds.end(), std::ostream_iterator<std::string>(s, " "));
                                argret.push_back(s.str());
                                cmds.clear();
                                cmds.push_back(s.str());
                                break;
                        }

                        printf("check !S\n");
                        // might have !S
                        if ((it = sub_cmds.find("!S")) != sub_cmds.end()) {
                                argret.push_back(cmds[0]);
                                break;
                        }

                        printf("check abbr\n");
                        // might be abbrv
                        std::map<std::string, cmd>::const_iterator low_it, high_it;
                        low_it = sub_cmds.lower_bound(cmds[0]);
                        high_it = sub_cmds.upper_bound(cmds[0]);

                        if (low_it == high_it && low_it != sub_cmds.end() &&
                            (low_it->first.substr(0, cmds[0].size()) == cmds[0])) {
                                it = low_it;
                                break;
                        }

                        printf("nope\n");
                        return false;
                } while (0);
        }

        printf ("size=%d\n", cmds.size());
        if (cmds.size() != 1) {
                cmds.erase(cmds.begin());
                return it->second.find(ret, argret, cmds, incomp);
        }


        // size == 1 means this must be cmd
        if (!it->second.f && !incomp) {
                printf("no f and !incomp (incomp = %d)\n", incomp);
                return false;
        }

        ret = it->second;
        printf("find: f = %p\n", ret.f);
        return true;
}