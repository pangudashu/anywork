package handler

import (
	"fmt"
	"time"
)

func TestHandler(arg interface{}) {

	//panic("ddd")

	fmt.Println(arg)
	time.Sleep(time.Millisecond * 5)
}
