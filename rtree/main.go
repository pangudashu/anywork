package main

import (
	"fmt"
	"rtree/rtree"
)

const (
	DIMENSION = 2
)

func main() {
	rtree.DIM = DIMENSION
	rtree.SetUnitSphereVolume(2)

	r1 := &rtree.Rect{
		Boundary: [][2]float64{{3, 5}, {2, 7}},
	}

	r2 := &rtree.Rect{
		Boundary: [][2]float64{{1, 2.5}, {7.5, 10}},
	}

	r3 := &rtree.Rect{
		Boundary: [][2]float64{{6, 9}, {11, 14}},
	}

	r4 := &rtree.Rect{
		Boundary: [][2]float64{{10, 13}, {15, 17}},
	}

	root_node := rtree.NewNode()

	root_node = rtree.Insert(root_node, r1)
	root_node = rtree.Insert(root_node, r2)
	root_node = rtree.Insert(root_node, r3)
	root_node = rtree.Insert(root_node, r4)

	/*
		fmt.Println(root_node)
		fmt.Println(root_node.BranchList[0].Child)
		fmt.Println(root_node.BranchList[1].Child)
	*/

	search_rect := &rtree.Rect{
		Boundary: [][2]float64{{3, 10}, {2, 12}},
	}
	ret := make([]*rtree.Rect, 0)
	search_cnt := 0
	rtree.Search(root_node, search_rect, &ret, &search_cnt)

	for _, v := range ret {
		fmt.Println(v)
	}
	fmt.Println(search_cnt)
}
