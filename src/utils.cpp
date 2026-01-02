#include <global.h>

#include <sys/wait.h>
#include <unistd.h>

/**
 *  Simple string split function.
 * 
 *  @param std::string Data to split
 *  @param char Character to split data at
 * 
 *  @return std::vector <std::string> Split array of strings
 */
std::vector <std::string> split_str(std::string data, char c){
    std::vector <std::string> ret;
    std::string current = "";

    for (int i = 0; i < data.size(); i++) {
        if (data[i] != c) {
            current += data[i];
            continue;
        }

        ret.push_back(current);
        current = "";
    }

    if (current.size() > 0) ret.push_back(current);

    return ret;
}

/**
 *  Spawn a process from a given command 
 * 
 *  @param char* Command to spawn
 * 
 *  @return int Error Code
 */
int spawn_process(const char* command) {
    if (fork() == 0) {
        setsid();
        if (fork() != 0) {
            exit(0);
        }
        execvp(command, (char**) NULL);
        exit(0);
    }
    wait(NULL);

    return 0;
}

std::string strip(std::string &str ){
	if  (!(str.length() == 0)) {
		auto w = std::string(" ") ;
		auto n = std::string("\n") ;
		auto r = std::string("\t") ;
		auto t = std::string("\r") ;
		auto v = std::string(1 ,str.front()); 
		while((v == w) or(v==t)or(v==r)or(v==n)) {
			str.erase(str.begin()) ;v = std::string(1 ,str.front()); 
		}
		
		v = std::string(1 , str.back()) ; 

		while((v ==w) or(v==t)or(v==r)or(v==n)) {
			str.erase(str.end() - 1 )  ;v = std::string(1 , str.back()) ;
		}
		
		return str;
	}
	return "";
}

std::string to_upper(std::string text){
	std::string upper;
	for (auto & c: text ) upper += toupper(c);
	return upper;
}

std::string to_lower(std::string text){
	std::string lower;
	for (auto & c: text ) lower += tolower(c);
	return lower;
}