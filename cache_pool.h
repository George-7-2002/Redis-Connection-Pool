/*
 * @Author: your name
 * @Date: 2019-12-07 10:54:57
 * @LastEditTime : 2025-11-11 17:30:00
 * @LastEditors  : Gemini
 * @Description: A C++ Redis Connection Pool (Refactored to Google C++ Style)
 * @FilePath: \src\cache_pool\cache_pool.h
 */
#ifndef CACHEPOOL_H_
#define CACHEPOOL_H_

#include <condition_variable>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <vector>

#include "hiredis.h"

//
// Note: Removed 'using std::...' declarations from header.
// They are disallowed by Google C++ Style Guide as they pollute the namespace.
//

#define REDIS_COMMAND_SIZE 300 /* Max length of a redis command */
#define FIELD_ID_SIZE 100      /* Max length of a redis hash field */
#define VALUES_ID_SIZE 1024    /* Max length of a redis value */
typedef char (
    *RFIELDS)[FIELD_ID_SIZE]; /* Type for batch storing hash fields */

// Pointer to an array, e.g., points to char[1024]
typedef char (
    *RVALUES)[VALUES_ID_SIZE]; /* Type for batch storing values */

class CachePool;

class CacheConn {
  public:
    CacheConn(const char *server_ip, int server_port, int db_index,
              const char *password, const char *pool_name = "");
    explicit CacheConn(CachePool *cache_pool);
    virtual ~CacheConn();

    int Init();
    void DeInit();
    const char *pool_name();
    
    // Common operations
    bool IsExists(const std::string& key);
    long Del(const std::string& key);

    // ------------------- String operations -------------------
    std::string Get(const std::string& key);
    std::string Set(const std::string& key, const std::string& value);
    std::string SetEx(const std::string& key, int timeout, const std::string& value);

    // Batch get
    // Output parameter 'ret_value' is a pointer as per Google style.
    bool MGet(const std::vector<std::string>& keys,
              std::map<std::string, std::string>* ret_value);
    
    // Atomic increment/decrement
    // Output parameter 'value' is a pointer.
    int Incr(const std::string& key, int64_t* value);
    int Decr(const std::string& key, int64_t* value);

    // ---------------- Hash operations ------------------------
    long Hdel(const std::string& key, const std::string& field);
    std::string Hget(const std::string& key, const std::string& field);
    int Hget(const std::string& key, char *field, char *value);
    bool HgetAll(const std::string& key,
                 std::map<std::string, std::string>* ret_value);
    long Hset(const std::string& key, const std::string& field,
              const std::string& value);

    long HincrBy(const std::string& key, const std::string& field, long value);
    long IncrBy(const std::string& key, long value);
    std::string Hmset(const std::string& key,
                      const std::map<std::string, std::string>& hash);
    bool Hmget(const std::string& key, const std::list<std::string>& fields,
               std::list<std::string>* ret_value);

    // ------------ List operations ------------
    long Lpush(const std::string& key, const std::string& value);
    long Rpush(const std::string& key, const std::string& value);
    long Llen(const std::string& key);
    bool Lrange(const std::string& key, long start, long end,
                std::list<std::string>* ret_value);

    // ------------ Zset operations ------------
    int ZsetExit(const std::string& key, const std::string& member);
    int ZsetAdd(const std::string& key, long score, const std::string& member);
    int ZsetZrem(const std::string& key, const std::string& member);
    int ZsetIncr(const std::string& key, const std::string& member);
    int ZsetZcard(const std::string& key);
    int ZsetZrevrange(const std::string& key, int from_pos, int end_pos,
                      RVALUES values, int* get_num);
    int ZsetGetScore(const std::string& key, const std::string& member);

    bool FlushDb();

  private:
    CachePool *cache_pool_;
    redisContext *context_;
    uint64_t last_connect_time_;
    uint16_t server_port_;
    std::string server_ip_;
    std::string password_;
    uint16_t db_index_;
    std::string pool_name_;
};

class CachePool {
  public:
    CachePool(const char *pool_name, const char *server_ip, int server_port,
              int db_index, const char *password, int max_conn_cnt);
    virtual ~CachePool();

    int Init();
    CacheConn *GetCacheConn(const int timeout_ms = 0);
    void RelCacheConn(CacheConn *cache_conn);

    // Accessors
    const char *pool_name() const { return pool_name_.c_str(); }
    const char *server_ip() const { return server_ip_.c_str(); }
    const char *password() const { return password_.c_str(); }
    int server_port() const { return server_port_; }
    int db_index() const { return db_index_; }

  private:
    std::string pool_name_;
    std::string server_ip_;
    std::string password_;
    int server_port_;
    int db_index_;

    int cur_conn_cnt_;
    int max_conn_cnt_;
    std::list<CacheConn *> free_list_;

    std::mutex mutex_;
    std::condition_variable cond_var_;
    bool abort_request_ = false;
};

class CacheManager {
  public:
    virtual ~CacheManager();
    
    static void SetConfPath(const char *conf_path);
    static CacheManager *GetInstance();

    int Init();
    CacheConn *GetCacheConn(const char *pool_name);
    void RelCacheConn(CacheConn *cache_conn);

  private:
    CacheManager();

  private:
    // Static instance pointer
    static CacheManager *instance_;
    
    // Map of connection pools
    std::map<std::string, CachePool *> cache_pool_map_;
    
    // Static configuration path
    static std::string conf_path_;
};

class AutoRelCacheCon {
  public:
    AutoRelCacheCon(CacheManager *manager, CacheConn *conn)
        : manager_(manager), conn_(conn) {}
    ~AutoRelCacheCon() {
        if (manager_) {
            manager_->RelCacheConn(conn_);
        }
    }
  
  private:
    CacheManager *manager_; // Fixed typo: manger_ -> manager_
    CacheConn *conn_;
};

// Macro variable name updated to snake_case
#define AUTO_REL_CACHECONN(m, c) AutoRelCacheCon auto_rel_cache_conn(m, c)

#endif /* CACHEPOOL_H_ */
