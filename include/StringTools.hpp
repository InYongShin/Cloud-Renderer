#ifndef StringTools_hpp
#define StringTools_hpp


#include <string>
#include <fstream>
#include <iostream>


inline std::string backToFrontSlash( const std::string& filename ) {
	std::string ret = filename;
	for( int i=0; i<ret.size(); i++ )
	if( ret[i]=='\\' ) ret[i] = '/';
	return ret;
}

inline std::string getFilenameFromAbsPath( const std::string& filename )
{
	size_t slashPos = filename.find_last_of('/');
	if( slashPos == std::string::npos ) return filename;
	if( slashPos == filename.length()-1 ) return "";
	return filename.substr(slashPos+1);
}

inline std::string loadText(const std::string& filename) {
	std::ifstream t(backToFrontSlash(filename));
	if( !t.is_open() ) {
		std::cerr<<"[ERROR] Text file: "<<getFilenameFromAbsPath(filename)<<" is not found\n";
		return "";
	}
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}


#endif /* StringTools_hpp */
