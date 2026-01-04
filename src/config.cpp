#include <config.h>
#include <global.h>

#include <fstream>
#include <map>

std::vector <_key_bind_t> config::_key_bind_table = {};

int config::_global_border_width = 1;
int config::_global_border_focus_color = 0xffffffff;
int config::_global_border_unfocus_color = 0;
int config::_global_extend_amount = 10;

std::map <std::string, unsigned int> _key_mod_bind = {
    { "alt",     Mod1Mask },
    { "meta",    Mod4Mask },
    { "win",     Mod4Mask },
    { "ctrl",    ControlMask },
    { "control", ControlMask }
};

/**
 *  Load a given config file
 * 
 *  @todo: This would be much better of getting read from a JSON File.
 *  @note: This function is really really not protected and should be
 *         either removed, or cleaned before uploading to the internet.
 * 
 *  @param char* Config file-path
 * 
 *  @return Error code
 */
int config::init(char* filepath){
    std::ifstream _file_read(filepath);

    if (!_file_read.is_open()){
        printf("[ SEWM ] ! Failed to read config file `%s`\n", filepath);
        return -1;
    }

    std::string cline = "";
    while (getline(_file_read, cline)){
        if (cline.size() == 0)
            continue;

        std::vector <std::string> line_split = split_str(cline, ':');
        
        /**
         *  Very basic error check
         */
        if (line_split.size() < 1){
            printf(" [ SEWM ] ! Invalid config\n");
            return -1;
        }

        /**
         *  Process configuration variables
         *
         *  @todo: This is a truly horrible way of doing this, do it a
         *         different way at some point.
         */
        if (line_split[0] == "extend-amount"){
            config::_global_extend_amount = atoi(strip(line_split[1]).c_str());
        }

        /**
         *  KEY-BIND & KEY-BIND-EXEC
         * 
         *  Set key binds for internal & external commands.
         *  (Eg: CTRL+X closes current window, CTRL+T loads terminal)
         */
        if (line_split[0] == "key-bind-exec" ||
            line_split[0] == "key-bind"
        ){
            std::vector <std::string> bind_split = split_str(line_split[1], '=');
            std::vector <std::string> keyc_split = split_str(bind_split[0], '+');

            // Slight error management
            if (bind_split.size() < 1 || keyc_split.size() < 1){
                printf(" [ SEWM ] ! Invalid config\n");
                return -1;
            }

            /**
             *  Remove white space from config values as we can't assume
             *  people have written stuff exactly how we want.
             * 
             *  @fix: This may break exec commands provided, but it's not
             *        likely.
             */
            for (int i = 0; i < bind_split.size(); i++)
                bind_split[i] = to_lower(strip(bind_split[i]));

            for (int i = 0; i < keyc_split.size(); i++)
                keyc_split[i] = strip(keyc_split[i]);

            _key_bind_t _new_bind;
            _new_bind._key = keyc_split[1];
            _new_bind._command = bind_split[1];
            _new_bind._mod = 0;

            std::vector <std::string> mod_split = split_str(keyc_split[0], '-');
            for (int i = 0; i < mod_split.size(); i++)
                _new_bind._mod |= _key_mod_bind[to_lower(mod_split[i])];
            
            if (line_split[0] == "key-bind-exec") _new_bind._type = _KEY_BIND_TYPE_EXEC;
            if (line_split[0] == "key-bind") _new_bind._type = _KEY_BIND_TYPE_FUNC;

            config::_key_bind_table.push_back(_new_bind);
        
        /**
         *  STARTUP
         * 
         *  Process startup commands to run on window manager intialization.
         *  (Eg: set wallpaper, load task bar)
         */
        } else if (line_split[0] == "startup"){
            system(strip(line_split[1]).c_str());
        }
    }

    _file_read.close();

    return 0;
}
