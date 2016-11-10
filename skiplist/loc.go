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
	keys    []int
	forward []*node
}

const (
	_gt_ = iota // >
	_lt_        //<
	_eq_        //=
	_ge_        //>=
	_le_        //<=
)

type search_request struct {
	key_index int
	expr      int
	val       int
}

var list *skip_list

var search_cnt int = 0

func init() {
}

func main() {
	keys := []int{0, 0}
	header := newNodeWithLevel(keys, MAX_LEVEL)

	for i := 0; i < MAX_LEVEL; i++ {
		header.forward[i] = nil
	}

	list = &skip_list{
		level:  0,
		header: header,
	}

	insert(list, []int{99, 2})
	insert(list, []int{99, 1})
	insert(list, []int{88, 1})
	insert(list, []int{44, 1})
	insert(list, []int{66, 5})
	insert(list, []int{99, 2})
	insert(list, []int{99, 4})
	insert(list, []int{99, 1})
	insert(list, []int{99, 8})
	insert(list, []int{99, 10})
	insert(list, []int{99, 3})
	insert(list, []int{99, 2})

	//fmt.Println(list.header.forward)
	x := list.header.forward[0]
	for x != nil {
		fmt.Println(x)
		//fmt.Printf("%p", x)
		x = x.forward[0]
	}

	//search by range
	//key 1 : 45 < k1 < 99
	key_request := make([]*search_request, 2)

	key_request[0] = &search_request{
		key_index: 0,
		expr:      _gt_,
		val:       45,
	}

	key_request[1] = &search_request{
		key_index: 1,
		expr:      _lt_,
		val:       5,
	}

	fmt.Println("===========================")
	min_node := searchByRange(list, 99, 2)

	fmt.Println(min_node)
	fmt.Println("===========================")
	for min_node.forward[0] != nil {
		if min_node.keys[0] >= 99 && min_node.keys[1] >= 2 {
			fmt.Println(min_node)
		}
		min_node = min_node.forward[0]
	}
	fmt.Println(min_node)

	//fmt.Println(list)
}

func searchByRange(list *skip_list, min int, min2 int) *node {
	x := list.header

	//key_index := 0
	a := 0
	b := 0
	for i := list.level; i >= 0; i-- {
		a++
		for x.forward[i] != nil {
			b++
			if x.forward[i].keys[0] < min {
				x = x.forward[i]
				continue
			}
			break
		}
	}
	fmt.Println(a, b)

	if x.forward[0].keys[0] >= min {
		return x.forward[0]
	}

	return nil
}

func verifyExpr(req []*search_request, d *node) bool {
	is_true := false
	for index, r := range req {
		key_val := d.keys[index]
		switch r.expr {
		case _gt_:
			is_true = (key_val > r.val)
		case _lt_:
			is_true = (key_val < r.val)
		case _eq_:
			is_true = (key_val == r.val)
		case _ge_:
			is_true = (key_val >= r.val)
		case _le_:
			is_true = (key_val <= r.val)
		default:
			is_true = false
		}
	}

	return is_true
}

//插入
func insert(list *skip_list, keys []int) *node {
	update_nodes := make([]*node, MAX_LEVEL)
	x := list.header

	key_cnt := len(keys)

	//find insert position
	for i := list.level; i >= 0; i-- {
		key_index := 0
		for x.forward[i] != nil && key_index < key_cnt {
			if x.forward[i].keys[key_index] < keys[key_index] {
				x = x.forward[i]
			} else if x.forward[i].keys[key_index] == keys[key_index] {
				key_index++
			} else {
				break
			}
		}
		update_nodes[i] = x
	}

	//create new node forward level by random
	level := createRandLevel()
	if level > list.level {
		for i := list.level; i < level; i++ {
			update_nodes[i] = list.header
		}
		list.level = level
	}

	new_node := newNodeWithLevel(keys, level)

	//update forward point
	for i := 0; i < level; i++ {
		new_node.forward[i] = update_nodes[i].forward[i]
		update_nodes[i].forward[i] = new_node
	}

	return new_node
}

func createRandLevel() int {
	rand.Seed(time.Now().UnixNano())
	return rand.Intn(MAX_LEVEL-1) + 1
}

func newNodeWithLevel(keys []int, level int) *node {
	n := &node{}
	n.keys = keys
	n.forward = make([]*node, level)

	return n
}
