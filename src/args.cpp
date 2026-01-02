#include <args.h>
#include <config.h>

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <map>

std::map <std::string, std::vector <std::string>> _arg_alias_map = {
    { "version", {"--version", "-v"} },
    { "config",  {"--config",  "-c"} }
};

/**
 *  Quick and dirty function to check if a value has been
 *  provided after an option.
 * 
 *  @todo: This could be more elaborate but it's fine for now.
 * 
 *  @param int Index within argv
 *  @param int Argc from program
 *  @param char* String to print on error
 * 
 *  @return Error code ( 0 == Fine :: -1 == Fail )
 */
int error_check(int i, int argc, const char* err_str){
    if (i + 1 >= argc){
        printf(" [ SEWM ] ! %s\n", err_str);
        return -1;
    }
    return 0;
}

/**
 *  Check argument against all possible aliases.
 *
 *  @param std::string Key in _arg_alias_map
 *  @param const char* Argument to check
 *
 *  @return (0 == match || -1 == no match)
 */
int check_alias(std::string key, const char* arg){
    if (_arg_alias_map.find(key) == _arg_alias_map.end())
        return -1;

    for (int i = 0; i < _arg_alias_map[key].size(); i++){
        if (strcmp(arg, _arg_alias_map[key][i].c_str()) == 0)
            return 0;
    }

    return -1;
}

/**
 *  Process argv & argc passed to main
 * 
 *  @todo: This most likely isn't the best way to process arguments,
 *         however, it's not too bad and fairly compact.
 *
 *  @param int argc
 *  @param int argv
 */
void wm::process_args(int argc, char** argv){
    for (int i = 0; i < argc; i++){
        /**
         *  --version -v
         *  Print version string
         */
        if (check_alias("version", argv[i]) == 0){
            printf("SEWM :: 0.0.1 :: build 42925\n");
            exit(1);

        /**
         *  --config -c
         *  Load with config file
         */
        } else if (check_alias("config", argv[i]) == 0) {
            if (error_check(i, argc, "Config file not provided") != 0)
                exit(-1);

            config::init(argv[i + 1]);
            i++;
        }

    }
}
