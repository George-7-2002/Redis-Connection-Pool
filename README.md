# Redis Connection Pool for basic backend development
A lightweight, thread-safe, and multi-instance Redis connection pool written in C++. Designed for high-performance backend services, it provides an efficient way to manage and reuse Redis connections.  This project is built with C++11 features  and wraps the hiredis C library for Redis communication.
# Key Features
Connection Pooling: Manages a free_list_ of reusable CacheConn objects to avoid the high overhead of creating a new TCP connection for every Redis command.

Dynamic Sizing: If the pool is empty but has not reached its maximum capacity (max_conn_cnt_), it will dynamically create new connections to handle load.

RAII Resource Management: Includes an AutoRelCacheCon helper class. When you use the AUTO_REL_CACHECONN macro, the connection is guaranteed to be returned to the pool when the variable goes out of scope, preventing connection leaks.

Multi-Instance Support: The CacheManager singleton can parse a configuration file and manage multiple, named CachePool instances. This allows a single application to easily connect to different Redis servers or databases.

External Configuration: Pool instances (host, port, db, max connections) are loaded from a text-based .conf file, not hard-coded.

Singleton Management: Core services like CacheManager and DLog are provided as singletons, accessible via a GetInstance() method.

Advanced Logging: Integrates the spdlog library via a DLog wrapper. It is pre-configured for both console output and daily-rotating file logs.

Automatic Reconnection: The CacheConn::Init() method includes logic to automatically attempt reconnection if the redisContext is lost, with a simple 1-second throttle to prevent spamming.
# Project Structure
The project is broken down into several key components:

## CacheConn
 The lowest level. This class wraps a single redisContext (from hiredis). It handles the logic for connecting, authenticating, selecting a DB, and executing raw Redis commands.

## CachePool
Manages a free_list_ of CacheConn objects for one specific Redis instance. It contains the core thread-safe logic for GetCacheConn() and RelCacheConn().

## CacheManager 
A singleton that acts as the main entry point for the application. It holds a std::map to manage all available CachePools by name.

## DLog 
A singleton wrapper for spdlog that provides a globally accessible logger.

## CConfigFileReader
A simple utility class to parse key-value pairs from a configuration file.

## util
Provides various helper functions, such as CStrExplode for string splitting and CRefObject for reference counting.

# Dependencies
A C++11 (or higher) compliant compiler

redis (version>4.1.0)

spdlog library (e.g., libspdlog-dev)

