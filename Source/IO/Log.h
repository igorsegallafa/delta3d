#pragma once

namespace Delta3D::IO
{

enum class LogLevel
{
	None = -1,

	Debug = 1 << 0,
	Error = 1 << 1,
	Info = 1 << 2,
};	DEFINE_ENUM_FLAG_OPERATORS( LogLevel );

class Log
{
public:
	//! Default Constructor.
	Log();

	//! Deconstructor.
	~Log() = default;

	//! Set if log will be written with timestamp.
	void SetTimestamp( bool timestamp_ ) { timestamp = timestamp_; }

	//! Get if timestamp is enabled.
	const bool Timestamp() const { return timestamp; }

	//! Set a new Log Level.
	void SetLogLevel( const LogLevel& level_ ) { level = level_; }

	//! Get instance from Log Object.
	static Log* Get() { if( !instance ) instance = new Log(); return instance; }

	inline std::string GetLogAsString( LogLevel logLevel ) const;

	/**
	 * Write a Log based on your level
	 * @param logLevel Level of the log being written
	 * @param format Formatted string
	 */
	void Write( LogLevel logLevel, const char* format, ... );

	/**
	 * Write a Log on a specified file
	 * @param fileName Name of file where log will be written
	 * @param line Formatted string
	 */
	static void WriteFile( std::string fileName, std::string line );

	bool timestamp;
	LogLevel level;	//!< Log Level
	std::mutex mutex;	//!< Mutex for thread safety 
	static Log* instance;	//!< Log Object instance
};
}

#define DELTA3D_LOGDEBUG( format, ... ) Delta3D::IO::Log::Get()->Write( Delta3D::IO::LogLevel::Debug, format, ##__VA_ARGS__)
#define DELTA3D_LOGERROR( format, ... ) Delta3D::IO::Log::Get()->Write( Delta3D::IO::LogLevel::Error, format, ##__VA_ARGS__)
#define DELTA3D_LOGINFO( format, ... ) Delta3D::IO::Log::Get()->Write( Delta3D::IO::LogLevel::Info, format, ##__VA_ARGS__)