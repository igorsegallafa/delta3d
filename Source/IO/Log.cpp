#include "PrecompiledHeader.h"
#include "Log.h"

namespace Delta3D::IO
{

Log* Log::instance = nullptr;

std::unordered_map<LogLevel, std::string> logFiles = {	{ LogLevel::Debug, "save/logs/delta3d.log" },
															{ LogLevel::Error, "save/logs/delta3d.log" },
															{ LogLevel::Info, "save/logs/delta3d.log" } };

Log::Log() : level( LogLevel::Info | LogLevel::Error ), timestamp( true )
{
	for( const auto& file : logFiles )
		DeleteFileA( file.second.c_str() );
}

inline std::string Log::GetLogAsString( LogLevel logLevel ) const
{
	switch( logLevel )
	{
		case LogLevel::Debug:
			return "DBG";
		case LogLevel::Error:
			return "ERR";
		case LogLevel::Info:
			return "INF";
	}

	return "UNK";
}

void Log::Write( LogLevel logLevel, const char* format, ... )
{
	mutex.lock();

	//Check if can write the log
	if( (level& logLevel) != LogLevel::None )
	{
		va_list args;
		va_start( args, format );
		int length = _vscprintf( format, args ) + 1;
		char* line = new char[length];
		vsprintf_s( line , length, format, args );

		//Use as std string
		std::ostringstream logLine;
	
		//Check if timestamp is enabled
		if( timestamp )
		{
			std::time_t t = std::time( nullptr );
			std::tm tm;

			localtime_s( &tm, &t );
			logLine << std::put_time( &tm, "%d/%m/%Y %H:%M:%S" ) << " - ";
		}

		logLine << "(" << GetLogAsString( logLevel ) << ") " << line;

		//Write Formatted Log
		WriteFile( logFiles[logLevel], logLine.str() );
		va_end( args );

		delete[] line;
	}

	mutex.unlock();
}

void Log::WriteFile( std::string fileName, std::string line )
{
	std::ofstream log( fileName, std::ios_base::out | std::ios_base::app );

	if( log.is_open() )
	{
		log << line << std::endl;
		log.close();
	}
}

}