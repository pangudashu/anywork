package main

import (
	"fmt"
	"handler"
	"routine_pool"
	"time"
)

func main() {
	pool1 := routine_pool.NewPool("my test")
	if err := pool1.SetMode(routine_pool.MODE_ONDEMOND, 10, 10, 10, 30, 5); err != nil {
		//if err := pool1.SetMode(routine_pool.MODE_DYNAMIC, 10, 100, 10, 30, 0); err != nil {
		//if err := pool1.SetMode(routine_pool.MODE_STATIC, 1000, 100, 0, 0, 0); err != nil {
		fmt.Println(err)
		return
	}

	pool1.SetQueue(1000, false)
	pool1.StartUp()
	fmt.Println(pool1)

	start := time.Now().UnixNano()
	cnt := 0

	for i := 0; i < 10000; i++ {
		if err := pool1.RegTask(handler.TestHandler, 123456789); err != nil {
			fmt.Println(err)
			continue
		}
		cnt++

		time.Sleep(time.Millisecond * 0)
	}

	fmt.Println(time.Now().UnixNano() - start)

	for {
		time.Sleep(time.Second)
	}
}
