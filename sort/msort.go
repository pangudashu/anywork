//冒泡排序
package main

import (
	"fmt"
)

func main() {
	arr := []int{4, 7, 1, 4, 6, 10, 6, 9, 2}

	for i := 0; i < len(arr)-1; i++ {
		for j := 0; j < len(arr)-i-1; j++ {
			if arr[j] > arr[j+1] {
				arr[j], arr[j+1] = arr[j+1], arr[j]
			}
		}
	}

	fmt.Println(arr)
}
