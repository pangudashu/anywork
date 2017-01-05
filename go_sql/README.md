# golang sql连接池的实现解析

golang的"database/sql"是操作数据库时常用的包，这个包定义了一些sql操作的接口，具体的实现还需要不同数据库的实现，mysql比较优秀的一个驱动是：`github.com/go-sql-driver/mysql`，在接口、驱动的设计上"database/sql"的实现非常优秀，对于类似设计有很多值得我们借鉴的地方，比如beego框架cache的实现模式就是借鉴了这个包的实现；"database/sql"除了定义接口外还有一个重要的功能：__连接池__，我们在实现其他网络通信时也可以借鉴其实现。

连接池的重要性这里就不再多说了，下面从一个普通的示例看下"database/sql"的连接池是如何实现的：
```
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

