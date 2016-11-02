package main

import (
	"fmt"
)

func main() {
	data := []int{100, 100, 10, 3, 6, 22, 77, 1, 8, 44}

	fmt.Println("befor sort:", data)
	qsort(data, 0, len(data)-1)

	fmt.Println("after sort:", data)
}

func qsort(data []int, start, end int) {
	if start >= end {
		return
	}

	pivot := start
	a := start
	b := end

	for {
		for ; a < b && data[a] < data[pivot]; a++ {
		}

		for ; a < b && data[b] >= data[pivot]; b-- {
		}
		if a >= b {
			break
		}
		data[a], data[b] = data[b], data[a]
	}

	//data[a] >= data[pivot]
	//data[b] < data[pivot]
	data[pivot], data[b] = data[b], data[pivot]

	qsort(data, start, b)
	qsort(data, b+1, end)
}
