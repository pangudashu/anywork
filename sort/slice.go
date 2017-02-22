//选择排序
package main

import (
	"fmt"
)

func main() {
	arr := []int{4, 7, 1, 4, 6, 10, 6, 9, 2} //奇数、偶数

	i := 0
	j := len(arr) - 1
	for {
		for ; i < j; i++ {
			if arr[i]%2 == 0 {
				break
			}
		}

		for ; j > i; j-- {
			if arr[j]%2 != 0 {
				break
			}
		}
		if i == j {
			break
		}
		arr[i], arr[j] = arr[j], arr[i]
	}

	fmt.Println(arr)

}
