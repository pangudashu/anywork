//选择排序
package main

import (
	"fmt"
)

func main() {
	arr := []int{4, 7, 1, 4, 6, 10, 6, 9, 2}

	for i := 0; i < len(arr); i++ {
		min := i
		for j := i; j < len(arr); j++ {
			if arr[j] < arr[min] {
				min = j
			}
		}
		arr[i], arr[min] = arr[min], arr[i]
	}

	fmt.Println(arr)
}
