struct optstruct {
    char *name;
    char *cmd;
    char *strarg;
    long long numarg;
    int enabled;
    int active;
    int flags;
    int idx;
    struct optstruct *nextarg;
    struct optstruct *next;

    char **filename; /* cmdline */
};


struct main_option {
    const char *name;
    const char *longopt;
    char shortopt;
    int argtype;
    const char *regex;
    long long numarg;
    const char *strarg;
    int flags;
    int misc;
    const char *description;

};

const struct main_option __m_options[] = {
    /* name, longopt,       shortopt,   argtype,        regex,      num,    str,             flags,                 misc             description */

    /* cmdline only */
    { NULL, "help",         'h',    TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0,                "" },
    { NULL, "config-file",  'f',    TYPE_STRING,    NULL,           0,      pm_configfile,      IS_REQUIRED,        0 ,               "" },
    { NULL, "version",      'V',    TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0 ,               "" },
    { NULL, "debug",         0,     TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0,                "" },
    { NULL, "verbose",      'v',    TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0,                "" },
    { NULL, "quiet",        0,      TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0,                "" },
    { NULL, "stdout",       0,      TYPE_BOOL,      MATCH_BOOL,     0,      NULL,               0,                  0,                "" }

}
const struct main_option *m_options = __m_options;
const struct optstruct *optget(const struct optstruct *opts, const char *name)
{
    while(opts) {
	if((opts->name && !strcmp(opts->name, name)) || (opts->cmd && !strcmp(opts->cmd, name)))
	    return opts;
	opts = opts->next;
    }
    return NULL;
}
struct optstruct *optparse(const char *cfgfile, int argc, char **argv, int verbose, int toolmask, int ignore, struct optstruct *oldopts)
{
	FILE *fs = NULL;
	const struct main_option *optentry;
	char *pt;
	const char *name = NULL, *arg;
	int i, err = 0, lc = 0, sc = 0, opt_index, line = 0, ret;
	struct optstruct *opts = NULL, *opts_last = NULL, *opt;
	char buffer[512], *buff;
	struct option longopts[MAXCMDOPTS];
	char shortopts[MAXCMDOPTS];
	regex_t regex;
	long long numarg, lnumarg;
	int regflags = REG_EXTENDED | REG_NOSUB;


    if(oldopts)
	opts = oldopts;

    shortopts[sc++] = ':';
    for(i = 0; ; i++) {
	optentry = &m_options[i];
	if(!optentry->name && !optentry->longopt)
	    break;

	if(((optentry->owner & toolmask) && ((optentry->owner & toolmask) != OPT_DEPRECATED)) || (ignore && (optentry->owner & ignore))) {
	    if(!oldopts && optadd(&opts, &opts_last, optentry->name, optentry->longopt, optentry->strarg, optentry->numarg, optentry->flags, i) < 0) {
		fprintf(stderr, "ERROR: optparse: Can't register new option (not enough memory)\n");
		optfree(opts);
		return NULL;
	    }

	    if(!cfgfile) {
		if(optentry->longopt) {
		    if(lc >= MAXCMDOPTS) {
			fprintf(stderr, "ERROR: optparse: longopts[] is too small\n");
			optfree(opts);
			return NULL;
		    }
		    longopts[lc].name = optentry->longopt;
		    if(!(optentry->flags & FLAG_REQUIRED) && (optentry->argtype == TYPE_BOOL || optentry->strarg))
			longopts[lc].has_arg = 2;
		    else
			longopts[lc].has_arg = 1;
		    longopts[lc].flag = NULL;
		    longopts[lc++].val = optentry->shortopt;
		}
		if(optentry->shortopt) {
		    if(sc + 2 >= MAXCMDOPTS) {
			fprintf(stderr, "ERROR: optparse: shortopts[] is too small\n");
			optfree(opts);
			return NULL;
		    }
		    shortopts[sc++] = optentry->shortopt;
		    if(optentry->argtype != TYPE_BOOL) {
			shortopts[sc++] = ':';
			if(!(optentry->flags & FLAG_REQUIRED) && optentry->strarg)
			    shortopts[sc++] = ':';
		    }
		}
	    }
	}
    }

    if(cfgfile) {
	if((fs = fopen(cfgfile, "r")) == NULL) {
	    /* don't print error messages here! */
	    optfree(opts);
	    return NULL;
	}
    } else {
	if(MAX(sc, lc) > MAXCMDOPTS) {
	    fprintf(stderr, "ERROR: optparse: (short|long)opts[] is too small\n");
	    optfree(opts);
	    return NULL;
	}
	shortopts[sc] = 0;
	longopts[lc].name = NULL;
	longopts[lc].flag = NULL;
	longopts[lc].has_arg = longopts[lc].val = 0;
    }

    while(1) {

	if(cfgfile) {
	    if(!fgets(buffer, sizeof(buffer), fs))
		break;

	    buff = buffer;
	    for(i = 0; i < (int) strlen(buff) - 1 && (buff[i] == ' ' || buff[i] == '\t'); i++);
	    buff += i;
	    line++;
	    if(strlen(buff) <= 2 || buff[0] == '#')
		continue;

	    if(!strncmp("Example", buff, 7)) {
		if(verbose)
		    fprintf(stderr, "ERROR: Please edit the example config file %s\n", cfgfile);
		err = 1;
		break;
	    }

	    if(!(pt = strpbrk(buff, " \t"))) {
		if(verbose)
		    fprintf(stderr, "ERROR: Missing argument for option at line %d\n", line);
		err = 1;
		break;
	    }
	    name = buff;
	    *pt++ = 0;
	    for(i = 0; i < (int) strlen(pt) - 1 && (pt[i] == ' ' || pt[i] == '\t'); i++);
	    pt += i;
	    for(i = strlen(pt); i >= 1 && (pt[i - 1] == ' ' || pt[i - 1] == '\t' || pt[i - 1] == '\n'); i--);
	    if(!i) {
		if(verbose)
		    fprintf(stderr, "ERROR: Missing argument for option at line %d\n", line);
		err = 1;
		break;
	    }
	    pt[i] = 0;
	    arg = pt;
	    if(*arg == '"') {
		arg++; pt++;
		pt = strrchr(pt, '"');
		if(!pt) {
		    if(verbose)
			fprintf(stderr, "ERROR: Missing closing parenthesis in option %s at line %d\n", name, line);
		    err = 1;
		    break;
		}
		*pt = 0;
		if(!strlen(arg)) {
		    if(verbose)
			fprintf(stderr, "ERROR: Empty argument for option %s at line %d\n", name, line);
		    err = 1;
		    break;
		}
	    }

	} else {
	    opt_index = 0;
	    ret = my_getopt_long(argc, argv, shortopts, longopts, &opt_index);
	    if(ret == -1)
		break;

	    if(ret == ':') {
		fprintf(stderr, "ERROR: Incomplete option passed (missing argument)\n");
		err = 1;
		break;
	    } else if(!ret || strchr(shortopts, ret)) {
		name = NULL;
		if(ret) {
		    for(i = 0; i < lc; i++) {
			if(ret == longopts[i].val) {
			    name = longopts[i].name;
			    break;
			}
		    }
		} else {
		    name = longopts[opt_index].name;
		}
		if(!name) {
		    fprintf(stderr, "ERROR: optparse: No corresponding long name for option '-%c'\n", (char) ret);
		    err = 1;
		    break;
		}
		optarg ? (arg = optarg) : (arg = NULL);
	    } else {
		fprintf(stderr, "ERROR: Unknown option passed\n");
		err = 1;
		break;
	    }
	}

	if(!name) {
	    fprintf(stderr, "ERROR: Problem parsing options (name == NULL)\n");
	    err = 1;
	    break;
	}

	opt = optget_i(opts, name);
	if(!opt) {
	    if(cfgfile) {

		    fprintf(stderr, "ERROR: Parse error at line %d: Unknown option %s\n", line, name);
	    }
	    err = 1;
	    break;
	}
	optentry = &m_options[opt->idx];



	if(!cfgfile && !arg && optentry->argtype == TYPE_BOOL) {
	    arg = "yes"; /* default to yes */
	}
	else if(optentry->regex) {

	}

	numarg = -1;
	switch(optentry->argtype) {
	    case TYPE_STRING:
		if(!arg)
		    arg = optentry->strarg;
		if(!cfgfile && !strlen(arg)) {
		    if(optentry->shortopt)
			fprintf(stderr, "ERROR: Option --%s (-%c) requires a non-empty string argument\n", optentry->longopt, optentry->shortopt);
		    else
			fprintf(stderr, "ERROR: Option --%s requires a non-empty string argument\n", optentry->longopt);
		    err = 1;
		    break;
		}
		break;

	    case TYPE_NUMBER:
		numarg = atoi(arg);
		arg = NULL;
		break;

	    case TYPE_SIZE:
		errno = 0;
		lnumarg = strtoul(arg, &buff, 0);
		if(errno != ERANGE) {

		}

		arg = NULL;
		if(err) break;
		if(errno == ERANGE) {
		      fprintf(stderr, "WARNING: Numerical value for option %s too high!\n", optentry->longopt);
		    }
		  lnumarg = UINT_MAX;
		}

		numarg = lnumarg ? lnumarg : UINT_MAX;
		break;

	    case TYPE_BOOL:
                if(!strcasecmp(arg, "yes") || !strcmp(arg, "1") || !strcasecmp(arg, "true"))
		    numarg = 1;
		else
		    numarg = 0;

		arg = NULL;
		break;
	}

	if(err)
	    break;

	if(optaddarg(opts, name, arg, numarg) < 0) {
	    if(cfgfile)
		fprintf(stderr, "ERROR: Can't register argument for option %s\n", name);
	    else
		fprintf(stderr, "ERROR: Can't register argument for option --%s\n", optentry->longopt);
	    err = 1;
	    break;
	}
    }

    if(fs)
	fclose(fs);

    if(err) {
	optfree(opts);
	return NULL;
    }

    if(!cfgfile && (optind < argc)) {
	opts->filename = (char **) calloc(argc - optind + 1, sizeof(char *));
	if(!opts->filename) {
	    fprintf(stderr, "ERROR: optparse: calloc failed\n");
	    optfree(opts);
	    return NULL;
	}
        for(i = optind; i < argc; i++) {
	    opts->filename[i - optind] = strdup(argv[i]);
	    if(!opts->filename[i - optind]) {
		fprintf(stderr, "ERROR: optparse: strdup failed\n");
		optfree(opts);
		return NULL;
	    }
	}
    }

    /* optprint(opts); */

    return opts;
}

