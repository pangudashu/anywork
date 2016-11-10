package main

import (
	"fmt"
	"math/rand"
	"time"
)

const MAX_LEVEL = 16

type skip_list struct {
	level  int
	header *node
}

type node struct {
	key     int
	value   interface{}
	forward []*node
}

var list *skip_list

var search_cnt int = 0

func main() {
	header := newNodeWithLevel(MAX_LEVEL)
	header.key = 0

	for i := 0; i < MAX_LEVEL; i++ {
		header.forward[i] = nil
	}

	list = &skip_list{
		level:  0,
		header: header,
	}

	//for i := 1; i < 10000; i++ {
	//	insert(list, i, "b")
	//}

	insert(list, 99, "")
	insert(list, 44, "")
	insert(list, 88, "")
	insert(list, 99, "")
	insert(list, 55, "")
	insert(list, 66, "")
	//for i := 5000; i > 0; i-- {
	//	get(list, i)
	//}

	//fmt.Println(list.header.forward)
	x := list.header.forward[0]
	for x != nil {
		fmt.Println(x)
		//fmt.Printf("%p", x)
		x = x.forward[0]
	}

	//fmt.Println(search_cnt, search_cnt/5000)

	//fmt.Println(list)
}

func get(list *skip_list, key int) {

	i := 0
	x := list.header
	for l := list.level; l >= 0; l-- {
		i++
		j := 0
		for x.forward[l] != nil && x.forward[l].key <= key {
			j++
			if x.forward[l].key == key {
				search_cnt += i * j
				fmt.Println( /*x.forward[l],*/ i * j)
				return
			}
			x = x.forward[l]
		}
	}

}

func insert(list *skip_list, key int, value interface{}) *node {
	updates := make([]*node, MAX_LEVEL)

	x := list.header
	for l := list.level; l >= 0; l-- {
		for x.forward[l] != nil && x.forward[l].key < key {
			x = x.forward[l]
		}
		updates[l] = x
	}

	var level int
	level = createRandLevel()
	if level > list.level {
		for i := list.level; i < level; i++ {
			updates[i] = list.header
		}
		list.level = level
	}

	newNode := newNodeWithLevel(level)
	newNode.key = key
	newNode.value = value

	for i := 0; i < level; i++ {
		newNode.forward[i] = updates[i].forward[i]
		updates[i].forward[i] = newNode
	}

	return newNode
}

func createRandLevel() int {
	rand.Seed(time.Now().UnixNano())
	return rand.Intn(MAX_LEVEL-1) + 1
}

func newNodeWithLevel(level int) *node {
	n := &node{}
	n.forward = make([]*node, level)

	return n
}
