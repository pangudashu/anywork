# golang sql连接池的实现解析

golang的"database/sql"是操作数据库时常用的包，这个包定义了一些sql操作的接口，具体的实现还需要不同数据库的实现，mysql比较优秀的一个驱动是：`github.com/go-sql-driver/mysql`，在接口、驱动的设计上"database/sql"的实现非常优秀，对于类似设计有很多值得我们借鉴的地方，比如beego框架cache的实现模式就是借鉴了这个包的实现；"database/sql"除了定义接口外还有一个重要的功能：__连接池__，我们在实现其他网络通信时也可以借鉴其实现。

连接池的重要性这里就不再多说了，我们先从一个简单的示例看下"database/sql"怎么用：
```go
package main

import(
    "fmt"
    "database/sql"
    _ "github.com/go-sql-driver/mysql"
)

func main(){
    
    db, err := sql.Open("mysql", "username:password@tcp(host)/db_name?charset=utf8&allowOldPasswords=1")
    if err != nil {
        fmt.Println(err)
        return
    }
    defer db.Close()

    rows,err := db.Query("select * from test")
    
    for rows.Next(){
        //row.Scan(...)
    }
    rows.Close()
}
```
用法很简单，首先Open打开一个数据库，然后调用Query、Exec进行数据库操作，`github.com/go-sql-driver/mysql`具体实现了`database/sql/driver`的接口，所以最终具体的数据库操作都是调用`github.com/go-sql-driver/mysql`实现的方法，另外需要注意的是同一个数据库只需要调用一次Open，下面根据具体的操作分析下"database/sql"都干了哪些事。

## 1.驱动注册
`import _ "github.com/go-sql-driver/mysql"`前面的"_"作用时不需要把该包都导进来，只执行包的`init()`方法，mysql驱动正是通过这种方式注册到"database/sql"中的：
```go
//github.com/go-sql-driver/mysql/driver.go
func init() {
    sql.Register("mysql", &MySQLDriver{})
}

type MySQLDriver struct{}

func (d MySQLDriver) Open(dsn string) (driver.Conn, error) {
    ...
}
```
`init()`通过`Register()`方法将mysql驱动添加到`sql.drivers`(类型：make(map[string]driver.Driver))中,MySQLDriver实现了`driver.Driver`接口：
```go
//database/sql/sql.go
func Register(name string, driver driver.Driver) {
    driversMu.Lock()
    defer driversMu.Unlock()
    if driver == nil {
        panic("sql: Register driver is nil")
    }
    if _, dup := drivers[name]; dup {
        panic("sql: Register called twice for driver " + name)
    }
    drivers[name] = driver
}

//database/sql/driver/driver.go
type Driver interface {
    // Open returns a new connection to the database.
    // The name is a string in a driver-specific format.
    //
    // Open may return a cached connection (one previously
    // closed), but doing so is unnecessary; the sql package
    // maintains a pool of idle connections for efficient re-use.
    //
    // The returned connection is only used by one goroutine at a
    // time.
    Open(name string) (Conn, error)
}

```

## 2.获取可用连接

## 3.连接释放
